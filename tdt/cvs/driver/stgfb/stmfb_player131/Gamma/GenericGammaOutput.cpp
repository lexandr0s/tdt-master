/***********************************************************************
 *
 * File: stgfb/Gamma/GenericGammaOutput.cpp
 * Copyright (c) 2000, 2004, 2005 STMicroelectronics Limited.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 *
\***********************************************************************/

#include <stmdisplay.h>

#include <Generic/IOS.h>
#include <Generic/IDebug.h>

#include <Generic/DisplayDevice.h>

#include <STMCommon/stmdenc.h>
#include <STMCommon/stmvtg.h>
#include <STMCommon/stmfsynth.h>

#include "GenericGammaOutput.h"
#include "GammaMixer.h"


CGenericGammaOutput::CGenericGammaOutput(CDisplayDevice *pDev,
                                         CSTmDENC       *pDENC,
                                         CSTmVTG        *pVTG,
                                         CGammaMixer    *pMixer,
                                         CSTmFSynth     *pFSynth) : COutput(pDev, (ULONG)pVTG)
{
  DEBUGF2(2,("%s I am @ %p\n",__PRETTY_FUNCTION__,this));

  m_pGammaReg = (ULONG*)pDev->GetCtrlRegisterBase();

  m_pDENC     = pDENC;
  m_pVTG      = pVTG;
  m_pMixer    = pMixer;
  m_pFSynth   = pFSynth;

  m_bUsingDENC  = false;

  /*
   * Analogue outputs don't have hot plug status, so we say they are always
   * connected.
   */
  m_displayStatus = STM_DISPLAY_CONNECTED;

  m_pPendingMode = 0;

  m_ulBrightness = 0x80;
  m_ulSaturation = 0x80;
  m_ulContrast   = 0x80;
  m_ulHue        = 0x80;

  /*
   * Just pick some common default numbers to avoid divide by zero exceptions
   */
  m_maxDACVoltage  = 1400; // in mV
  m_DACSaturation  = 1023;
  RecalculateDACSetup();

}


CGenericGammaOutput::~CGenericGammaOutput() {}


bool CGenericGammaOutput::Stop(void)
{
  DEBUGF2(2,("CGenericGammaOutput::Stop this = %p\n",this));

  if(m_pCurrentMode)
  {
    ULONG planes = m_pMixer->GetActivePlanes();

    if((planes & ~(ULONG)OUTPUT_BKG) != 0)
    {
      DEBUGF2(1,("CGenericGammaOutput::Stop error - output has active planes\n"));
      return false;
    }

    this->DisableDACs();

    m_pMixer->Stop();
    m_pDENC->Stop();
    m_pVTG->Stop();

    m_pPendingMode = 0;
    m_bUsingDENC = false;

    COutput::Stop();
  }

  DEBUGF2(2,("CGenericGammaOutput::Stop out\n"));
  return true;
}


bool CGenericGammaOutput::TryModeChange(const stm_mode_line_t* pModeLine, ULONG tvStandard)
{
  DEBUGF2(2,("%s (%p), current/new mode: %p/%p\n",
             __PRETTY_FUNCTION__, this, m_pCurrentMode, pModeLine));

  if(m_pCurrentMode)
  {
    if(tvStandard & STM_OUTPUT_STD_SD_MASK)
    {
      if(m_pCurrentMode == pModeLine)
      {
        if(m_ulTVStandard != tvStandard)
        {
          DEBUGF2(2,("CGenericGammaOutput::TryModeChange changing DENC standard\n"));
          m_pDENC->Start(this, pModeLine,tvStandard);
          m_ulTVStandard = tvStandard;
        }

        return true;
      }
    }
    else
    {
      /*
       * This supports changing between a select number of very closely
       * related modes, which requires only the fsynth register to be
       * changed and possibly the HDMI video info frame to be updated.
       * However getting all the hardware updated at the beginning of the
       * same vsync and glitch free is a challenge, so we have to
       * synchronize things in the VTG interrupt handler.
       */
      DEBUGF2(2,("CGenericGammaOutput::TryModeChange trying VTG update\n"));

      /*
       * Don't go any further if we already have a mode update pending,
       * the caller will have to stop the output if they really want to
       * do the change, which will cancel the outstanding update.
       */
      if(m_pPendingMode)
      {
        DEBUGF2(1,("CGenericGammaOutput::TryModeChange Failed, VTG update already pending m_pPendingMode = %p m_pCurrentMode = %p\n",m_pPendingMode,m_pCurrentMode));
        return false;
      }

      m_pPendingMode = pModeLine;
      if(m_pVTG->RequestModeUpdate(pModeLine))
        return true;

      m_pPendingMode = 0;
    }
  }

  DEBUGF2(2,("CGenericGammaOutput::TryModeChange failed, new mode is not compatible\n"));

  return false;
}


bool CGenericGammaOutput::Start(const stm_mode_line_t* pModeLine, ULONG tvStandard)
{
  DEBUGF2(2,("CGenericGammaOutput::Start this = %p\n",this));
  DEBUGF2(2,("CGenericGammaOutput::Start m_pVTG = %p m_pDENC = %p m_pMixer = %p\n",m_pVTG,m_pDENC,m_pMixer));


  // Check this object has been sanely set up
  ASSERTF((m_pVTG && m_pDENC && m_pMixer),
          ("CGenericGammaOutput::Start Error: Output not initialised correctly\n"));

  ASSERTF(pModeLine, ("CGenericGammaOutput::Start Error: NULL mode line\n"));

  if(m_bIsSuspended)
  {
    DEBUGF2(1,("CGenericGammaOutput::Start output is suspended\n"));
    return false;
  }

  /*
   * First try to change the display mode on the fly, if that works there is
   * nothing else to do.
   */
  if(TryModeChange(pModeLine, tvStandard))
  {
    DEBUGF2(2,("CGenericGammaOutput::Start - mode change successfull\n"));
    return true;
  }

  if(m_pCurrentMode)
  {
    DEBUGF2(1,("CGenericGammaOutput::Start - failed, output is active\n"));
    return false;
  }

  if(!m_pVTG->Start(pModeLine))
  {
    DEBUGF2(1,("CGenericGammaOutput::Start VTG start failed\n"));
    return false;
  }

  if(pModeLine->ModeParams.OutputStandards & (STM_OUTPUT_STD_SD_MASK | STM_OUTPUT_STD_ED_MASK))
  {
    /*
     * Only start the DENC for SD modes, which includes ED modes for
     * parts containing a PDENC (variant that supports progressive ED
     * modes)
     */
    if(!m_pDENC->Start(this, pModeLine, tvStandard))
    {
      DEBUGF2(1,("CGenericGammaOutput::Start DENC start failed\n"));
      return false;
    }

    m_bUsingDENC = true;

    /*
     * Now we have ownership of the DENC, set this output's PSI and
     * signal range control setup.
     */
    ProgramPSIControls();

    m_pDENC->SetControl(STM_CTRL_SIGNAL_RANGE, (ULONG)m_signalRange);
  }

  if(!m_pMixer->Start(pModeLine))
  {
    DEBUGF2(1,("CGenericGammaOutput::Start Mixer start failed\n"));
    return false;
  }

  COutput::Start(pModeLine, tvStandard);

  /*
   * Make sure that the current output format and DAC configuration are
   * applied, now that the mode is active, then turn the DACs on.
   */
  this->SetOutputFormat(m_ulOutputFormat);
  this->EnableDACs();

  DEBUGF2(2,("CGenericGammaOutput::Start out\n"));
  return true;
}


void CGenericGammaOutput::Suspend(void)
{
  DENTRY();

  if(m_bIsSuspended)
    return;

  this->DisableDACs();

  if(m_pVTG)
    m_pVTG->Stop();

  COutput::Suspend();

  DEXIT();
}


void CGenericGammaOutput::Resume(void)
{
  DENTRY();

  if(!m_bIsSuspended)
    return;

  if(m_pCurrentMode)
  {
    if(m_pVTG)
      m_pVTG->Start(m_pCurrentMode);

    this->EnableDACs();
  }

  COutput::Resume();

  DEXIT();
}


bool CGenericGammaOutput::HandleInterrupts(void)
{
  stm_field_t sync;
  TIME64      now;
  bool        bError = false;
  static int  errorcount = 1; /* this prevents a spurious message while loading the driver */

  if(!m_pVTG)
    return false;

  sync = m_pVTG->GetInterruptStatus();

  ASSERTF((sync != STM_UNKNOWN_FIELD),("CGenericGammaOutput::HandleVSYNCInterrupt bad interrupt status\n"));

  m_LastVTGSync = sync;

  if (sync == STM_NOT_A_FIELD)
    return true;

  now = g_pIOS->GetSystemTime();

  /*
   * First check if we have had an on the fly mode change complete on this
   * vsync. If so then reset the VSync timing information (we may have changed
   * refresh frequency).
   */
  if((m_pPendingMode != 0) && (m_pPendingMode == m_pVTG->GetCurrentMode()))
  {
    DEBUGF2(2,("CGenericGammaOutput::HandleVSYNCInterrupt changing display mode m_pPendingMode = %p\n",m_pPendingMode));
    COutput::Start(m_pPendingMode, m_ulTVStandard);
  }

  if(m_LastVSyncTime != 0)
  {
    TIME64 timediff = now - m_LastVSyncTime;

    if(timediff < 0LL)
    {
      g_pIDebug->IDebugPrintf("HandleVSYNCInterrupt: - backwards time detected, last time = %lld now = %lld\n",m_LastVSyncTime,now);
      bError = true;
    }
    else if(timediff >= (m_fieldframeDuration*2LL))
    {
      if(UNLIKELY (errorcount == 0))
      {
        g_pIDebug->IDebugPrintf("HandleVSYNCInterrupt: - time discontinuity detected, vsync interval = %lld field duration = %lld\n",timediff,m_fieldframeDuration);
        errorcount = 60;
      }
      else
      {
	errorcount--;
      }

      bError = true;
    }
  }

  m_LastVSyncTime = now;

  return true;
}


void CGenericGammaOutput::SoftReset(void)
{
  DENTRY();

  if(m_pCurrentMode != 0)
  {
    m_pVTG->ResetCounters();
  }

  DEXIT();
}


void CGenericGammaOutput::UpdateHW(void)
{
  if((m_pPendingMode != 0) && (m_pPendingMode == m_pVTG->GetCurrentMode()))
  {
    /*
     * If we have had an on the fly mode change complete on this
     * vsync, clear the pending mode as we have now finished all processing for
     * the mode chage.
     */
    m_pPendingMode = 0;
  }

  if(m_pDENC && m_bUsingDENC)
    m_pDENC->UpdateHW();
}


bool CGenericGammaOutput::CanShowPlane(stm_plane_id_t planeID)
{
  return m_pMixer->PlaneValid(planeID);
}


bool CGenericGammaOutput::ShowPlane(stm_plane_id_t planeID)
{
  DEBUGF2(2,("CGenericGammaOutput::ShowPlane %d\n",(int)planeID));
  return m_pMixer->EnablePlane(planeID);
}


void CGenericGammaOutput::HidePlane(stm_plane_id_t planeID)
{
  DEBUGF2(2,("CGenericGammaOutput::HidePlane %d\n",(int)planeID));
  m_pMixer->DisablePlane(planeID);
}


bool CGenericGammaOutput::SetPlaneDepth(stm_plane_id_t planeID, int depth, bool activate)
{
  return m_pMixer->SetPlaneDepth(planeID, depth, activate);
}


bool CGenericGammaOutput::GetPlaneDepth(stm_plane_id_t planeID, int *depth) const
{
  return m_pMixer->GetPlaneDepth(planeID, depth);
}


ULONG CGenericGammaOutput::SupportedControls(void) const
{
  ULONG ctrlCaps = STM_CTRL_CAPS_CLOCK_RECOVERY |
                   STM_CTRL_CAPS_DAC_VOLTAGE    |
                   STM_CTRL_CAPS_VIDEO_OUT_SELECT;

  ctrlCaps |= m_pMixer->SupportedControls();

  if(m_pDENC)
    ctrlCaps |= m_pDENC->SupportedControls();

  return ctrlCaps;
}


void CGenericGammaOutput::SetControl(stm_output_control_t ctrl, ULONG ulNewVal)
{
  switch(ctrl)
  {
    case STM_CTRL_YCBCR_COLORSPACE:
    case STM_CTRL_BACKGROUND_ARGB:
      m_pMixer->SetControl(ctrl, ulNewVal);
      break;
    case STM_CTRL_VIDEO_OUT_SELECT:
    {
      if(this->SetOutputFormat(ulNewVal))
      {
        m_ulOutputFormat = ulNewVal;
        if(m_pCurrentMode)
          this->EnableDACs();
      }
      break;
    }
    case STM_CTRL_CLOCK_ADJUSTMENT:
    {
      if(m_pFSynth)
      {
        /*
         * The value passed in is actually signed 2's complement
         */
        m_pFSynth->SetAdjustment((int)ulNewVal);
      }
      break;
    }
    case STM_CTRL_BRIGHTNESS:
    {
      m_ulBrightness = ulNewVal;
      if(m_pCurrentMode)
        ProgramPSIControls();

      break;
    }
    case STM_CTRL_SATURATION:
    {
      m_ulSaturation = ulNewVal;
      if(m_pCurrentMode)
        ProgramPSIControls();

      break;
    }
    case STM_CTRL_CONTRAST:
    {
      m_ulContrast = ulNewVal;
      if(m_pCurrentMode)
        ProgramPSIControls();

      break;
    }
    case STM_CTRL_HUE:
    {
      m_ulHue = ulNewVal;
      if(m_pCurrentMode)
        ProgramPSIControls();

      break;
    }
    case STM_CTRL_SIGNAL_RANGE:
    {
      if(ulNewVal> STM_SIGNAL_VIDEO_RANGE)
        break;

      m_signalRange = (stm_display_signal_range_t)ulNewVal;
      if(m_pCurrentMode && m_pDENC && m_bUsingDENC)
        m_pDENC->SetControl(STM_CTRL_SIGNAL_RANGE, (ULONG)m_signalRange);

      break;
    }
    case STM_CTRL_DAC123_MAX_VOLTAGE:
    {
      DEBUGF2(2,("CGenericGammaOutput::SetControl max dac voltage = %lu\n",ulNewVal));
      m_maxDACVoltage = ulNewVal;

      RecalculateDACSetup();
      this->SetOutputFormat(m_ulOutputFormat);

      if(m_pDENC)
        m_pDENC->SetControl(ctrl,ulNewVal);
      break;
    }
    case STM_CTRL_DAC123_SATURATION_POINT:
    {
      DEBUGF2(2,("CGenericGammaOutput::SetControl saturation point = %lu\n",ulNewVal));
      m_DACSaturation = ulNewVal;

      RecalculateDACSetup();
      this->SetOutputFormat(m_ulOutputFormat);

      if(m_pDENC)
        m_pDENC->SetControl(ctrl,ulNewVal);
      break;
    }
    case STM_CTRL_MAX_PIXEL_CLOCK:
    {
      if(ulNewVal < 150000000)
        m_ulMaxPixClock = ulNewVal;
    }
    default:
      if(m_pDENC)
        m_pDENC->SetControl(ctrl, ulNewVal);
      break;
  }
}


ULONG CGenericGammaOutput::GetControl(stm_output_control_t ctrl) const
{
  switch(ctrl)
  {
    case STM_CTRL_YCBCR_COLORSPACE:
    case STM_CTRL_BACKGROUND_ARGB:
      return m_pMixer->GetControl(ctrl);

    case STM_CTRL_CLOCK_ADJUSTMENT:
      if(m_pFSynth)
        return (ULONG)m_pFSynth->GetAdjustment(); /* Note this is actually signed */
      else
        return 0;

    case STM_CTRL_AV_SOURCE_SELECT:
      return m_ulInputSource;

    case STM_CTRL_VIDEO_OUT_SELECT:
      return m_ulOutputFormat;

    case STM_CTRL_BRIGHTNESS:
      return m_ulBrightness;

    case STM_CTRL_SATURATION:
      return m_ulSaturation;

    case STM_CTRL_CONTRAST:
      return m_ulContrast;

    case STM_CTRL_HUE:
      return m_ulHue;

    case STM_CTRL_SIGNAL_RANGE:
      return m_signalRange;

    case STM_CTRL_DAC123_MAX_VOLTAGE:
      return m_maxDACVoltage;

    case STM_CTRL_DAC123_SATURATION_POINT:
      return m_DACSaturation;

    case STM_CTRL_MAX_PIXEL_CLOCK:
      return m_ulMaxPixClock;

    default:
      return m_pDENC->GetControl(ctrl);
  }
}


bool CGenericGammaOutput::SetFilterCoefficients(const stm_display_filter_setup_t *f)
{
  bool ret = false;
  DENTRY();
  switch(f->type)
  {
    case DENC_COEFF_LUMA:
    case DENC_COEFF_CHROMA:
      ret = m_pDENC->SetFilterCoefficients(f);
      break;
    default:
      break;
  }
  DEXIT();
  return ret;
}


void CGenericGammaOutput::ProgramPSIControls(void)
{
  DENTRY();
  /*
   * Default is to defer this down to the DENC, future pipelines may
   * support control on HD DACs as well.
   */
  if(m_pDENC && m_bUsingDENC)
  {
    m_pDENC->SetControl(STM_CTRL_BRIGHTNESS,m_ulBrightness);
    m_pDENC->SetControl(STM_CTRL_CONTRAST,  m_ulContrast);
    m_pDENC->SetControl(STM_CTRL_SATURATION,m_ulSaturation);
    m_pDENC->SetControl(STM_CTRL_HUE,       m_ulHue);
  }

  DEXIT();
}

void CGenericGammaOutput::RecalculateDACSetup(void)
{
  DENTRY();

  /*
   * Calculate DAC levels for synchronisation insertion and rescaling.
   * We need to set things up to give the correct output voltages
   * based on the maximum voltage range of the DACs. This is partly
   * dependent on the reference voltage setup on a specific board, so this
   * can be set by the STM_CTRL_DAC_MAX_VOLTAGE and STM_CTRL_DAC_SATURATION
   * controls. The voltage is specified in mV.
   */

  /*
   * RGB and Y active signals are offset by 0.321V to allow
   * for negative sync pulses.
   */
  m_DAC_321mV = (m_DACSaturation*321)/m_maxDACVoltage;

  DEBUGF2(2,("CGenericGammaOutput::RecalculateDACSetup 321mv = 0x%lx (%lu)\n",m_DAC_321mV,m_DAC_321mV));

  /*
   * Sync pulses are +-43IRE from the blanking level, where
   * 43IRE = 0.301V
   */
  m_DAC_43IRE = (m_DACSaturation*301)/m_maxDACVoltage;

  DEBUGF2(2,("CGenericGammaOutput::RecalculateDACSetup 43IRE = 0x%lx (%lu)\n",m_DAC_43IRE,m_DAC_43IRE));

  /*
   * All video signals (including Pr & Pb) have a 0.7V range
   */
  m_DAC_700mV = (m_DACSaturation*700)/m_maxDACVoltage;

  DEBUGF2(2,("CGenericGammaOutput::RecalculateDACSetup 700mV = 0x%lx (%lu)\n",m_DAC_700mV,m_DAC_700mV));

  /*
   * We assume that for RGB we map the full range of output from
   * the compositor (0-1023) to the DAC_Video_Range
   */
  m_DAC_RGB_Scale = m_DAC_700mV;

  /*
   * These are the Y & C values (in 8bit values) where video usually saturates
   * in the DENC upscaled to the useable range of the DACs.
   */
  ULONG Compositor_Y_White_Value = (235 * m_DACSaturation+1)/256;
  ULONG Compositor_Y_Black_Value = (16  * m_DACSaturation+1)/256;
  ULONG Compositor_C_Max_Value   = (240 * m_DACSaturation+1)/256;
  ULONG Compositor_C_Mid_Value   = (128 * m_DACSaturation+1)/256;
  ULONG Compositor_C_Min_Value   = (16  * m_DACSaturation+1)/256;

  /*
   * Chroma is centered around 0.653V
   */
  ULONG DAC_653mV = (m_DACSaturation*653)/m_maxDACVoltage;
  DEBUGF2(2,("CGenericGammaOutput::RecalculateDACSetup 653mv = 0x%lx (%lu)\n",DAC_653mV,DAC_653mV));

  /*
   * We scale the _active_ range of luma and chroma from the compositor to
   * the DAC_Video_Range. Then we offset the output so that the Black/0 level
   * chroma levels are located at the appropriate voltages.
   */
  m_DAC_Y_Scale   = (m_DACSaturation * m_DAC_700mV) / (Compositor_Y_White_Value - Compositor_Y_Black_Value);
  m_DAC_Y_Offset  = m_DAC_321mV - ((Compositor_Y_Black_Value * m_DAC_Y_Scale) / m_DACSaturation);

  m_DAC_C_Scale   = (m_DACSaturation * m_DAC_700mV) / (Compositor_C_Max_Value - Compositor_C_Min_Value);
  m_DAC_C_Offset  = DAC_653mV - ((Compositor_C_Mid_Value * m_DAC_C_Scale) / m_DACSaturation);

  /*
   * Used in the AWG based devices to shift its signal output on the chroma
   * channels. Although we don't support syncs on chroma at the moment, this is
   * still needed to get the correct chroma blanking level, where the syncs
   * would otherwise be.
   */
  m_AWG_Y_C_Offset = DAC_653mV - m_DAC_Y_Offset;

  DEXIT();
}


void CGenericGammaOutput::SetClockReference(stm_clock_ref_frequency_t refClock, int error_ppm)
{
  DENTRY();

  if(m_pFSynth)
    m_pFSynth->SetClockReference(refClock,error_ppm);

  DEXIT();
}


stm_meta_data_result_t CGenericGammaOutput::QueueMetadata(stm_meta_data_t *m)
{
  DENTRY();

  switch(m->type)
  {
    case STM_METADATA_TYPE_PICTURE_INFO:
    case STM_METADATA_TYPE_EBU_TELETEXT:
      if(m_pDENC && m_bUsingDENC)
        return m_pDENC->QueueMetadata(m);
      else
        return STM_METADATA_RES_QUEUE_UNAVAILABLE;

    default:
      return STM_METADATA_RES_UNSUPPORTED_TYPE;
  }
}


void CGenericGammaOutput::FlushMetadata(stm_meta_data_type_t type)
{
  DENTRY();

  switch(type)
  {
    case STM_METADATA_TYPE_PICTURE_INFO:
    case STM_METADATA_TYPE_EBU_TELETEXT:
      if(m_pDENC && m_bUsingDENC)
        m_pDENC->FlushMetadata(type);

      break;
    default:
      break;
  }

  DEXIT();
}
