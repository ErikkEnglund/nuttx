/****************************************************************************
 * arch/arm/src/common/stm32/stm32_dsihost.h
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

#ifndef __ARCH_ARM_SRC_COMMON_STM32_STM32_DSIHOST_H
#define __ARCH_ARM_SRC_COMMON_STM32_STM32_DSIHOST_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdbool.h>
#include <stdint.h>

#include <nuttx/video/mipi_dsi.h>

#include "hardware/stm32_dsihost.h"

#ifdef CONFIG_STM32_DSIHOST

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Video timing of the LTDC, for video mode.  The horizontal values are in
 * pixels; the driver converts them to lane byte clock cycles.
 */

struct stm32_dsihost_video_s
{
  uint32_t pixelclock;         /* LTDC pixel clock in Hz */
  uint16_t hactive;            /* Active pixels per line */
  uint16_t hsync;              /* Horizontal sync width */
  uint16_t hbp;                /* Horizontal back porch */
  uint16_t hfp;                /* Horizontal front porch */
  uint16_t vactive;            /* Active lines */
  uint16_t vsync;              /* Vertical sync lines */
  uint16_t vbp;                /* Vertical back porch lines */
  uint16_t vfp;                /* Vertical front porch lines */
  uint8_t  lpsize;             /* Largest low-power packet in the blanking
                                * periods, in bytes */
  uint8_t  vlpsize;            /* Largest low-power packet in the active
                                * lines, in bytes */
};

/* DSI host configuration.
 *
 * The DSI PLL is clocked by the HSE, which must be running.  The lane bit
 * rate is
 *
 *   HSE / pll_idf * 2 * pll_ndiv / (2 * pll_odf)
 *
 * PHY timings are in lane byte clock cycles (lane bit rate / 8).
 *
 * The video settings apply when the attached device asks for
 * MIPI_DSI_MODE_VIDEO, the adapted command mode settings otherwise.  In
 * adapted command mode the LTDC sends a frame each time
 * stm32_dsihost_refresh() is called, or on each tearing effect event with
 * autorefresh.
 */

struct stm32_dsihost_config_s
{
  /* PLL */

  uint8_t  pll_ndiv;           /* Loop division factor, 10-125 */
  uint8_t  pll_idf;            /* Input division factor, 1-7 */
  uint8_t  pll_odf;            /* Output division factor, 1, 2, 4 or 8 */

  /* PHY */

  uint8_t  lanes;              /* Number of data lanes, 1 or 2 */
  uint8_t  txesc_div;          /* TX escape clock divider (max 20 MHz) */
  uint16_t clk_lp2hs;          /* Clock lane low-power to high-speed */
  uint16_t clk_hs2lp;          /* Clock lane high-speed to low-power */
  uint8_t  data_lp2hs;         /* Data lane low-power to high-speed */
  uint8_t  data_hs2lp;         /* Data lane high-speed to low-power */
  uint8_t  stop_wait;          /* Minimum wait period to request HS */

  /* Polarities of the LTDC signals as seen by the DSI host */

  bool     de_active_low;
  bool     vsync_active_low;
  bool     hsync_active_low;

  /* Video mode */

  struct stm32_dsihost_video_s video;

  /* Adapted command mode */

  uint16_t cmdsize;            /* Maximum pixels per memory write command */
  bool     te_pin;             /* Tearing effect on the TE pin, not the
                                * DSI link */
  bool     te_falling;         /* Tearing effect on the falling edge */
  bool     te_ack;             /* Request tearing effect acknowledge */
  bool     autorefresh;        /* Refresh on each tearing effect */
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Name: stm32_dsihost_initialize
 *
 * Description:
 *   Power up and configure the DSI host and its PHY, and register it as
 *   MIPI-DSI host 0.  The host stays disabled until a device is attached
 *   with mipi_dsi_attach(), which configures video or adapted command mode
 *   and enables the host.
 *
 * Input Parameters:
 *   config - DSI host configuration
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int stm32_dsihost_initialize(
      FAR const struct stm32_dsihost_config_s *config);

/****************************************************************************
 * Name: stm32_dsihost_start
 *
 * Description:
 *   Enable the DSI wrapper, which connects the LTDC to the DSI host.  In
 *   video mode this starts the video stream, so the LTDC must be running.
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int stm32_dsihost_start(void);

/****************************************************************************
 * Name: stm32_dsihost_refresh
 *
 * Description:
 *   In adapted command mode, send the LTDC frame to the display.  Waits for
 *   a refresh still in progress to finish first.
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int stm32_dsihost_refresh(void);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* CONFIG_STM32_DSIHOST */
#endif /* __ARCH_ARM_SRC_COMMON_STM32_STM32_DSIHOST_H */
