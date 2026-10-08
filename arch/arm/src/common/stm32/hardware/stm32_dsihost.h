/****************************************************************************
 * arch/arm/src/common/stm32/hardware/stm32_dsihost.h
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

#ifndef __ARCH_ARM_SRC_COMMON_STM32_HARDWARE_STM32_DSIHOST_H
#define __ARCH_ARM_SRC_COMMON_STM32_HARDWARE_STM32_DSIHOST_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"
#include "hardware/stm32_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Register Offsets *********************************************************/

#define STM32_DSI_VR_OFFSET       0x0000  /* Version register */
#define STM32_DSI_CR_OFFSET       0x0004  /* Control register */
#define STM32_DSI_CCR_OFFSET      0x0008  /* Clock control register */
#define STM32_DSI_LVCIDR_OFFSET   0x000c  /* LTDC VCID register */
#define STM32_DSI_LCOLCR_OFFSET   0x0010  /* LTDC color coding register */
#define STM32_DSI_LPCR_OFFSET     0x0014  /* LTDC polarity configuration register */
#define STM32_DSI_LPMCR_OFFSET    0x0018  /* Low-power mode configuration register */
#define STM32_DSI_PCR_OFFSET      0x002c  /* Protocol configuration register */
#define STM32_DSI_GVCIDR_OFFSET   0x0030  /* Generic VCID register */
#define STM32_DSI_MCR_OFFSET      0x0034  /* Mode configuration register */
#define STM32_DSI_VMCR_OFFSET     0x0038  /* Video mode configuration register */
#define STM32_DSI_VPCR_OFFSET     0x003c  /* Video packet configuration register */
#define STM32_DSI_VCCR_OFFSET     0x0040  /* Video chunks configuration register */
#define STM32_DSI_VNPCR_OFFSET    0x0044  /* Video null packet configuration register */
#define STM32_DSI_VHSACR_OFFSET   0x0048  /* Video HSA configuration register */
#define STM32_DSI_VHBPCR_OFFSET   0x004c  /* Video HBP configuration register */
#define STM32_DSI_VLCR_OFFSET     0x0050  /* Video line configuration register */
#define STM32_DSI_VVSACR_OFFSET   0x0054  /* Video VSA configuration register */
#define STM32_DSI_VVBPCR_OFFSET   0x0058  /* Video VBP configuration register */
#define STM32_DSI_VVFPCR_OFFSET   0x005c  /* Video VFP configuration register */
#define STM32_DSI_VVACR_OFFSET    0x0060  /* Video VA configuration register */
#define STM32_DSI_LCCR_OFFSET     0x0064  /* LTDC command configuration register */
#define STM32_DSI_CMCR_OFFSET     0x0068  /* Command mode configuration register */
#define STM32_DSI_GHCR_OFFSET     0x006c  /* Generic header configuration register */
#define STM32_DSI_GPDR_OFFSET     0x0070  /* Generic payload data register */
#define STM32_DSI_GPSR_OFFSET     0x0074  /* Generic packet status register */

/* Timeout counter configuration registers 0-5 */

#define STM32_DSI_TCCR_OFFSET(n)  (0x0078 + ((n) << 2))

#define STM32_DSI_CLCR_OFFSET     0x0094  /* Clock lane configuration register */
#define STM32_DSI_CLTCR_OFFSET    0x0098  /* Clock lane timer configuration register */
#define STM32_DSI_DLTCR_OFFSET    0x009c  /* Data lane timer configuration register */
#define STM32_DSI_PCTLR_OFFSET    0x00a0  /* PHY control register */
#define STM32_DSI_PCONFR_OFFSET   0x00a4  /* PHY configuration register */
#define STM32_DSI_PUCR_OFFSET     0x00a8  /* PHY ULPS control register */
#define STM32_DSI_PTTCR_OFFSET    0x00ac  /* PHY TX triggers configuration register */
#define STM32_DSI_PSR_OFFSET      0x00b0  /* PHY status register */
#define STM32_DSI_ISR0_OFFSET     0x00bc  /* Interrupt and status register 0 */
#define STM32_DSI_ISR1_OFFSET     0x00c0  /* Interrupt and status register 1 */
#define STM32_DSI_IER0_OFFSET     0x00c4  /* Interrupt enable register 0 */
#define STM32_DSI_IER1_OFFSET     0x00c8  /* Interrupt enable register 1 */
#define STM32_DSI_WCFGR_OFFSET    0x0400  /* Wrapper configuration register */
#define STM32_DSI_WCR_OFFSET      0x0404  /* Wrapper control register */
#define STM32_DSI_WIER_OFFSET     0x0408  /* Wrapper interrupt enable register */
#define STM32_DSI_WISR_OFFSET     0x040c  /* Wrapper interrupt and status register */
#define STM32_DSI_WIFCR_OFFSET    0x0410  /* Wrapper interrupt flag clear register */
#define STM32_DSI_WPCR0_OFFSET    0x0418  /* Wrapper PHY configuration register 0 */
#define STM32_DSI_WRPCR_OFFSET    0x0430  /* Wrapper regulator and PLL control register */

/* Register Addresses *******************************************************/

#define STM32_DSI_VR              (STM32_DSIHOST_BASE + STM32_DSI_VR_OFFSET)
#define STM32_DSI_CR              (STM32_DSIHOST_BASE + STM32_DSI_CR_OFFSET)
#define STM32_DSI_CCR             (STM32_DSIHOST_BASE + STM32_DSI_CCR_OFFSET)
#define STM32_DSI_LVCIDR          (STM32_DSIHOST_BASE + STM32_DSI_LVCIDR_OFFSET)
#define STM32_DSI_LCOLCR          (STM32_DSIHOST_BASE + STM32_DSI_LCOLCR_OFFSET)
#define STM32_DSI_LPCR            (STM32_DSIHOST_BASE + STM32_DSI_LPCR_OFFSET)
#define STM32_DSI_LPMCR           (STM32_DSIHOST_BASE + STM32_DSI_LPMCR_OFFSET)
#define STM32_DSI_PCR             (STM32_DSIHOST_BASE + STM32_DSI_PCR_OFFSET)
#define STM32_DSI_GVCIDR          (STM32_DSIHOST_BASE + STM32_DSI_GVCIDR_OFFSET)
#define STM32_DSI_MCR             (STM32_DSIHOST_BASE + STM32_DSI_MCR_OFFSET)
#define STM32_DSI_VMCR            (STM32_DSIHOST_BASE + STM32_DSI_VMCR_OFFSET)
#define STM32_DSI_VPCR            (STM32_DSIHOST_BASE + STM32_DSI_VPCR_OFFSET)
#define STM32_DSI_VCCR            (STM32_DSIHOST_BASE + STM32_DSI_VCCR_OFFSET)
#define STM32_DSI_VNPCR           (STM32_DSIHOST_BASE + STM32_DSI_VNPCR_OFFSET)
#define STM32_DSI_VHSACR          (STM32_DSIHOST_BASE + STM32_DSI_VHSACR_OFFSET)
#define STM32_DSI_VHBPCR          (STM32_DSIHOST_BASE + STM32_DSI_VHBPCR_OFFSET)
#define STM32_DSI_VLCR            (STM32_DSIHOST_BASE + STM32_DSI_VLCR_OFFSET)
#define STM32_DSI_VVSACR          (STM32_DSIHOST_BASE + STM32_DSI_VVSACR_OFFSET)
#define STM32_DSI_VVBPCR          (STM32_DSIHOST_BASE + STM32_DSI_VVBPCR_OFFSET)
#define STM32_DSI_VVFPCR          (STM32_DSIHOST_BASE + STM32_DSI_VVFPCR_OFFSET)
#define STM32_DSI_VVACR           (STM32_DSIHOST_BASE + STM32_DSI_VVACR_OFFSET)
#define STM32_DSI_LCCR            (STM32_DSIHOST_BASE + STM32_DSI_LCCR_OFFSET)
#define STM32_DSI_CMCR            (STM32_DSIHOST_BASE + STM32_DSI_CMCR_OFFSET)
#define STM32_DSI_GHCR            (STM32_DSIHOST_BASE + STM32_DSI_GHCR_OFFSET)
#define STM32_DSI_GPDR            (STM32_DSIHOST_BASE + STM32_DSI_GPDR_OFFSET)
#define STM32_DSI_GPSR            (STM32_DSIHOST_BASE + STM32_DSI_GPSR_OFFSET)
#define STM32_DSI_TCCR(n)         (STM32_DSIHOST_BASE + STM32_DSI_TCCR_OFFSET(n))
#define STM32_DSI_CLCR            (STM32_DSIHOST_BASE + STM32_DSI_CLCR_OFFSET)
#define STM32_DSI_CLTCR           (STM32_DSIHOST_BASE + STM32_DSI_CLTCR_OFFSET)
#define STM32_DSI_DLTCR           (STM32_DSIHOST_BASE + STM32_DSI_DLTCR_OFFSET)
#define STM32_DSI_PCTLR           (STM32_DSIHOST_BASE + STM32_DSI_PCTLR_OFFSET)
#define STM32_DSI_PCONFR          (STM32_DSIHOST_BASE + STM32_DSI_PCONFR_OFFSET)
#define STM32_DSI_PUCR            (STM32_DSIHOST_BASE + STM32_DSI_PUCR_OFFSET)
#define STM32_DSI_PTTCR           (STM32_DSIHOST_BASE + STM32_DSI_PTTCR_OFFSET)
#define STM32_DSI_PSR             (STM32_DSIHOST_BASE + STM32_DSI_PSR_OFFSET)
#define STM32_DSI_ISR0            (STM32_DSIHOST_BASE + STM32_DSI_ISR0_OFFSET)
#define STM32_DSI_ISR1            (STM32_DSIHOST_BASE + STM32_DSI_ISR1_OFFSET)
#define STM32_DSI_IER0            (STM32_DSIHOST_BASE + STM32_DSI_IER0_OFFSET)
#define STM32_DSI_IER1            (STM32_DSIHOST_BASE + STM32_DSI_IER1_OFFSET)
#define STM32_DSI_WCFGR           (STM32_DSIHOST_BASE + STM32_DSI_WCFGR_OFFSET)
#define STM32_DSI_WCR             (STM32_DSIHOST_BASE + STM32_DSI_WCR_OFFSET)
#define STM32_DSI_WIER            (STM32_DSIHOST_BASE + STM32_DSI_WIER_OFFSET)
#define STM32_DSI_WISR            (STM32_DSIHOST_BASE + STM32_DSI_WISR_OFFSET)
#define STM32_DSI_WIFCR           (STM32_DSIHOST_BASE + STM32_DSI_WIFCR_OFFSET)
#define STM32_DSI_WPCR0           (STM32_DSIHOST_BASE + STM32_DSI_WPCR0_OFFSET)
#define STM32_DSI_WRPCR           (STM32_DSIHOST_BASE + STM32_DSI_WRPCR_OFFSET)

/* Register Bitfield Definitions ********************************************/

/* Control register */

#define DSI_CR_EN                 (1 << 0)  /* Bit 0: DSI Host enable */

/* Clock control register */

#define DSI_CCR_TXECKDIV_SHIFT    (0)       /* Bits 0-7: TX escape clock division */
#define DSI_CCR_TXECKDIV_MASK     (0xff << DSI_CCR_TXECKDIV_SHIFT)
#  define DSI_CCR_TXECKDIV(n)     ((uint32_t)(n) << DSI_CCR_TXECKDIV_SHIFT)
#define DSI_CCR_TOCKDIV_SHIFT     (8)       /* Bits 8-15: Timeout clock division */
#define DSI_CCR_TOCKDIV_MASK      (0xff << DSI_CCR_TOCKDIV_SHIFT)
#  define DSI_CCR_TOCKDIV(n)      ((uint32_t)(n) << DSI_CCR_TOCKDIV_SHIFT)

/* LTDC color coding register */

#define DSI_LCOLCR_COLC_SHIFT     (0)       /* Bits 0-3: Color coding */
#define DSI_LCOLCR_COLC_MASK      (0xf << DSI_LCOLCR_COLC_SHIFT)
#  define DSI_LCOLCR_COLC_RGB565  (0 << DSI_LCOLCR_COLC_SHIFT)
#  define DSI_LCOLCR_COLC_RGB666  (3 << DSI_LCOLCR_COLC_SHIFT)
#  define DSI_LCOLCR_COLC_RGB888  (5 << DSI_LCOLCR_COLC_SHIFT)
#define DSI_LCOLCR_LPE            (1 << 8)  /* Bit 8: Loosely packed (RGB666) */

/* LTDC polarity configuration register */

#define DSI_LPCR_DEP              (1 << 0)  /* Bit 0: Data enable active low */
#define DSI_LPCR_VSP              (1 << 1)  /* Bit 1: VSYNC active low */
#define DSI_LPCR_HSP              (1 << 2)  /* Bit 2: HSYNC active low */

/* Protocol configuration register */

#define DSI_PCR_ETTXE             (1 << 0)  /* Bit 0: EoTp transmission enable */
#define DSI_PCR_ETRXE             (1 << 1)  /* Bit 1: EoTp reception enable */
#define DSI_PCR_BTAE              (1 << 2)  /* Bit 2: Bus turn around enable */
#define DSI_PCR_ECCRXE            (1 << 3)  /* Bit 3: ECC reception enable */
#define DSI_PCR_CRCRXE            (1 << 4)  /* Bit 4: CRC reception enable */

/* Mode configuration register */

#define DSI_MCR_CMDM              (1 << 0)  /* Bit 0: Command mode */

/* Low-power mode configuration register */

#define DSI_LPMCR_VLPSIZE_SHIFT   (0)       /* Bits 0-7: VACT largest packet size */
#define DSI_LPMCR_VLPSIZE_MASK    (0xff << DSI_LPMCR_VLPSIZE_SHIFT)
#  define DSI_LPMCR_VLPSIZE(n)    ((uint32_t)(n) << DSI_LPMCR_VLPSIZE_SHIFT)
#define DSI_LPMCR_LPSIZE_SHIFT    (16)      /* Bits 16-23: Largest packet size */
#define DSI_LPMCR_LPSIZE_MASK     (0xff << DSI_LPMCR_LPSIZE_SHIFT)
#  define DSI_LPMCR_LPSIZE(n)     ((uint32_t)(n) << DSI_LPMCR_LPSIZE_SHIFT)

/* Video mode configuration register */

#define DSI_VMCR_VMT_SHIFT        (0)       /* Bits 0-1: Video mode type */
#define DSI_VMCR_VMT_MASK         (3 << DSI_VMCR_VMT_SHIFT)
#  define DSI_VMCR_VMT_PULSES     (0 << DSI_VMCR_VMT_SHIFT) /* Non-burst, sync pulses */
#  define DSI_VMCR_VMT_EVENTS     (1 << DSI_VMCR_VMT_SHIFT) /* Non-burst, sync events */
#  define DSI_VMCR_VMT_BURST      (2 << DSI_VMCR_VMT_SHIFT) /* Burst */

#define DSI_VMCR_LPVSAE           (1 << 8)  /* Bit 8: LP in vertical sync active */
#define DSI_VMCR_LPVBPE           (1 << 9)  /* Bit 9: LP in vertical back porch */
#define DSI_VMCR_LPVFPE           (1 << 10) /* Bit 10: LP in vertical front porch */
#define DSI_VMCR_LPVAE            (1 << 11) /* Bit 11: LP in vertical active */
#define DSI_VMCR_LPHBPE           (1 << 12) /* Bit 12: LP in horizontal back porch */
#define DSI_VMCR_LPHFPE           (1 << 13) /* Bit 13: LP in horizontal front porch */
#define DSI_VMCR_FBTAAE           (1 << 14) /* Bit 14: Frame BTA acknowledge */
#define DSI_VMCR_LPCE             (1 << 15) /* Bit 15: LP commands in video mode */
#define DSI_VMCR_PGE              (1 << 16) /* Bit 16: Pattern generator enable */

/* Video timing registers.  Horizontal values are in lane byte clock
 * cycles, vertical values in lines.
 */

#define DSI_VPCR_VPSIZE_MASK      0x3fff    /* Video packet size in pixels */
#define DSI_VCCR_NUMC_MASK        0x1fff    /* Number of chunks */
#define DSI_VNPCR_NPSIZE_MASK     0x1fff    /* Null packet size */
#define DSI_VHSACR_HSA_MASK       0x0fff    /* Horizontal sync active */
#define DSI_VHBPCR_HBP_MASK       0x0fff    /* Horizontal back porch */
#define DSI_VLCR_HLINE_MASK       0x7fff    /* Horizontal line */
#define DSI_VVSACR_VSA_MASK       0x03ff    /* Vertical sync active */
#define DSI_VVBPCR_VBP_MASK       0x03ff    /* Vertical back porch */
#define DSI_VVFPCR_VFP_MASK       0x03ff    /* Vertical front porch */
#define DSI_VVACR_VA_MASK         0x3fff    /* Vertical active */

/* LTDC command configuration register */

#define DSI_LCCR_CMDSIZE_SHIFT    (0)       /* Bits 0-15: Command size */
#define DSI_LCCR_CMDSIZE_MASK     (0xffff << DSI_LCCR_CMDSIZE_SHIFT)
#  define DSI_LCCR_CMDSIZE(n)     ((uint32_t)(n) << DSI_LCCR_CMDSIZE_SHIFT)

/* Command mode configuration register */

#define DSI_CMCR_TEARE            (1 << 0)  /* Bit 0: Tearing effect acknowledge request enable */
#define DSI_CMCR_ARE              (1 << 1)  /* Bit 1: Acknowledge request enable */
#define DSI_CMCR_GSW0TX           (1 << 8)  /* Bit 8: Generic short write zero parameters in LP */
#define DSI_CMCR_GSW1TX           (1 << 9)  /* Bit 9: Generic short write one parameter in LP */
#define DSI_CMCR_GSW2TX           (1 << 10) /* Bit 10: Generic short write two parameters in LP */
#define DSI_CMCR_GSR0TX           (1 << 11) /* Bit 11: Generic short read zero parameters in LP */
#define DSI_CMCR_GSR1TX           (1 << 12) /* Bit 12: Generic short read one parameter in LP */
#define DSI_CMCR_GSR2TX           (1 << 13) /* Bit 13: Generic short read two parameters in LP */
#define DSI_CMCR_GLWTX            (1 << 14) /* Bit 14: Generic long write in LP */
#define DSI_CMCR_DSW0TX           (1 << 16) /* Bit 16: DCS short write zero parameters in LP */
#define DSI_CMCR_DSW1TX           (1 << 17) /* Bit 17: DCS short write one parameter in LP */
#define DSI_CMCR_DSR0TX           (1 << 18) /* Bit 18: DCS short read zero parameters in LP */
#define DSI_CMCR_DLWTX            (1 << 19) /* Bit 19: DCS long write in LP */
#define DSI_CMCR_MRDPS            (1 << 24) /* Bit 24: Maximum read packet size in LP */

/* Generic header configuration register */

#define DSI_GHCR_DT_SHIFT         (0)       /* Bits 0-5: Data type */
#define DSI_GHCR_DT_MASK          (0x3f << DSI_GHCR_DT_SHIFT)
#  define DSI_GHCR_DT(n)          ((uint32_t)(n) << DSI_GHCR_DT_SHIFT)
#define DSI_GHCR_VCID_SHIFT       (6)       /* Bits 6-7: Virtual channel ID */
#define DSI_GHCR_VCID_MASK        (3 << DSI_GHCR_VCID_SHIFT)
#  define DSI_GHCR_VCID(n)        ((uint32_t)(n) << DSI_GHCR_VCID_SHIFT)
#define DSI_GHCR_WCLSB_SHIFT      (8)       /* Bits 8-15: Word count LSB */
#define DSI_GHCR_WCLSB_MASK       (0xff << DSI_GHCR_WCLSB_SHIFT)
#  define DSI_GHCR_WCLSB(n)       ((uint32_t)(n) << DSI_GHCR_WCLSB_SHIFT)
#define DSI_GHCR_WCMSB_SHIFT      (16)      /* Bits 16-23: Word count MSB */
#define DSI_GHCR_WCMSB_MASK       (0xff << DSI_GHCR_WCMSB_SHIFT)
#  define DSI_GHCR_WCMSB(n)       ((uint32_t)(n) << DSI_GHCR_WCMSB_SHIFT)

/* Generic packet status register */

#define DSI_GPSR_CMDFE            (1 << 0)  /* Bit 0: Command FIFO empty */
#define DSI_GPSR_CMDFF            (1 << 1)  /* Bit 1: Command FIFO full */
#define DSI_GPSR_PWRFE            (1 << 2)  /* Bit 2: Payload write FIFO empty */
#define DSI_GPSR_PWRFF            (1 << 3)  /* Bit 3: Payload write FIFO full */
#define DSI_GPSR_PRDFE            (1 << 4)  /* Bit 4: Payload read FIFO empty */
#define DSI_GPSR_PRDFF            (1 << 5)  /* Bit 5: Payload read FIFO full */
#define DSI_GPSR_RCB              (1 << 6)  /* Bit 6: Read command busy */

/* Clock lane configuration register */

#define DSI_CLCR_DPCC             (1 << 0)  /* Bit 0: D-PHY clock control */
#define DSI_CLCR_ACR              (1 << 1)  /* Bit 1: Automatic clock lane control */

/* Clock lane timer configuration register */

#define DSI_CLTCR_LP2HS_SHIFT     (0)       /* Bits 0-9: Low-power to high-speed time */
#define DSI_CLTCR_LP2HS_MASK      (0x3ff << DSI_CLTCR_LP2HS_SHIFT)
#  define DSI_CLTCR_LP2HS(n)      ((uint32_t)(n) << DSI_CLTCR_LP2HS_SHIFT)
#define DSI_CLTCR_HS2LP_SHIFT     (16)      /* Bits 16-25: High-speed to low-power time */
#define DSI_CLTCR_HS2LP_MASK      (0x3ff << DSI_CLTCR_HS2LP_SHIFT)
#  define DSI_CLTCR_HS2LP(n)      ((uint32_t)(n) << DSI_CLTCR_HS2LP_SHIFT)

/* Data lane timer configuration register */

#define DSI_DLTCR_MRDTIME_SHIFT   (0)       /* Bits 0-14: Maximum read time */
#define DSI_DLTCR_MRDTIME_MASK    (0x7fff << DSI_DLTCR_MRDTIME_SHIFT)
#  define DSI_DLTCR_MRDTIME(n)    ((uint32_t)(n) << DSI_DLTCR_MRDTIME_SHIFT)
#define DSI_DLTCR_LP2HS_SHIFT     (16)      /* Bits 16-23: Low-power to high-speed time */
#define DSI_DLTCR_LP2HS_MASK      (0xff << DSI_DLTCR_LP2HS_SHIFT)
#  define DSI_DLTCR_LP2HS(n)      ((uint32_t)(n) << DSI_DLTCR_LP2HS_SHIFT)
#define DSI_DLTCR_HS2LP_SHIFT     (24)      /* Bits 24-31: High-speed to low-power time */
#define DSI_DLTCR_HS2LP_MASK      (0xff << DSI_DLTCR_HS2LP_SHIFT)
#  define DSI_DLTCR_HS2LP(n)      ((uint32_t)(n) << DSI_DLTCR_HS2LP_SHIFT)

/* PHY control register */

#define DSI_PCTLR_DEN             (1 << 1)  /* Bit 1: Digital enable */
#define DSI_PCTLR_CKE             (1 << 2)  /* Bit 2: Clock enable */

/* PHY configuration register */

#define DSI_PCONFR_NL_SHIFT       (0)       /* Bits 0-1: Number of data lanes */
#define DSI_PCONFR_NL_MASK        (3 << DSI_PCONFR_NL_SHIFT)
#  define DSI_PCONFR_NL_ONE       (0 << DSI_PCONFR_NL_SHIFT)
#  define DSI_PCONFR_NL_TWO       (1 << DSI_PCONFR_NL_SHIFT)
#define DSI_PCONFR_SWTIME_SHIFT   (8)       /* Bits 8-15: Stop wait time */
#define DSI_PCONFR_SWTIME_MASK    (0xff << DSI_PCONFR_SWTIME_SHIFT)
#  define DSI_PCONFR_SWTIME(n)    ((uint32_t)(n) << DSI_PCONFR_SWTIME_SHIFT)

/* PHY status register */

#define DSI_PSR_PD                (1 << 1)  /* Bit 1: PHY direction */
#define DSI_PSR_PSSC              (1 << 2)  /* Bit 2: PHY stop state clock lane */
#define DSI_PSR_UANC              (1 << 3)  /* Bit 3: ULPS active not clock lane */
#define DSI_PSR_PSS0              (1 << 4)  /* Bit 4: PHY stop state lane 0 */
#define DSI_PSR_UAN0              (1 << 5)  /* Bit 5: ULPS active not lane 0 */
#define DSI_PSR_RUE0              (1 << 6)  /* Bit 6: RX ULPS escape lane 0 */
#define DSI_PSR_PSS1              (1 << 7)  /* Bit 7: PHY stop state lane 1 */
#define DSI_PSR_UAN1              (1 << 8)  /* Bit 8: ULPS active not lane 1 */

/* Interrupt and status register 1 */

#define DSI_ISR1_PSE              (1 << 5)  /* Bit 5: Packet size error */
#define DSI_ISR1_GPRDE            (1 << 11) /* Bit 11: Generic payload read error */

/* Wrapper configuration register */

#define DSI_WCFGR_DSIM            (1 << 0)  /* Bit 0: DSI mode (adapted command mode) */
#define DSI_WCFGR_COLMUX_SHIFT    (1)       /* Bits 1-3: Color multiplexing */
#define DSI_WCFGR_COLMUX_MASK     (7 << DSI_WCFGR_COLMUX_SHIFT)
#  define DSI_WCFGR_COLMUX_RGB565 (0 << DSI_WCFGR_COLMUX_SHIFT)
#  define DSI_WCFGR_COLMUX_RGB666 (3 << DSI_WCFGR_COLMUX_SHIFT)
#  define DSI_WCFGR_COLMUX_RGB888 (5 << DSI_WCFGR_COLMUX_SHIFT)
#define DSI_WCFGR_TESRC           (1 << 4)  /* Bit 4: TE source (external pin) */
#define DSI_WCFGR_TEPOL           (1 << 5)  /* Bit 5: TE polarity (falling edge) */
#define DSI_WCFGR_AR              (1 << 6)  /* Bit 6: Automatic refresh */
#define DSI_WCFGR_VSPOL           (1 << 7)  /* Bit 7: VSync polarity (rising edge) */

/* Wrapper control register */

#define DSI_WCR_COLM              (1 << 0)  /* Bit 0: Color mode (reduced) */
#define DSI_WCR_SHTDN             (1 << 1)  /* Bit 1: Shutdown */
#define DSI_WCR_LTDCEN            (1 << 2)  /* Bit 2: LTDC enable (start a refresh) */
#define DSI_WCR_DSIEN             (1 << 3)  /* Bit 3: DSI wrapper enable */

/* Wrapper interrupt and status register */

#define DSI_WISR_TEIF             (1 << 0)  /* Bit 0: Tearing effect */
#define DSI_WISR_ERIF             (1 << 1)  /* Bit 1: End of refresh */
#define DSI_WISR_BUSY             (1 << 2)  /* Bit 2: Busy (refresh ongoing) */
#define DSI_WISR_PLLLS            (1 << 8)  /* Bit 8: PLL lock status */
#define DSI_WISR_RRS              (1 << 12) /* Bit 12: Regulator ready status */

/* Wrapper interrupt flag clear register */

#define DSI_WIFCR_CTEIF           (1 << 0)  /* Bit 0: Clear tearing effect flag */
#define DSI_WIFCR_CERIF           (1 << 1)  /* Bit 1: Clear end of refresh flag */

/* Wrapper PHY configuration register 0 */

#define DSI_WPCR0_UIX4_SHIFT      (0)       /* Bits 0-5: Unit interval multiplied by 4 */
#define DSI_WPCR0_UIX4_MASK       (0x3f << DSI_WPCR0_UIX4_SHIFT)
#  define DSI_WPCR0_UIX4(n)       ((uint32_t)(n) << DSI_WPCR0_UIX4_SHIFT)

/* Wrapper regulator and PLL control register */

#define DSI_WRPCR_PLLEN           (1 << 0)  /* Bit 0: PLL enable */
#define DSI_WRPCR_NDIV_SHIFT      (2)       /* Bits 2-8: PLL loop division factor */
#define DSI_WRPCR_NDIV_MASK       (0x7f << DSI_WRPCR_NDIV_SHIFT)
#  define DSI_WRPCR_NDIV(n)       ((uint32_t)(n) << DSI_WRPCR_NDIV_SHIFT)
#define DSI_WRPCR_IDF_SHIFT       (11)      /* Bits 11-14: PLL input division factor */
#define DSI_WRPCR_IDF_MASK        (0xf << DSI_WRPCR_IDF_SHIFT)
#  define DSI_WRPCR_IDF(n)        ((uint32_t)(n) << DSI_WRPCR_IDF_SHIFT)
#define DSI_WRPCR_ODF_SHIFT       (16)      /* Bits 16-17: PLL output division factor */
#define DSI_WRPCR_ODF_MASK        (3 << DSI_WRPCR_ODF_SHIFT)
#  define DSI_WRPCR_ODF_1         (0 << DSI_WRPCR_ODF_SHIFT)
#  define DSI_WRPCR_ODF_2         (1 << DSI_WRPCR_ODF_SHIFT)
#  define DSI_WRPCR_ODF_4         (2 << DSI_WRPCR_ODF_SHIFT)
#  define DSI_WRPCR_ODF_8         (3 << DSI_WRPCR_ODF_SHIFT)
#define DSI_WRPCR_REGEN           (1 << 24) /* Bit 24: Regulator enable */

#endif /* __ARCH_ARM_SRC_COMMON_STM32_HARDWARE_STM32_DSIHOST_H */
