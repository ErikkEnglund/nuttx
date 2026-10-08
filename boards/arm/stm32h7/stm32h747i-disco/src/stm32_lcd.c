/****************************************************************************
 * boards/arm/stm32h7/stm32h747i-disco/src/stm32_lcd.c
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

/* The 4" 800x480 display of the MB1166 display board.  The LTDC scans the
 * framebuffer in SDRAM and the DSI host sends it to the panel over two
 * lanes in video burst mode.
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <debug.h>
#include <errno.h>
#include <stdbool.h>
#include <syslog.h>

#include <nuttx/arch.h>
#include <nuttx/video/fb.h>
#include <nuttx/video/mipi_dsi.h>

#include <arch/board/board.h>

#include "stm32_gpio.h"
#include "stm32_ltdc.h"
#include "stm32_dsihost.h"
#include "stm32h747i-disco.h"

#ifdef CONFIG_STM32H747I_DISCO_LCD

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define LCD_DSI_LANES   2

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* DSI host: two lanes at 500 Mbit/s from the 25 MHz HSE, as in the ST
 * board support package.  The PHY timers keep their reset values.
 */

static const struct stm32_dsihost_config_s g_dsiconfig =
{
  .pll_ndiv  = 100,
  .pll_idf   = 5,
  .pll_odf   = 1,
  .lanes     = LCD_DSI_LANES,
  .txesc_div = 4,
  .video     =
    {
      .pixelclock = STM32_PLL3R_FREQUENCY,
      .hactive    = BOARD_LTDC_WIDTH,
      .hsync      = BOARD_LTDC_HSYNC,
      .hbp        = BOARD_LTDC_HBP,
      .hfp        = BOARD_LTDC_HFP,
      .vactive    = BOARD_LTDC_HEIGHT,
      .vsync      = BOARD_LTDC_VSYNC,
      .vbp        = BOARD_LTDC_VBP,
      .vfp        = BOARD_LTDC_VFP,
      .lpsize     = 64,
      .vlpsize    = 64,
    },
};

static bool g_initialized;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_fbinitialize
 *
 * Description:
 *   Reset the display, start the DSI video stream from the LTDC and
 *   initialize the display controller.
 *
 ****************************************************************************/

int up_fbinitialize(int display)
{
  FAR struct mipi_dsi_device *device;
  int panel;
  int ret;

  if (display != 0)
    {
      return -EINVAL;
    }

  if (g_initialized)
    {
      return OK;
    }

  /* Reset the display controller and turn the backlight on */

  stm32_configgpio(GPIO_LCD_RESET);
  up_mdelay(20);
  stm32_gpiowrite(GPIO_LCD_RESET, true);
  up_mdelay(10);

  stm32_configgpio(GPIO_LCD_BL);
  stm32_gpiowrite(GPIO_LCD_BL, true);

  ret = stm32_dsihost_initialize(&g_dsiconfig);
  if (ret < 0)
    {
      verr("ERROR: DSI host initialization failed: %d\n", ret);
      return ret;
    }

  device = mipi_dsi_device_register(mipi_dsi_host_get(0), "mb1166", 0);
  if (device == NULL)
    {
      return -ENODEV;
    }

  device->lanes      = LCD_DSI_LANES;
  device->format     = MIPI_DSI_FMT_RGB888;
  device->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST;

  ret = mipi_dsi_attach(device);
  if (ret < 0)
    {
      verr("ERROR: mipi_dsi_attach failed: %d\n", ret);
      return ret;
    }

  /* The LTDC must run before the DSI video stream starts.  In video mode
   * the host sends commands in the blanking periods of the stream, so the
   * controller can only be probed and set up once the stream runs.
   */

  ret = stm32_ltdcinitialize();
  if (ret < 0)
    {
      verr("ERROR: LTDC initialization failed: %d\n", ret);
      return ret;
    }

  ret = stm32_dsihost_start();
  if (ret < 0)
    {
      return ret;
    }

  /* Find out which controller the display board has */

  panel = stm32_lcdpanel_probe(device);
  if (panel < 0)
    {
      verr("ERROR: No display controller found: %d\n", panel);
      return panel;
    }

  ret = stm32_lcdpanel_initialize(device, panel);
  if (ret < 0)
    {
      verr("ERROR: Display controller initialization failed: %d\n", ret);
      return ret;
    }

  g_initialized = true;
  return OK;
}

/****************************************************************************
 * Name: up_fbgetvplane
 ****************************************************************************/

FAR struct fb_vtable_s *up_fbgetvplane(int display, int vplane)
{
  if (display != 0)
    {
      return NULL;
    }

  return stm32_ltdcgetvplane(vplane);
}

/****************************************************************************
 * Name: up_fbuninitialize
 ****************************************************************************/

void up_fbuninitialize(int display)
{
}

#endif /* CONFIG_STM32H747I_DISCO_LCD */
