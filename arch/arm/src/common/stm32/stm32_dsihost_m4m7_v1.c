/****************************************************************************
 * arch/arm/src/common/stm32/stm32_dsihost_m4m7_v1.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/* DSI host of the STM32F7, STM32H7 and STM32L4+, driven by the LTDC in
 * video mode or adapted command mode.  It is registered as a MIPI-DSI host,
 * so panel drivers use the generic mipi_dsi_*() interface for commands.
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <assert.h>
#include <debug.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>
#include <nuttx/mutex.h>
#include <nuttx/video/mipi_display.h>
#include <nuttx/video/mipi_dsi.h>

#include <arch/board/board.h>

#include "arm_internal.h"
#include "chip.h"
#include "stm32_dsihost.h"

#ifdef CONFIG_STM32_DSIHOST

#ifndef CONFIG_MIPI_DSI
#  error "The DSI host needs CONFIG_MIPI_DSI"
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Timeout for the regulator, PLL, PHY and FIFOs */

#define DSI_TIMEOUT_US          (100 * 1000)

/* Timeout for a refresh, longer than one frame */

#define DSI_REFRESH_TIMEOUT_US  (100 * 1000)

/* Null packet size in video mode, as large as possible */

#define DSI_NULL_PACKET_SIZE    0xfff

/* The DSI host interrupt is named differently on the STM32F4 */

#if !defined(STM32_IRQ_DSIHOST) && defined(STM32_IRQ_DSI)
#  define STM32_IRQ_DSIHOST     STM32_IRQ_DSI
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct stm32_dsihost_s
{
  struct mipi_dsi_host host;              /* Generic host, must be first */
  struct stm32_dsihost_config_s config;   /* Host configuration */
  mutex_t lock;                           /* Serializes register access */
  bool initialized;
  uint8_t channel;                        /* Virtual channel of the device */

  /* Adapted command mode */

  stm32_dsihost_callback_t te;            /* Tearing effect callback */
  stm32_dsihost_callback_t refreshed;     /* End of refresh callback */
  FAR void *arg;                          /* Argument of the callbacks */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int stm32_dsihost_attach(FAR struct mipi_dsi_host *host,
                                FAR struct mipi_dsi_device *device);
static int stm32_dsihost_detach(FAR struct mipi_dsi_host *host,
                                FAR struct mipi_dsi_device *device);
static ssize_t stm32_dsihost_transfer(FAR struct mipi_dsi_host *host,
                                      FAR const struct mipi_dsi_msg *msg);
static int stm32_dsihost_interrupt(int irq, FAR void *context,
                                   FAR void *arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct mipi_dsi_host_ops g_dsihost_ops =
{
  .attach   = stm32_dsihost_attach,
  .detach   = stm32_dsihost_detach,
  .transfer = stm32_dsihost_transfer,
};

static struct stm32_dsihost_s g_dsihost =
{
  .host =
    {
      .bus = 0,
      .ops = &g_dsihost_ops,
    },
  .lock = NXMUTEX_INITIALIZER,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_dsihost_waitbits
 *
 * Description:
 *   Wait until the bits in mask of the register read as value.
 *
 ****************************************************************************/

static int stm32_dsihost_waitbits(uint32_t addr, uint32_t mask,
                                  uint32_t value, uint32_t timeout_us)
{
  while ((getreg32(addr) & mask) != value)
    {
      if (timeout_us < 10)
        {
          return -ETIMEDOUT;
        }

      up_udelay(10);
      timeout_us -= 10;
    }

  return OK;
}

/****************************************************************************
 * Name: stm32_dsihost_odf
 *
 * Description:
 *   Return the register encoding of the PLL output division factor.
 *
 ****************************************************************************/

static int stm32_dsihost_odf(uint8_t odf)
{
  switch (odf)
    {
      case 1:
        return DSI_WRPCR_ODF_1;

      case 2:
        return DSI_WRPCR_ODF_2;

      case 4:
        return DSI_WRPCR_ODF_4;

      case 8:
        return DSI_WRPCR_ODF_8;

      default:
        return -EINVAL;
    }
}

/****************************************************************************
 * Name: stm32_dsihost_colorcoding
 *
 * Description:
 *   Return the LTDC color coding and the wrapper color multiplexing for a
 *   MIPI-DSI pixel format.
 *
 ****************************************************************************/

static int stm32_dsihost_colorcoding(uint8_t format, FAR uint32_t *colc,
                                     FAR uint32_t *colmux)
{
  switch (format)
    {
      case MIPI_DSI_FMT_RGB888:
        *colc   = DSI_LCOLCR_COLC_RGB888;
        *colmux = DSI_WCFGR_COLMUX_RGB888;
        break;

      case MIPI_DSI_FMT_RGB666:
        *colc   = DSI_LCOLCR_COLC_RGB666 | DSI_LCOLCR_LPE;
        *colmux = DSI_WCFGR_COLMUX_RGB666;
        break;

      case MIPI_DSI_FMT_RGB666_PACKED:
        *colc   = DSI_LCOLCR_COLC_RGB666;
        *colmux = DSI_WCFGR_COLMUX_RGB666;
        break;

      case MIPI_DSI_FMT_RGB565:
        *colc   = DSI_LCOLCR_COLC_RGB565;
        *colmux = DSI_WCFGR_COLMUX_RGB565;
        break;

      default:
        return -ENOTSUP;
    }

  return OK;
}

/****************************************************************************
 * Name: stm32_dsihost_lanebyteclk
 *
 * Description:
 *   Return the lane byte clock in Hz.
 *
 ****************************************************************************/

static uint32_t stm32_dsihost_lanebyteclk(void)
{
  FAR const struct stm32_dsihost_config_s *config = &g_dsihost.config;

  return (uint32_t)(((uint64_t)STM32_HSE_FREQUENCY * config->pll_ndiv) /
                    ((uint64_t)config->pll_idf * config->pll_odf * 8));
}

/****************************************************************************
 * Name: stm32_dsihost_tolanebyte
 *
 * Description:
 *   Convert a number of pixel clocks to lane byte clock cycles.
 *
 ****************************************************************************/

static uint32_t stm32_dsihost_tolanebyte(uint32_t pixels)
{
  FAR const struct stm32_dsihost_video_s *video = &g_dsihost.config.video;

  return (uint32_t)(((uint64_t)pixels * stm32_dsihost_lanebyteclk()) /
                    video->pixelclock);
}

/****************************************************************************
 * Name: stm32_dsihost_videomode
 *
 * Description:
 *   Configure video mode from the video timing of the configuration.
 *
 ****************************************************************************/

static void stm32_dsihost_videomode(FAR struct mipi_dsi_device *device,
                                    uint32_t colmux)
{
  FAR const struct stm32_dsihost_video_s *video = &g_dsihost.config.video;
  uint32_t regval;

  modifyreg32(STM32_DSI_MCR, DSI_MCR_CMDM, 0);
  putreg32(colmux, STM32_DSI_WCFGR);

  /* Burst or non-burst mode.  The host returns to low-power in all the
   * blanking periods and may send commands there.
   */

  if ((device->mode_flags & MIPI_DSI_MODE_VIDEO_BURST) != 0)
    {
      regval = DSI_VMCR_VMT_BURST;
    }
  else if ((device->mode_flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE) != 0)
    {
      regval = DSI_VMCR_VMT_PULSES;
    }
  else
    {
      regval = DSI_VMCR_VMT_EVENTS;
    }

  regval |= DSI_VMCR_LPVSAE | DSI_VMCR_LPVBPE | DSI_VMCR_LPVFPE |
            DSI_VMCR_LPVAE | DSI_VMCR_LPHBPE | DSI_VMCR_LPHFPE |
            DSI_VMCR_LPCE;
  putreg32(regval, STM32_DSI_VMCR);

  /* One packet per line, no chunks */

  putreg32(video->hactive & DSI_VPCR_VPSIZE_MASK, STM32_DSI_VPCR);
  putreg32(0, STM32_DSI_VCCR);
  putreg32(DSI_NULL_PACKET_SIZE, STM32_DSI_VNPCR);

  /* Horizontal timing in lane byte clock cycles, vertical in lines */

  putreg32(stm32_dsihost_tolanebyte(video->hsync) & DSI_VHSACR_HSA_MASK,
           STM32_DSI_VHSACR);
  putreg32(stm32_dsihost_tolanebyte(video->hbp) & DSI_VHBPCR_HBP_MASK,
           STM32_DSI_VHBPCR);
  putreg32(stm32_dsihost_tolanebyte(video->hactive + video->hsync +
                                    video->hbp + video->hfp) &
           DSI_VLCR_HLINE_MASK, STM32_DSI_VLCR);
  putreg32(video->vsync & DSI_VVSACR_VSA_MASK, STM32_DSI_VVSACR);
  putreg32(video->vbp & DSI_VVBPCR_VBP_MASK, STM32_DSI_VVBPCR);
  putreg32(video->vfp & DSI_VVFPCR_VFP_MASK, STM32_DSI_VVFPCR);
  putreg32(video->vactive & DSI_VVACR_VA_MASK, STM32_DSI_VVACR);

  putreg32(DSI_LPMCR_LPSIZE(video->lpsize) |
           DSI_LPMCR_VLPSIZE(video->vlpsize), STM32_DSI_LPMCR);
}

/****************************************************************************
 * Name: stm32_dsihost_cmdmode
 *
 * Description:
 *   Configure adapted command mode: the LTDC is the pixel source and the
 *   wrapper turns its frames into memory write commands.
 *
 ****************************************************************************/

static void stm32_dsihost_cmdmode(uint32_t colmux)
{
  FAR const struct stm32_dsihost_config_s *config = &g_dsihost.config;
  uint32_t regval;

  modifyreg32(STM32_DSI_MCR, 0, DSI_MCR_CMDM);

  regval = DSI_WCFGR_DSIM | colmux;
  if (config->te_pin)
    {
      regval |= DSI_WCFGR_TESRC;
    }

  if (config->te_falling)
    {
      regval |= DSI_WCFGR_TEPOL;
    }

  if (config->autorefresh)
    {
      regval |= DSI_WCFGR_AR;
    }

  putreg32(regval, STM32_DSI_WCFGR);
  putreg32(DSI_LCCR_CMDSIZE(config->cmdsize), STM32_DSI_LCCR);
}

/****************************************************************************
 * Name: stm32_dsihost_attach
 *
 * Description:
 *   Configure the host for the device and enable it.
 *
 ****************************************************************************/

static int stm32_dsihost_attach(FAR struct mipi_dsi_host *host,
                                FAR struct mipi_dsi_device *device)
{
  FAR struct stm32_dsihost_s *priv = (FAR struct stm32_dsihost_s *)host;
  FAR const struct stm32_dsihost_config_s *config = &priv->config;
  bool video;
  uint32_t colc;
  uint32_t colmux;
  uint32_t regval;
  int ret;

  DEBUGASSERT(priv != NULL && device != NULL);

  if (device->lanes != config->lanes || device->channel > 3)
    {
      return -EINVAL;
    }

  video = (device->mode_flags & MIPI_DSI_MODE_VIDEO) != 0;
  if (video && config->video.pixelclock == 0)
    {
      return -EINVAL;
    }

  ret = stm32_dsihost_colorcoding(device->format, &colc, &colmux);
  if (ret < 0)
    {
      return ret;
    }

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  if (video)
    {
      stm32_dsihost_videomode(device, colmux);
    }
  else
    {
      stm32_dsihost_cmdmode(colmux);
    }

  putreg32(device->channel, STM32_DSI_LVCIDR);
  priv->channel = device->channel;

  regval = 0;
  if (config->de_active_low)
    {
      regval |= DSI_LPCR_DEP;
    }

  if (config->vsync_active_low)
    {
      regval |= DSI_LPCR_VSP;
    }

  if (config->hsync_active_low)
    {
      regval |= DSI_LPCR_HSP;
    }

  putreg32(regval, STM32_DSI_LPCR);
  putreg32(colc, STM32_DSI_LCOLCR);

  /* Send commands in low-power mode if the device asks for it */

  regval = 0;
  if ((device->mode_flags & MIPI_DSI_MODE_LPM) != 0)
    {
      regval = DSI_CMCR_GSW0TX | DSI_CMCR_GSW1TX | DSI_CMCR_GSW2TX |
               DSI_CMCR_GSR0TX | DSI_CMCR_GSR1TX | DSI_CMCR_GSR2TX |
               DSI_CMCR_GLWTX | DSI_CMCR_DSW0TX | DSI_CMCR_DSW1TX |
               DSI_CMCR_DSR0TX | DSI_CMCR_DLWTX | DSI_CMCR_MRDPS;
    }

  if (!video && config->te_ack)
    {
      regval |= DSI_CMCR_TEARE;
    }

  putreg32(regval, STM32_DSI_CMCR);

  /* Bus turn around, needed for reads and acknowledges */

  putreg32(DSI_PCR_BTAE, STM32_DSI_PCR);

  modifyreg32(STM32_DSI_CR, 0, DSI_CR_EN);

  nxmutex_unlock(&priv->lock);
  return OK;
}

/****************************************************************************
 * Name: stm32_dsihost_detach
 ****************************************************************************/

static int stm32_dsihost_detach(FAR struct mipi_dsi_host *host,
                                FAR struct mipi_dsi_device *device)
{
  FAR struct stm32_dsihost_s *priv = (FAR struct stm32_dsihost_s *)host;
  int ret;

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  modifyreg32(STM32_DSI_WCR, DSI_WCR_DSIEN, 0);
  modifyreg32(STM32_DSI_CR, DSI_CR_EN, 0);

  nxmutex_unlock(&priv->lock);
  return OK;
}

/****************************************************************************
 * Name: stm32_dsihost_sendpacket
 *
 * Description:
 *   Send a packet through the generic interface.  The payload goes into
 *   the FIFO before the header that sends it.  The hardware adds the ECC.
 *
 ****************************************************************************/

static int stm32_dsihost_sendpacket(FAR const struct mipi_dsi_packet *packet)
{
  irqstate_t flags;
  uint32_t word;
  size_t i;
  int ret;

  ret = stm32_dsihost_waitbits(STM32_DSI_GPSR, DSI_GPSR_CMDFE,
                               DSI_GPSR_CMDFE, DSI_TIMEOUT_US);
  if (ret < 0)
    {
      return ret;
    }

  /* The refresh in adapted command mode sends packets from the interrupt
   * handler.  Keep the payload and the header of a packet together.
   */

  flags = enter_critical_section();

  for (i = 0; i < packet->payload_length; i += 4)
    {
      word = packet->payload[i];
      if (i + 1 < packet->payload_length)
        {
          word |= (uint32_t)packet->payload[i + 1] << 8;
        }

      if (i + 2 < packet->payload_length)
        {
          word |= (uint32_t)packet->payload[i + 2] << 16;
        }

      if (i + 3 < packet->payload_length)
        {
          word |= (uint32_t)packet->payload[i + 3] << 24;
        }

      ret = stm32_dsihost_waitbits(STM32_DSI_GPSR, DSI_GPSR_PWRFF, 0,
                                   DSI_TIMEOUT_US);
      if (ret < 0)
        {
          leave_critical_section(flags);
          return ret;
        }

      putreg32(word, STM32_DSI_GPDR);
    }

  putreg32(packet->header[0] | DSI_GHCR_WCLSB(packet->header[1]) |
           DSI_GHCR_WCMSB(packet->header[2]), STM32_DSI_GHCR);
  leave_critical_section(flags);
  return OK;
}

/****************************************************************************
 * Name: stm32_dsihost_receive
 *
 * Description:
 *   Collect the response to a read request from the payload read FIFO.
 *
 ****************************************************************************/

static ssize_t stm32_dsihost_receive(FAR uint8_t *buffer, size_t len)
{
  uint32_t timeout = DSI_TIMEOUT_US;
  uint32_t word;
  size_t count = 0;
  int i;

  while (count < len)
    {
      if ((getreg32(STM32_DSI_GPSR) & DSI_GPSR_PRDFE) == 0)
        {
          word = getreg32(STM32_DSI_GPDR);
          for (i = 0; i < 4 && count < len; i++)
            {
              buffer[count++] = word & 0xff;
              word >>= 8;
            }

          continue;
        }

      /* The read is over without data: the peripheral sent an error or
       * less than asked for.
       */

      if ((getreg32(STM32_DSI_GPSR) & DSI_GPSR_RCB) == 0 &&
          (getreg32(STM32_DSI_ISR1) & DSI_ISR1_PSE) != 0)
        {
          return -EIO;
        }

      if (timeout < 10)
        {
          return count > 0 ? (ssize_t)count : -ETIMEDOUT;
        }

      up_udelay(10);
      timeout -= 10;
    }

  return count;
}

/****************************************************************************
 * Name: stm32_dsihost_transfer
 *
 * Description:
 *   Send a packet and, for a read request, receive the response.
 *
 ****************************************************************************/

static ssize_t stm32_dsihost_transfer(FAR struct mipi_dsi_host *host,
                                      FAR const struct mipi_dsi_msg *msg)
{
  FAR struct stm32_dsihost_s *priv = (FAR struct stm32_dsihost_s *)host;
  struct mipi_dsi_packet packet;
  ssize_t nbytes;
  int ret;

  DEBUGASSERT(priv != NULL && msg != NULL);

  ret = mipi_dsi_create_packet(&packet, msg);
  if (ret < 0)
    {
      return ret;
    }

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  ret = stm32_dsihost_sendpacket(&packet);
  if (ret < 0)
    {
      verr("ERROR: DSI FIFO timeout\n");
      nxmutex_unlock(&priv->lock);
      return ret;
    }

  if (msg->rx_len > 0 && msg->rx_buf != NULL)
    {
      nbytes = stm32_dsihost_receive(msg->rx_buf, msg->rx_len);
    }
  else
    {
      nbytes = msg->tx_len;
    }

  nxmutex_unlock(&priv->lock);
  return nbytes;
}

/****************************************************************************
 * Name: stm32_dsihost_interrupt
 *
 * Description:
 *   Wrapper interrupts of the adapted command mode.
 *
 ****************************************************************************/

static int stm32_dsihost_interrupt(int irq, FAR void *context,
                                   FAR void *arg)
{
  FAR struct stm32_dsihost_s *priv = (FAR struct stm32_dsihost_s *)arg;
  uint32_t status;

  status = getreg32(STM32_DSI_WISR) & getreg32(STM32_DSI_WIER);

  if ((status & DSI_WISR_TEIF) != 0)
    {
      putreg32(DSI_WIFCR_CTEIF, STM32_DSI_WIFCR);
      priv->te(priv->arg);
    }

  if ((status & DSI_WISR_ERIF) != 0)
    {
      putreg32(DSI_WIFCR_CERIF, STM32_DSI_WIFCR);
      priv->refreshed(priv->arg);
    }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_dsihost_initialize
 *
 * Description:
 *   Power up and configure the DSI host and its PHY, and register it as
 *   MIPI-DSI host 0.
 *
 ****************************************************************************/

int stm32_dsihost_initialize(FAR const struct stm32_dsihost_config_s *config)
{
  FAR struct stm32_dsihost_s *priv = &g_dsihost;
  uint32_t stopstate;
  uint32_t regval;
  uint32_t uix4;
  uint16_t clktime;
  int odf;
  int ret;
  int i;

  DEBUGASSERT(config != NULL);

  odf = stm32_dsihost_odf(config->pll_odf);
  if (odf < 0 || config->pll_idf < 1 || config->pll_idf > 7 ||
      config->pll_ndiv < 10 || config->pll_ndiv > 125 ||
      config->lanes < 1 || config->lanes > 2)
    {
      return -EINVAL;
    }

  if (priv->initialized)
    {
      return -EBUSY;
    }

  priv->config = *config;

  /* Enable the regulator */

  modifyreg32(STM32_DSI_WRPCR, 0, DSI_WRPCR_REGEN);
  ret = stm32_dsihost_waitbits(STM32_DSI_WISR, DSI_WISR_RRS, DSI_WISR_RRS,
                               DSI_TIMEOUT_US);
  if (ret < 0)
    {
      verr("ERROR: DSI regulator not ready\n");
      return ret;
    }

  /* Configure and enable the PLL */

  regval  = getreg32(STM32_DSI_WRPCR);
  regval &= ~(DSI_WRPCR_NDIV_MASK | DSI_WRPCR_IDF_MASK |
              DSI_WRPCR_ODF_MASK);
  regval |= DSI_WRPCR_NDIV(config->pll_ndiv) |
            DSI_WRPCR_IDF(config->pll_idf) | (uint32_t)odf;
  putreg32(regval, STM32_DSI_WRPCR);

  modifyreg32(STM32_DSI_WRPCR, 0, DSI_WRPCR_PLLEN);
  up_mdelay(1);

  ret = stm32_dsihost_waitbits(STM32_DSI_WISR, DSI_WISR_PLLLS,
                               DSI_WISR_PLLLS, DSI_TIMEOUT_US);
  if (ret < 0)
    {
      verr("ERROR: DSI PLL not locked\n");
      return ret;
    }

  /* Enable the host and the PHY and wait for the lanes to reach the stop
   * state.
   */

  modifyreg32(STM32_DSI_CR, 0, DSI_CR_EN);
  modifyreg32(STM32_DSI_CCR, DSI_CCR_TXECKDIV_MASK,
              DSI_CCR_TXECKDIV(config->txesc_div));
  modifyreg32(STM32_DSI_PCTLR, 0, DSI_PCTLR_DEN | DSI_PCTLR_CKE);
  modifyreg32(STM32_DSI_PCONFR, DSI_PCONFR_NL_MASK,
              config->lanes == 2 ? DSI_PCONFR_NL_TWO : DSI_PCONFR_NL_ONE);

  stopstate = DSI_PSR_PSS0 | DSI_PSR_PSSC;
  if (config->lanes == 2)
    {
      stopstate |= DSI_PSR_PSS1;
    }

  ret = stm32_dsihost_waitbits(STM32_DSI_PSR, stopstate, stopstate,
                               DSI_TIMEOUT_US);
  if (ret < 0)
    {
      verr("ERROR: DSI lanes not in stop state\n");
      modifyreg32(STM32_DSI_CR, DSI_CR_EN, 0);
      return ret;
    }

  /* Unit interval times 4, in ns */

  uix4 = (4000000 * config->pll_idf * config->pll_odf) /
         ((STM32_HSE_FREQUENCY / 1000) * config->pll_ndiv);
  modifyreg32(STM32_DSI_WPCR0, DSI_WPCR0_UIX4_MASK, DSI_WPCR0_UIX4(uix4));

  putreg32(0, STM32_DSI_IER0);
  putreg32(0, STM32_DSI_IER1);

  modifyreg32(STM32_DSI_CR, DSI_CR_EN, 0);

  /* The clock lane is driven by the D-PHY and stays in high-speed */

  putreg32(DSI_CLCR_DPCC, STM32_DSI_CLCR);

  /* PHY timers.  The host computes the clock lane LP2HS + HS2LP as twice
   * HS2LP, so both get the larger of the two.
   */

  clktime = config->clk_lp2hs > config->clk_hs2lp ?
            config->clk_lp2hs : config->clk_hs2lp;
  putreg32(DSI_CLTCR_LP2HS(clktime) | DSI_CLTCR_HS2LP(clktime),
           STM32_DSI_CLTCR);
  putreg32(DSI_DLTCR_LP2HS(config->data_lp2hs) |
           DSI_DLTCR_HS2LP(config->data_hs2lp), STM32_DSI_DLTCR);
  modifyreg32(STM32_DSI_PCONFR, DSI_PCONFR_SWTIME_MASK,
              DSI_PCONFR_SWTIME(config->stop_wait));

  /* No host timeouts */

  modifyreg32(STM32_DSI_CCR, DSI_CCR_TOCKDIV_MASK, DSI_CCR_TOCKDIV(1));
  for (i = 0; i < 6; i++)
    {
      putreg32(0, STM32_DSI_TCCR(i));
    }

  ret = mipi_dsi_host_register(&priv->host);
  if (ret < 0)
    {
      verr("ERROR: mipi_dsi_host_register failed: %d\n", ret);
      return ret;
    }

  priv->initialized = true;
  return OK;
}

/****************************************************************************
 * Name: stm32_dsihost_start
 *
 * Description:
 *   Enable the DSI wrapper, which connects the LTDC to the DSI host.
 *
 ****************************************************************************/

int stm32_dsihost_start(void)
{
  FAR struct stm32_dsihost_s *priv = &g_dsihost;
  int ret;

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  modifyreg32(STM32_DSI_WCR, 0, DSI_WCR_DSIEN);

  nxmutex_unlock(&priv->lock);
  return OK;
}

/****************************************************************************
 * Name: stm32_dsihost_refresh
 *
 * Description:
 *   In adapted command mode, send the LTDC frame to the display.
 *
 ****************************************************************************/

int stm32_dsihost_refresh(void)
{
  FAR struct stm32_dsihost_s *priv = &g_dsihost;
  int ret;

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  ret = stm32_dsihost_waitbits(STM32_DSI_WISR, DSI_WISR_BUSY, 0,
                               DSI_REFRESH_TIMEOUT_US);
  if (ret < 0)
    {
      verr("ERROR: DSI refresh still busy\n");
      goto out;
    }

  putreg32(DSI_WIFCR_CERIF, STM32_DSI_WIFCR);
  modifyreg32(STM32_DSI_WCR, 0, DSI_WCR_LTDCEN);

out:
  nxmutex_unlock(&priv->lock);
  return ret;
}

/****************************************************************************
 * Name: stm32_dsihost_cmdattach
 *
 * Description:
 *   In adapted command mode, call te on each tearing effect event and
 *   refreshed at the end of each refresh.
 *
 ****************************************************************************/

int stm32_dsihost_cmdattach(stm32_dsihost_callback_t te,
                            stm32_dsihost_callback_t refreshed,
                            FAR void *arg)
{
  FAR struct stm32_dsihost_s *priv = &g_dsihost;
  int ret;

  DEBUGASSERT(te != NULL && refreshed != NULL);

  priv->te        = te;
  priv->refreshed = refreshed;
  priv->arg       = arg;

  putreg32(DSI_WIFCR_CTEIF | DSI_WIFCR_CERIF, STM32_DSI_WIFCR);

  ret = irq_attach(STM32_IRQ_DSIHOST, stm32_dsihost_interrupt, priv);
  if (ret < 0)
    {
      return ret;
    }

  putreg32(DSI_WIER_TEIE | DSI_WIER_ERIE, STM32_DSI_WIER);
  up_enable_irq(STM32_IRQ_DSIHOST);
  return OK;
}

/****************************************************************************
 * Name: stm32_dsihost_cmdrefresh
 *
 * Description:
 *   In adapted command mode, send the LTDC frame to the columns x0 to x1
 *   of the display memory.
 *
 ****************************************************************************/

void stm32_dsihost_cmdrefresh(uint16_t x0, uint16_t x1)
{
  FAR struct stm32_dsihost_s *priv = &g_dsihost;
  struct mipi_dsi_packet packet;
  struct mipi_dsi_msg msg;
  uint8_t columns[5];

  /* The memory write of the LTDC frame starts at the first column */

  columns[0] = MIPI_DCS_SET_COLUMN_ADDRESS;
  columns[1] = x0 >> 8;
  columns[2] = x0 & 0xff;
  columns[3] = x1 >> 8;
  columns[4] = x1 & 0xff;

  memset(&msg, 0, sizeof(msg));
  msg.channel = priv->channel;
  msg.type    = MIPI_DSI_DCS_LONG_WRITE;
  msg.tx_len  = sizeof(columns);
  msg.tx_buf  = columns;

  if (mipi_dsi_create_packet(&packet, &msg) < 0 ||
      stm32_dsihost_sendpacket(&packet) < 0)
    {
      verr("ERROR: Set column address failed\n");
      return;
    }

  modifyreg32(STM32_DSI_WCR, 0, DSI_WCR_LTDCEN);
}

/****************************************************************************
 * Name: stm32_dsihost_wrapper
 *
 * Description:
 *   Disconnect the LTDC from the DSI host, or connect it again.
 *
 ****************************************************************************/

void stm32_dsihost_wrapper(bool enable)
{
  modifyreg32(STM32_DSI_WCR, enable ? 0 : DSI_WCR_DSIEN,
              enable ? DSI_WCR_DSIEN : 0);
}

#endif /* CONFIG_STM32_DSIHOST */
