/***********************************************************************
 *
 * File: stmfb/Linux/stm/coredisplay/sti7200c1.c
 * Copyright (c) 2007,2008 STMicroelectronics Limited.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 *
\***********************************************************************/

/*
 * Note: the STi7200 is only supported on 2.6.23 and above kernels.
 */
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/stm/sysconf.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/processor.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
#include <asm/semaphore.h>
#else
#include <linux/semaphore.h>
#endif

#include <linux/stm/pio.h>
#include <asm/irq-ilc.h>

#include <stmdisplay.h>
#include <linux/stm/stmcoredisplay.h>

#include "Gamma/sti7200/sti7200reg.h"
#include "Gamma/sti7200/sti7200device.h"

#if defined(CONFIG_SH_ST_MB519)
#define HAS_DSUB 1
#else
#define HAS_DSUB 0
#endif

static const unsigned long local_whitelist[] = {
    STi7200_REGISTER_BASE + STi7200C1_LOCAL_DISPLAY_BASE,
    STi7200_REGISTER_BASE + STi7200C1_LOCAL_DISPLAY_BASE+PAGE_SIZE,
    STi7200_REGISTER_BASE + STi7200C1_LOCAL_DISPLAY_BASE+(PAGE_SIZE*2),
    STi7200_REGISTER_BASE + STi7200C1_HDMI_BASE,
};

static const unsigned long remote_whitelist[] = {
    STi7200_REGISTER_BASE + STi7200C1_REMOTE_DENC_BASE
};


static struct stmcore_display_pipeline_data platform_data[] = {
  {
    .owner                    = THIS_MODULE,
    .name                     = "STi7200c1-main",
    .device                   = 0,
    .vtg_irq                  = ILC_IRQ(68),
    .blitter_irq              = ILC_IRQ(50),
    .hdmi_irq                 = ILC_IRQ(60),
#if defined(CONFIG_SH_ST_CB101)
    .hdmi_i2c_adapter_id      = 1,
#else
    .hdmi_i2c_adapter_id      = 0,
#endif
    .main_output_id           = STi7200_OUTPUT_IDX_VDP0_MAIN,
    .aux_output_id            = -1,
    .hdmi_output_id           = STi7200_OUTPUT_IDX_VDP0_HDMI,
    .dvo_output_id            = STi7200_OUTPUT_IDX_VDP0_DVO0,

    .blitter_id               = STi7200c1_BLITTER_IDX,
    .blitter_type             = STMCORE_BLITTER_BDISPII,

    .preferred_graphics_plane = OUTPUT_GDP1,
    .preferred_video_plane    = OUTPUT_VID1,
    .planes                   = {
    	{ OUTPUT_GDP1, STMCORE_PLANE_GFX   | STMCORE_PLANE_PREFERRED | STMCORE_PLANE_MEM_ANY},
    	{ OUTPUT_GDP2, STMCORE_PLANE_GFX   | STMCORE_PLANE_MEM_ANY},
    	{ OUTPUT_GDP3, STMCORE_PLANE_GFX   | STMCORE_PLANE_SHARED    | STMCORE_PLANE_MEM_ANY},
    	{ OUTPUT_VID1, STMCORE_PLANE_VIDEO | STMCORE_PLANE_PREFERRED | STMCORE_PLANE_MEM_ANY},
    	{ OUTPUT_VID2, STMCORE_PLANE_VIDEO | STMCORE_PLANE_SHARED    | STMCORE_PLANE_MEM_ANY},
    	{ OUTPUT_CUR , STMCORE_PLANE_GFX   | STMCORE_PLANE_MEM_ANY}
    },
    .whitelist                = local_whitelist,
    .whitelist_size           = ARRAY_SIZE(local_whitelist),
    .io_offset                = 0
  },
  {
    .owner                    = THIS_MODULE,
    .name                     = "STi7200c1-aux",
    .device                   = 0,
    .vtg_irq                  = ILC_IRQ(70),
    .blitter_irq              = -1,
    .hdmi_irq                 = -1,
    .hdmi_i2c_adapter_id      = -1,
    .main_output_id           = STi7200_OUTPUT_IDX_VDP1_MAIN,
    .aux_output_id            = -1,
    .hdmi_output_id           = -1,
    .dvo_output_id            = -1,

    .blitter_id               = STi7200c1_BLITTER_IDX,
    .blitter_type             = STMCORE_BLITTER_BDISPII,

    .preferred_graphics_plane = OUTPUT_GDP4,
    .preferred_video_plane    = OUTPUT_VID2,
    .planes                   = {
    	{ OUTPUT_GDP3, STMCORE_PLANE_GFX   | STMCORE_PLANE_SHARED    | STMCORE_PLANE_MEM_ANY},
    	{ OUTPUT_GDP4, STMCORE_PLANE_GFX   | STMCORE_PLANE_PREFERRED | STMCORE_PLANE_MEM_ANY},
    	{ OUTPUT_VID2, STMCORE_PLANE_VIDEO | STMCORE_PLANE_SHARED    | STMCORE_PLANE_PREFERRED | STMCORE_PLANE_MEM_ANY}
    },
    .whitelist                = local_whitelist,
    .whitelist_size           = ARRAY_SIZE(local_whitelist),
    .io_offset                = 0
  },
  {
    .owner                    = THIS_MODULE,
    .name                     = "STi7200c1-remote",
    .device                   = (void*)1,
    .vtg_irq                  = ILC_IRQ(72),
    .blitter_irq              = -1,
    .hdmi_irq                 = -1,
    .hdmi_i2c_adapter_id      = -1,
    .main_output_id           = STi7200_OUTPUT_IDX_VDP2_MAIN,
    .aux_output_id            = -1,
    .hdmi_output_id           = -1,
    .dvo_output_id            = -1,

    .blitter_id               = STi7200c1_BLITTER_IDX,
    .blitter_type             = STMCORE_BLITTER_BDISPII,

    .preferred_graphics_plane = OUTPUT_GDP1,
    .preferred_video_plane    = OUTPUT_VID1,
    .planes                   = {
    	{ OUTPUT_GDP1, STMCORE_PLANE_GFX   | STMCORE_PLANE_PREFERRED | STMCORE_PLANE_MEM_ANY},
    	{ OUTPUT_VID1, STMCORE_PLANE_VIDEO | STMCORE_PLANE_PREFERRED | STMCORE_PLANE_MEM_ANY}
    },
    .whitelist                = remote_whitelist,
    .whitelist_size           = ARRAY_SIZE(remote_whitelist),
    .io_offset                = 0
  }
};

/*
 * For ST boards only. Customers will need to tune this for their board designs.
 */
#if defined(THIS_IS_WHAT_THE_REFERENCE_DESIGN_SHOULD_PRODUCE)
static const int maxDAC123Voltage = 1400;   // Rref=7.81Kohm Rload=140ohm, Vmax=(78.093/7.810)*140
static const int maxDAC456Voltage = 1400;
#else /* THIS IS WHAT THE MB519 ACTUALLY PRODUCES */
static const int maxDAC123Voltage = 1350;
static const int maxDAC456Voltage = 1350;
#endif


#if defined(CONFIG_SH_ST_MB519)
/*
 * This error has been observed on two MB519 boards, one RevC and one RevD, so
 * it seems systematic. The error is enough to disturb colour burst and sync in
 * HD modes (particularly 1080i/60Hz) on some very sensitive TVs.
 */
static const int refClockError = 102; /* ppm */
#else
static const int refClockError = 0;
#endif


static const int DAC123SaturationPoint; // Use Default (1023 for a 10bit DAC)
static const int DAC456SaturationPoint;

static struct stpio_pin *hotplug_pio;
static struct stpio_pin *vsync_pio;
static struct stpio_pin *hsync_pio;
static struct sysconf_field *syscfg7_24;

int __init stmcore_probe_device(struct stmcore_display_pipeline_data **pd, int *nr_platform_devices)
{
  /*
   * As we may have Cut2 chips in Cut1 packages we cannot simply rely on the
   * board being built for, we need to test the chip version.
   */
  if(boot_cpu_data.type == CPU_STX7200 && boot_cpu_data.cut_major == 1)
  {
    *pd = platform_data;
    *nr_platform_devices = N_ELEMENTS (platform_data);

    /*
     * Setup HDMI hotplug
     */
    hotplug_pio = stpio_request_pin(5,7,"HDMI Hotplug",STPIO_IN);

    if(HAS_DSUB != 0)
    {
      /*
       * Setup DSub connector H & V sync outputs on MB519/MB520.
       * NOTE: These signals use pio ports that on production boards are almost
       *       certainly dedicated to something else. So don't just copy this
       *       blindly for your own board.
       *
       *
       *       On MB520 J35 & J36 must be set to the 1-2 postion to get the
       *       signals routed to the DSub connector.
       */
      vsync_pio = stpio_request_pin(3,7,"DSub VSync",STPIO_ALT_OUT);
      hsync_pio = stpio_request_pin(4,0,"DSub HSync",STPIO_ALT_OUT);

      if(vsync_pio && hsync_pio)
      {
        syscfg7_24 = sysconf_claim(SYS_CFG,7,24,24,"DVO Syncs");
        sysconf_write(syscfg7_24,1);
      }
      else
      {
        printk(KERN_WARNING "stmcore-display: External H&V sync signals already in use, probably as an I2C bus\n");
        printk(KERN_WARNING "stmcore-display: DSub connector output will not be functional.\n");
      }
    }

    printk(KERN_WARNING "stmcore-display: STi7200 display probed\n");
    return 0;
  }

  printk(KERN_WARNING "stmcore-display: STi7200 display not found\n");

  return -ENODEV;
}


int __init stmcore_display_postinit(struct stmcore_display *p)
{
  stm_display_output_setup_clock_reference(p->main_output, STM_CLOCK_REF_30MHZ, refClockError);

  /*
   * Setup internal configuration controls
   */
  if(maxDAC123Voltage != 0)
    stm_display_output_set_control(p->main_output, STM_CTRL_DAC123_MAX_VOLTAGE, maxDAC123Voltage);

  if(maxDAC456Voltage != 0)
    stm_display_output_set_control(p->main_output, STM_CTRL_DAC456_MAX_VOLTAGE, maxDAC456Voltage);

  if(DAC123SaturationPoint != 0)
    stm_display_output_set_control(p->main_output, STM_CTRL_DAC123_SATURATION_POINT, DAC123SaturationPoint);

  if(DAC456SaturationPoint != 0)
    stm_display_output_set_control(p->main_output, STM_CTRL_DAC456_SATURATION_POINT, DAC456SaturationPoint);

  return 0;
}


void stmcore_cleanup_device(void)
{
  if(hotplug_pio)
    stpio_free_pin(hotplug_pio);

  if(vsync_pio)
    stpio_free_pin(vsync_pio);

  if(hsync_pio)
    stpio_free_pin(hsync_pio);

  if(syscfg7_24)
    sysconf_release (syscfg7_24);
}
