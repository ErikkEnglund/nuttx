/****************************************************************************
 * boards/arm/stm32h7/stm32h747i-disco/src/stm32h747i-disco.h
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

#ifndef __BOARDS_ARM_STM32H7_STM32H747I_DISCO_SRC_STM32H747I_DISCO_H
#define __BOARDS_ARM_STM32H7_STM32H747I_DISCO_SRC_STM32H747I_DISCO_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Configuration ************************************************************/

/* procfs File System */

#ifdef CONFIG_FS_PROCFS
#  ifdef CONFIG_NSH_PROC_MOUNTPOINT
#    define STM32_PROCFS_MOUNTPOINT CONFIG_NSH_PROC_MOUNTPOINT
#  else
#    define STM32_PROCFS_MOUNTPOINT "/proc"
#  endif
#endif

/* Check if we can support the RTC driver */

#define HAVE_RTC_DRIVER 1
#if !defined(CONFIG_RTC) || !defined(CONFIG_RTC_DRIVER)
#  undef HAVE_RTC_DRIVER
#endif

/* LED
 *
 * LD1 Green   PI12
 * LD2 Orange  PI13
 * LD3 Red     PI14
 * LD4 Blue    PI15
 */

#define GPIO_LD1        (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | \
                         GPIO_PORTI | GPIO_PIN12)
#define GPIO_LD2        (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | \
                         GPIO_PORTI | GPIO_PIN13)
#define GPIO_LD3        (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | \
                         GPIO_PORTI | GPIO_PIN14)
#define GPIO_LD4        (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | \
                         GPIO_PORTI | GPIO_PIN15)

#define GPIO_LED_GREEN  GPIO_LD1
#define GPIO_LED_ORANGE GPIO_LD2
#define GPIO_LED_RED    GPIO_LD3
#define GPIO_LED_BLUE   GPIO_LD4

#define LED_DRIVER_PATH "/dev/userleds"

/* BUTTONS
 *
 * The Blue pushbutton B1, labeled "Wakeup", is connected to GPIO PC13.  A
 * high value will be sensed when the button is depressed.
 *
 * Notes:
 *    1) That the EXTI is included in the definition to enable an interrupt
 *       on this IO.
 *    2) The following definitions assume the default Solder Bridges are
 *       installed.
 */

#define GPIO_BTN_USER  (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | GPIO_PORTC | GPIO_PIN13)

/* MB1166 DSI display: reset (active low) and backlight enable */

#define GPIO_LCD_RESET (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                        GPIO_OUTPUT_CLEAR | GPIO_PORTG | GPIO_PIN3)
#define GPIO_LCD_BL    (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                        GPIO_OUTPUT_CLEAR | GPIO_PORTJ | GPIO_PIN12)

/* FT6x06 touch controller of the MB1166 display board on I2C4.  Its
 * interrupt line PK7 is active low.
 */

#define FT5X06_I2C_ADDRESS  0x38

#define GPIO_FT5X06_INT    (GPIO_INPUT | GPIO_FLOAT | GPIO_SPEED_100MHz | \
                            GPIO_PORTK | GPIO_PIN7)

/* SD/TF Card'detected pin */

#if defined(CONFIG_STM32_SDMMC1)
#  define HAVE_SDIO
#endif

#if defined(CONFIG_DISABLE_MOUNTPOINT) || !defined(CONFIG_MMCSD_SDIO)
#  undef HAVE_SDIO
#endif

#define GPIO_SDIO_NCD      (GPIO_INPUT|GPIO_FLOAT|GPIO_EXTI|GPIO_PORTI|GPIO_PIN8)

#define SDIO_SLOTNO        0
#define SDIO_MINOR         0

/****************************************************************************
 * Name: stm32_bringup
 *
 * Description:
 *   Perform architecture-specific initialization
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=y :
 *     Called from board_late_initialize().
 *
 ****************************************************************************/

int stm32_bringup(void);

/****************************************************************************
 * Name: stm32_spidev_initialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the board.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32_SPI
void stm32_spidev_initialize(void);
#endif

/****************************************************************************
 * Name: stm32_adc_setup
 *
 * Description:
 *   Initialize ADC and register the ADC driver.
 *
 ****************************************************************************/

#ifdef CONFIG_ADC
int stm32_adc_setup(void);
#endif

/****************************************************************************
 * Name: stm32_dma_alloc_init
 *
 * Description:
 *   Called to create a FAT DMA allocator
 *
 * Returned Value:
 *   0 on success or -ENOMEM
 *
 ****************************************************************************/

#if defined (CONFIG_FAT_DMAMEMORY)
int stm32_dma_alloc_init(void);
#endif

/****************************************************************************
 * Name: stm32_sdio_initialize
 *
 * Description:
 *   Initialize SDIO-based MMC/SD card support
 *
 ****************************************************************************/

#ifdef HAVE_SDIO
int stm32_sdio_initialize(void);
#endif

/****************************************************************************
 * Name: stm32_lcdpanel_probe
 *
 * Description:
 *   Read the ID of the display controller.
 *
 * Returned Value:
 *   LCD_PANEL_OTM8009A or LCD_PANEL_NT35510 on success; a negated errno
 *   value on failure.
 *
 ****************************************************************************/

#ifdef CONFIG_INPUT_FT5X06
/****************************************************************************
 * Name: stm32_tsc_setup
 *
 * Description:
 *   Register the touch controller as /dev/inputN where N is the minor
 *   device number.
 *
 ****************************************************************************/

int stm32_tsc_setup(int minor);
#endif

#ifdef CONFIG_STM32H747I_DISCO_LCD
#define LCD_PANEL_OTM8009A  1
#define LCD_PANEL_NT35510   2

struct mipi_dsi_device;
int stm32_lcdpanel_probe(FAR struct mipi_dsi_device *device);

/****************************************************************************
 * Name: stm32_lcdpanel_initialize
 *
 * Description:
 *   Send the initialization sequence of the display controller, for
 *   landscape orientation and RGB888 pixels.
 *
 ****************************************************************************/

int stm32_lcdpanel_initialize(FAR struct mipi_dsi_device *device,
                              int panel);
#endif

#endif /* __BOARDS_ARM_STM32H7_STM32H747I_DISCO_SRC_STM32H747I_DISCO_H */
