/*
  OutputMixer.cpp - SPI implement file for the Sparduino SDK
  Copyright (C) 2018 Sony Semiconductor Solutions Corp.
  Copyright (c) 2017 Sony Corporation  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

//***************************************************************************
// Included Files
//***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include "sdk/cxd56_audio.h"
#include <arch/board/board.h>

#include "OutputMixer.h"

#include "memutil/msgq_id.h"

/****************************************************************************
 * Public API on OutputMixer Class
 ****************************************************************************/

err_t OutputMixer::create(void)
{
  /* Activate mixer feature. */

  AsCreateOutputMixParam_t output_mix_create_param;
  output_mix_create_param.msgq_id.mixer = MSGQ_AUD_OUTPUT_MIX;

  bool result = AS_CreateOutputMixer(&output_mix_create_param);

  if (!result)
    {
      print_err("Error: AS_ActivateOutputMix() failed. system memory insufficient!\n");
      return OUTPUTMIXER_ECODE_COMMAND_ERROR;
    }

  /* Activate renderer feature. */

  AsActRendererParam_t renderer_act_param;

  renderer_act_param.msgq_id.dev0_req  = MSGQ_AUD_RND_PLY;
  renderer_act_param.msgq_id.dev0_sync = MSGQ_AUD_RND_PLY_SYNC;
  renderer_act_param.msgq_id.dev1_req  = 0xFF;
  renderer_act_param.msgq_id.dev1_sync = 0xFF;

  result = AS_ActivateRenderer(&renderer_act_param);
  if (!result)
    {
      print_err("Error: AS_ActivateRenderer() failure. system memory insufficient!\n");
      return OUTPUTMIXER_ECODE_COMMAND_ERROR;
    }

  return OUTPUTMIXER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t OutputMixer::activate(AsOutputMixerHandle handle, OutputMixerCallback omcb)
{
  AsActivateOutputMixer mixer_act;

  mixer_act.output_device = HPOutputDevice;
  mixer_act.mixer_type    = MainOnly;
  mixer_act.cb            = omcb;

  AS_ActivateOutputMixer(handle, mixer_act);

  return OUTPUTMIXER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t OutputMixer::sendData(AsOutputMixerHandle handle,
                            PcmProcDoneCallback pcmdone_cb,
                            MemMgrLite::MemHandle mh,
                            uint32_t sample,
                            uint32_t size,
                            bool is_end,
                            bool is_valid)
{
  AsSendDataOutputMixer data;

  data.handle   = handle;
  data.callback = pcmdone_cb;
  data.pcm.mh        = mh;
  data.pcm.sample    = sample;
  data.pcm.size      = size;
  data.pcm.is_end    = is_end;
  data.pcm.is_valid  = is_valid;

  AS_SendDataOutputMixer(data);

  return OUTPUTMIXER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t OutputMixer::deactivate(AsOutputMixerHandle handle)
{
  AsDeactivateOutputMixer mixer_deact;

  AS_DeactivateOutputMixer(handle, mixer_deact);

  return OUTPUTMIXER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t OutputMixer::activateBaseband(void)
{
  /* Power on and set up baseband
   * Sample rate = 48kHzm, Bypass mode = Disable
   */

  uint32_t rate[AS_I2S_ID_NUM] =
  {
    48000,
    48000
  };

  asBypassModeId bypass_mode_en[AS_I2S_ID_NUM] =
  {
    AS_I2S_BP_MODE_DISABLE,
    AS_I2S_BP_MODE_DISABLE
  };

  E_AS error_code;

  error_code = AS_PowerOnBaseBand(rate, bypass_mode_en);

  if (error_code != E_AS_OK)
    {
      print_err("AS_PowerOnBaseBand() error!\n");
      return OUTPUTMIXER_ECODE_COMMAND_ERROR;
    }

  error_code = AS_BaseBandEnable_output(AS_OUT_DEV_SP);

  if (error_code != E_AS_OK)
    {
      print_err("AS_BaseBandEnable_output() error!\n");
      return OUTPUTMIXER_ECODE_COMMAND_ERROR;
    }

  error_code = AS_SetOutputSelect(AS_OUT_DEV_SP);

   if (error_code != E_AS_OK)
    {
      print_err("AS_SetOutputSelect() error!\n");
      return OUTPUTMIXER_ECODE_COMMAND_ERROR;
    }

  return OUTPUTMIXER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t OutputMixer::deactivateBaseband(void)
{
  /* Disable output */

  E_AS error_code = E_AS_OK;

  error_code = AS_BaseBandDisable_output();

  if (error_code != E_AS_OK)
    {
      print_err("AS_BaseBandDisable_output() error!\n");
      return OUTPUTMIXER_ECODE_COMMAND_ERROR;
    }

  /* Power Off Baseband */

  error_code = AS_PowerOffBaseBand();

  if (error_code != E_AS_OK)
    {
      print_err("AS_PowerOffBaseBand() error!\n");
      return OUTPUTMIXER_ECODE_COMMAND_ERROR;
    }

  return OUTPUTMIXER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t OutputMixer::setVolume(int master, int player0, int player1)
{
  asCodecVol vol;

  vol.input1_db = player0;
  vol.input2_db = player1;
  vol.master_db = master;

  E_AS ret = AS_SetVolume(&vol);

  if (ret != E_AS_OK)
    {
      return OUTPUTMIXER_ECODE_COMMAND_ERROR;
    }

  return OUTPUTMIXER_ECODE_OK;
}


