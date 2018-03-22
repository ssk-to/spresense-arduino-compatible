/*
  Audio.cpp - SPI implement file for the Sparduino SDK
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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <nuttx/init.h>
#include <nuttx/arch.h>
#include <arch/chip/pm.h>
#include <arch/board/board.h>

#include "Audio.h"

//***************************************************************************
// Definitions
//***************************************************************************
// Configuration ************************************************************
// C++ initialization requires CXX initializer support

#define _POSIX
#define USE_MEMMGR_FENCE

#include <asmp/mpshm.h>

//#include <fcntl.h>

#include <memutils/memory_manager/MemHandle.h>
#include <memutils/message/Message.h>

#include "memutil/msgq_id.h"
#include "memutil/mem_layout.h"
#include "memutil/memory_layout.h"
#include "memutil/fixed_fence.h"
#include "memutil/msgq_pool.h"
#include "memutil/pool_layout.h"

using namespace MemMgrLite;

extern "C" void  input_device_callback(uint32_t);
extern "C" void  output_device_callback(uint32_t);

/****************************************************************************
 * Common API on Audio Class
 ****************************************************************************/
err_t AudioClass::begin(void)
{
  int ret;

  ret = initMemoryPools();
  if (ret != AUDIOLIB_ECODE_OK)
    {
      print_err("Memory pool initilization error.\n");
      return ret;
    }

  ret = activateAudio();
  if (ret != AUDIOLIB_ECODE_OK)
    {
      print_err("Audio activation error.\n");
  return ret;
    }

  return ret;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::end(void)
{
  /*Do not implimentation yet.*/
  int ret = 0;
  return ret;
}

/****************************************************************************
 * Private Common API for begin/end
 ****************************************************************************/
err_t AudioClass::initMemoryPools(void)
{
  mpshm_t s_shm;
  int ret = mpshm_init(&s_shm, 1,  RAM_TILE_SIZE * 2); /* Used 2 Tile */
  if (ret < 0)
    {
      print_err("mpshm_init() failure. %d\n", ret);
      return AUDIOLIB_ECODE_SHARED_MEMORY_ERROR;
    }

  uint32_t addr = AUD_SRAM_ADDR;
  ret = mpshm_remap(&s_shm, (void *)addr);
  if (ret < 0)
    {
      print_err("mpshm_remap() failure. %d\n", ret);
      return AUDIOLIB_ECODE_SHARED_MEMORY_ERROR;
    }

  /* Initalize MessageLib */
  MsgLib::initFirst(NUM_MSGQ_POOLS,MSGQ_TOP_DRM);
  MsgLib::initPerCpu();

  void* mml_data_area = translatePoolAddrToVa(MEMMGR_DATA_AREA_ADDR);
  Manager::initFirst(mml_data_area, MEMMGR_DATA_AREA_SIZE);

  Manager::initPerCpu(mml_data_area, NUM_MEM_POOLS);

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::activateAudio(void)
{
  AudioSubSystemIDs ids;
  int ret = AUDIOLIB_ECODE_OK;

  ids.app         = MSGQ_AUD_APP;
  ids.mng         = MSGQ_AUD_MGR;
  ids.player_main = MSGQ_AUD_PLY;
  ids.player_sub  = MSGQ_AUD_SUB_PLY;
  ids.mixer       = MSGQ_AUD_OUTPUT_MIX;
  ids.recorder    = MSGQ_AUD_RECORDER;
  ids.effector    = 0xFF;
  ids.recognizer  = 0xFF;

  AS_ActivateAudioSubSystem(ids);

  /*ここまででパワーオンもする?*/
  ret = powerOn();
  if (ret != AUDIOLIB_ECODE_OK)
    {
      print_err("Power On error.\n");
      return ret;
    }

  ret = initAttention();
  if (ret != AUDIOLIB_ECODE_OK)
    {
      print_err("Attention initialize error.\n");
      return ret;
    }

  return ret;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::powerOn(void)
{
  AudioCommand command;
  command.header.packet_length = LENGTH_POWERON;
  command.header.command_code  = AUDCMD_POWERON;
  command.header.sub_code      = 0x00;
  command.power_on_param.enable_sound_effect = AS_DISABLE_SOUNDEFFECT;
  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_STATUSCHANGED)
    {
      print_err("ERROR: Command (%x) fails. Result code(%x)\n", command.header.command_code, result.header.result_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  /*TODO:外部アンプの制御は詳細に。*/
  print_dbg("power on!\n");

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::powerOff(void)
{
  AudioCommand command;
  command.header.packet_length = LENGTH_SET_POWEROFF_STATUS;
  command.header.command_code  = AUDCMD_SETPOWEROFFSTATUS;
  command.header.sub_code      = 0x00;
  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_STATUSCHANGED)
    {
      print_err("ERROR: Command (%x) fails. Result code(%x)\n", command.header.command_code, result.header.result_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}


/*--------------------------------------------------------------------------*/
extern "C" {

void attentionCallback(unsigned char module_id, unsigned char error_code,unsigned char sub_code)
{
  print_dbg("attention!! ecode %d subcode %d\n", error_code, sub_code);
}

}

/*--------------------------------------------------------------------------*/
err_t AudioClass::initAttention(void)
{
  AudioCommand command;
  command.header.packet_length = LENGTH_INITATTENTIONS;
  command.header.command_code  = AUDCMD_INITATTENTIONS;
  command.header.sub_code      = 0x00;
  command.init_attentions_param.attention_callback_function = attentionCallback;
  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_INITATTENTIONSCMPLT)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}



/*--------------------------------------------------------------------------*/
err_t AudioClass::setReadyMode(void)
{
  AudioCommand command;
  command.header.packet_length = LENGTH_SET_READY_STATUS;
  command.header.command_code  = AUDCMD_SETREADYSTATUS;
  command.header.sub_code      = 0x00;
  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_STATUSCHANGED)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}
/****************************************************************************
 * Player API on Audio Class
 ****************************************************************************/
err_t AudioClass::setPlayerMode(uint8_t device)
{
  void* work_va = translatePoolAddrToVa(MEMMGR_WORK_AREA_ADDR);
  const NumLayout layout_no = MEM_LAYOUT_PLAYER;

  assert(layout_no < NUM_MEM_LAYOUTS);
  Manager::createStaticPools(layout_no, work_va, MEMMGR_MAX_WORK_SIZE, MemoryPoolLayouts[layout_no]);

  AsActPlayerParam_t player_act_param;
  player_act_param.msgq_id.player = MSGQ_AUD_PLY;
  player_act_param.msgq_id.mng    = MSGQ_AUD_MGR;
  player_act_param.msgq_id.mixer  = MSGQ_AUD_OUTPUT_MIX;
  player_act_param.msgq_id.dsp    = MSGQ_AUD_DSP;
  player_act_param.pool_id.es     = DEC_ES_MAIN_BUF_POOL;
  player_act_param.pool_id.pcm    = REND_PCM_BUF_POOL;
  player_act_param.pool_id.dsp    = DEC_APU_CMD_POOL;

  int act_rst = AS_ActivatePlayer(&player_act_param);
  if (!act_rst)
    {
      print_err("AS_ActivatePlayer failed. system memory insufficient!\n");
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  player_act_param.msgq_id.player = MSGQ_AUD_SUB_PLY;
  player_act_param.msgq_id.mng    = MSGQ_AUD_MGR;
  player_act_param.msgq_id.mixer  = MSGQ_AUD_OUTPUT_MIX;
  player_act_param.msgq_id.dsp    = MSGQ_AUD_DSP;
  player_act_param.pool_id.es     = DEC_ES_SUB_BUF_POOL;
  player_act_param.pool_id.pcm    = REND_PCM_SUB_BUF_POOL;
  player_act_param.pool_id.dsp    = DEC_APU_CMD_POOL;

  act_rst = AS_ActivateSubPlayer(&player_act_param);
  if (!act_rst)
    {
      printf("AS_ActivateSubPlayer failed. system memory insufficient!\n");
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  AsActOutputMixParam_t output_mix_act_param;

  output_mix_act_param.msgq_id.mixer = MSGQ_AUD_OUTPUT_MIX;
  output_mix_act_param.msgq_id.mng   = MSGQ_AUD_MGR;

  act_rst = AS_ActivateOutputMix(&output_mix_act_param);
  if (!act_rst)
    {
      print_err("AS_ActivateOutputMix failed. system memory insufficient!\n");
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  print_dbg("cmplt Activation\n");

  AudioClass::set_output(device);

  AsActRendererParam_t renderer_act_param;
  renderer_act_param.msgq_id.dev0_req  = MSGQ_AUD_RND_PLY;
  renderer_act_param.msgq_id.dev0_sync = MSGQ_AUD_RND_PLY_SYNC;
  renderer_act_param.msgq_id.dev1_req   = MSGQ_AUD_RND_SUB;
  renderer_act_param.msgq_id.dev1_sync  = MSGQ_AUD_RND_SUB_SYNC;

  act_rst = AS_ActivateRenderer(&renderer_act_param);
  if (!act_rst)
    {
      print_err("AS_ActivateRenderer failed. system memory insufficient!\n");
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  print_dbg("set output cmplt\n");


  if (CMN_SimpleFifoInitialize(&m_player0_simple_fifo_handle, m_player0_simple_fifo_buf, SIMPLE_FIFO_BUF_SIZE, NULL) != 0)
    {
      print_err("Fail to initialize simple FIFO.\n");
      return AUDIOLIB_ECODE_SIMPLEFIFO_ERROR;
    }
  CMN_SimpleFifoClear(&m_player0_simple_fifo_handle);

  if (CMN_SimpleFifoInitialize(&m_player1_simple_fifo_handle, m_player1_simple_fifo_buf, SIMPLE_FIFO_BUF_SIZE, NULL) != 0)
    {
      print_err("Fail to initialize simple FIFO.\n");
      return AUDIOLIB_ECODE_SIMPLEFIFO_ERROR;
    }
  CMN_SimpleFifoClear(&m_player1_simple_fifo_handle);

  m_player0_input_device_handler.simple_fifo_handler = (void*)(&m_player0_simple_fifo_handle);
  m_player0_input_device_handler.callback_function = input_device_callback; /*??*/

  m_player1_input_device_handler.simple_fifo_handler = (void*)(&m_player1_simple_fifo_handle);
  m_player1_input_device_handler.callback_function = input_device_callback; /*??*/

  AudioCommand command;
  command.header.packet_length = LENGTH_SET_PLAYER_STATUS;
  command.header.command_code  = AUDCMD_SETPLAYERSTATUS;
  command.header.sub_code      = 0x00;

  command.set_player_sts_param.active_player          = AS_ACTPLAYER_BOTH;
  command.set_player_sts_param.input_device           = AS_SETPLAYER_INPUTDEVICE_RAM;
  command.set_player_sts_param.ram_handler            = &m_player0_input_device_handler;
  command.set_player_sts_param.output_device          = device;
  command.set_player_sts_param.output_device_handler  = 0x00;
  command.set_player_sts_param.input_device_sub       = AS_SETPLAYER_INPUTDEVICE_RAM;
  command.set_player_sts_param.ram_handler_sub        = &m_player1_input_device_handler;
  command.set_player_sts_param.output_device_sub      = device;
  command.set_player_sts_param.output_device_handler_sub  = 0x00;

  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_STATUSCHANGED)
    {
      print_err("ERROR: Command (%x) fails. Result code(%x), subcode = %x\n", command.header.command_code, result.header.result_code,result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}


/*--------------------------------------------------------------------------*/
err_t AudioClass::initPlayer(PlayerId id, uint8_t codec_type, uint32_t sampling_rate, uint8_t channel_number)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_INIT_PLAYER;
  command.header.command_code  = (id == Player0) ? AUDCMD_INITPLAYER : AUDCMD_INITSUBPLAYER;
  command.header.sub_code      = 0x00;

  command.init_player_param.codec_type    = codec_type;
  command.init_player_param.bit_length    = AS_BITLENGTH_16;
  command.init_player_param.channel_number= channel_number;
  command.init_player_param.sampling_rate = sampling_rate;
  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if ((result.header.result_code != AUDRLT_INITPLAYERCMPLT) &&
      (result.header.result_code != AUDRLT_SUBINITPLAYERCMPLT))
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::startPlayer(PlayerId id)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_PLAY_PLAYER;
  command.header.command_code  = (id == Player0) ? AUDCMD_PLAYPLAYER : AUDCMD_PLAYSUBPLAYER;
  command.header.sub_code      = 0x00;
  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if ((result.header.result_code != AUDRLT_PLAYCMPLT) &&
      (result.header.result_code != AUDRLT_SUBPLAYCMPLT))
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x) Error subcode(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code, result.error_response_param.error_sub_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  board_external_amp_mute_control(false);

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::setBeep(char en, short vol, short freq)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_SETBEEPPARAM;
  command.header.command_code  = AUDCMD_SETBEEPPARAM;
  command.header.sub_code      = 0;

  command.set_beep_param.beep_en   = en;
  command.set_beep_param.beep_vol  = vol;
  command.set_beep_param.beep_freq = freq;
  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_SETBEEPCMPLT)
    {
      print_err("ERROR: Command (%x) fails. Result code(%x) Error code(0x%x)\n",
                command.header.command_code, result.header.result_code, result.error_response_param.error_code);

      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::stopPlayer(PlayerId id)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_STOP_PLAYER;
  command.header.command_code  = (id == Player0) ? AUDCMD_STOPPLAYER : AUDCMD_STOPSUBPLAYER;
  command.header.sub_code      = 0x00;

  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if ((result.header.result_code != AUDRLT_STOPCMPLT) &&
      (result.header.result_code != AUDRLT_SUBSTOPCMPLT))
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  CMN_SimpleFifoHandle *handle = (id == Player0) ? &m_player0_simple_fifo_handle : &m_player1_simple_fifo_handle;
  CMN_SimpleFifoClear(handle);

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::setVolume(int master_db)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_SETVOLUME;
  command.header.command_code  = AUDCMD_SETVOLUME;
  command.header.sub_code      = 0;

  command.set_volume_param.input1_db = AS_VOLUME_HOLD;
  command.set_volume_param.input2_db = AS_VOLUME_HOLD;
  command.set_volume_param.master_db = master_db;

  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_SETVOLUMECMPLT)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::setVolume(int master, int player0, int player1)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_SETVOLUME;
  command.header.command_code  = AUDCMD_SETVOLUME;
  command.header.sub_code      = 0;

  command.set_volume_param.input1_db = player0;
  command.set_volume_param.input2_db = player1;
  command.set_volume_param.master_db = master;

  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_SETVOLUMECMPLT)
    {
      printf("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
             command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::setLRgain(PlayerId id, unsigned char l_gain, unsigned char r_gain)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_SET_GAIN;
  command.header.command_code  = (id == Player0) ? AUDCMD_SETGAIN : AUDCMD_SETGAINSUB;
  command.header.sub_code      = 0;

  command.set_gain_param.l_gain = l_gain;
  command.set_gain_param.r_gain = r_gain;

  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if ((result.header.result_code != AUDRLT_SETGAIN_CMPLT) &&
      (result.header.result_code != AUDRLT_SETGAINSUB_CMPLT))
    {
      printf("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

#define WRITE_FRAME_NUM 5 
/*--------------------------------------------------------------------------*/
/*err_t AudioClass::writeFrames(PlayerId id, int fd)
{
  int ret = AUDIOLIB_ECODE_OK;
  char *buf = (id == Player0) ? m_es_player0_buf : m_es_player1_buf; 
  CMN_SimpleFifoHandle *handle = (id == Player0) ? m_player0_simple_fifo_handle : m_player1_simple_fifo_handle;

  for (int i = 0; i < WRITE_FRAME_NUM; i++)
    {
        ret = write_fifo(fd, buf, handle);
      if(ret != AUDIOLIB_ECODE_OK) break;
    }

    return ret;
}*/

/*--------------------------------------------------------------------------*/
err_t AudioClass::writeFrames(PlayerId id, File& myFile)
{
  int ret = AUDIOLIB_ECODE_OK;
  char *buf = (id == Player0) ? m_es_player0_buf : m_es_player1_buf; 
  CMN_SimpleFifoHandle *handle = (id == Player0) ? &m_player0_simple_fifo_handle : &m_player1_simple_fifo_handle;

  for (int i = 0; i < WRITE_FRAME_NUM; i++)
    {
      ret = write_fifo(myFile, buf, handle);
      if (ret != AUDIOLIB_ECODE_OK) break;
    }

  return ret;
}


/****************************************************************************
 * Recoder API on Audio Class
 ****************************************************************************/
#define m_recorder_simple_fifo_handle m_player0_simple_fifo_handle
#define m_recorder_simple_fifo_buf m_player0_simple_fifo_buf
#define m_es_recorder_buf m_es_player0_buf

err_t AudioClass::setRecorderMode(uint8_t input_device)
{
  const NumLayout layout_no = MEM_LAYOUT_RECORDER;
  void* work_va = translatePoolAddrToVa(MEMMGR_WORK_AREA_ADDR);
  Manager::createStaticPools(layout_no, work_va, MEMMGR_MAX_WORK_SIZE, MemoryPoolLayouts[layout_no]);

  AsActRecorderParam_t recorder_act_param;
  recorder_act_param.msgq_id.recorder      = MSGQ_AUD_RECORDER;
  recorder_act_param.msgq_id.mng           = MSGQ_AUD_MGR;
  recorder_act_param.msgq_id.dsp           = MSGQ_AUD_DSP;
  recorder_act_param.pool_id.input         = MIC_IN_BUF_POOL;
  recorder_act_param.pool_id.output        = OUTPUT_BUF_POOL;
  recorder_act_param.pool_id.dsp           = ENC_APU_CMD_POOL;

  if (!AS_ActivateVoiceRecorder(&recorder_act_param))
    {
      print_err("AS_ActivateVoiceRecorder failed. system memory insufficient!\n");
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  AsActCaptureParam_t capture_act_param;
  capture_act_param.msgq_id.dev0_req  = MSGQ_AUD_CAP;
  capture_act_param.msgq_id.dev0_sync = MSGQ_AUD_CAP_SYNC;
  capture_act_param.msgq_id.dev1_req  = 0xFF;
  capture_act_param.msgq_id.dev1_sync = 0xFF;

  if (!AS_ActivateCapture(&capture_act_param))
    {
      print_err("AS_ActivateCapture failed. system memory insufficient!\n");
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  if (CMN_SimpleFifoInitialize(&m_recorder_simple_fifo_handle, m_recorder_simple_fifo_buf, SIMPLE_FIFO_BUF_SIZE, NULL) != 0)
    {
      print_err("Fail to initialize simple FIFO.\n");
      return AUDIOLIB_ECODE_SIMPLEFIFO_ERROR;
    }

  CMN_SimpleFifoClear(&m_recorder_simple_fifo_handle);

  m_output_device_handler.simple_fifo_handler = (void*)(&m_recorder_simple_fifo_handle);
  m_output_device_handler.callback_function = output_device_callback;

  AudioCommand command;

  command.header.packet_length = LENGTH_SET_RECORDER_STATUS;
  command.header.command_code = AUDCMD_SETRECORDERSTATUS;
  command.header.sub_code = 0x00;

  command.set_recorder_status_param.input_device = input_device;
  command.set_recorder_status_param.input_device_handler = 0x00;
  command.set_recorder_status_param.output_device = AS_SETRECDR_STS_OUTPUTDEVICE_RAM;
  command.set_recorder_status_param.output_device_handler = &m_output_device_handler;

  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_STATUSCHANGED)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  if (init_mic_gain(input_device, 210) != AUDIOLIB_ECODE_OK)
    {
      print_err("Mic init error!");
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::init_recorder_wav(AudioCommand* command, uint32_t sampling_rate, uint8_t channel_number)
{
  command->init_recorder_param.sampling_rate  = sampling_rate;
  command->init_recorder_param.channel_number = channel_number;
  command->init_recorder_param.bit_length     = AS_BITLENGTH_16;
  command->init_recorder_param.codec_type     = m_codec_type;
  AS_SendAudioCommand(command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);
  if (result.header.result_code != AUDRLT_INITRECCMPLT)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
                command->header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  memcpy(m_wav_format.riff, CHUNKID_RIFF, strlen(CHUNKID_RIFF));
  memcpy(m_wav_format.wave, FORMAT_WAVE, strlen(FORMAT_WAVE));
  memcpy(m_wav_format.fmt, SUBCHUNKID_FMT, strlen(SUBCHUNKID_FMT));
  m_wav_format.fmt_size = FMT_SIZE;
  m_wav_format.format   = AUDIO_FORMAT_PCM;
  m_wav_format.channel  = channel_number;
  m_wav_format.rate     = sampling_rate;
  m_wav_format.avgbyte  = sampling_rate * channel_number * 2;
  m_wav_format.block    = channel_number * 2;
  m_wav_format.bit      = 2 * 8;
  memcpy(m_wav_format.data, SUBCHUNKID_DATA, strlen(SUBCHUNKID_DATA));

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::init_recorder_mp3(AudioCommand* command, uint32_t sampling_rate, uint8_t channel_number)
{
  command->init_recorder_param.sampling_rate  = sampling_rate;
  command->init_recorder_param.channel_number = channel_number;
  command->init_recorder_param.bit_length     = AS_BITLENGTH_16;
  command->init_recorder_param.codec_type     = m_codec_type;
  command->init_recorder_param.bitrate        = AS_BITRATE_96000;
  AS_SendAudioCommand(command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);
  if (result.header.result_code != AUDRLT_INITRECCMPLT)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
                command->header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::init_recorder_opus(AudioCommand* command, uint32_t sampling_rate, uint8_t channel_number)
{
  command->init_recorder_param.sampling_rate  = sampling_rate;
  command->init_recorder_param.channel_number = channel_number;
  command->init_recorder_param.bit_length     = AS_BITLENGTH_16;
  command->init_recorder_param.codec_type     = m_codec_type;
  command->init_recorder_param.bitrate        = AS_BITRATE_8000;
  command->init_recorder_param.computational_complexity = AS_INITREC_COMPLEXITY_0;
  AS_SendAudioCommand(command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);
  if (result.header.result_code != AUDRLT_INITRECCMPLT)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
                command->header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::init_recorder_pcm(AudioCommand* command, uint32_t sampling_rate, uint8_t channel_number)
{
  command->init_recorder_param.sampling_rate  = sampling_rate;
  command->init_recorder_param.channel_number = channel_number;
  command->init_recorder_param.bit_length     = AS_BITLENGTH_16;
  command->init_recorder_param.codec_type     = AS_CODECTYPE_WAV;
  AS_SendAudioCommand(command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);
  if (result.header.result_code != AUDRLT_INITRECCMPLT)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
                command->header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::initRecorder(uint8_t codec_type, uint32_t sampling_rate, uint8_t channel)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_INIT_RECORDER;
  command.header.command_code  = AUDCMD_INITREC;
  command.header.sub_code      = 0x00;

  m_codec_type = codec_type;

  int ret = AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
  switch (codec_type)
    {
      case AS_CODECTYPE_WAV:
        ret = init_recorder_wav(&command, sampling_rate, channel);
        break;

      case AS_CODECTYPE_MP3:
        ret = init_recorder_mp3(&command, sampling_rate, channel);
        break;

      case AS_CODECTYPE_OPUS:
        ret = init_recorder_opus(&command, sampling_rate, channel);
        break;

      case AS_CODECTYPE_PCM:
        ret = init_recorder_pcm(&command, sampling_rate, channel);
        break;

      default:
        break;
    }

  return ret;
}


/*--------------------------------------------------------------------------*/
err_t AudioClass::startRecorder(void)
{
  m_es_size = 0;

  CMN_SimpleFifoClear(&m_recorder_simple_fifo_handle);

  AudioCommand command;

  command.header.packet_length = LENGTH_START_RECORDER;
  command.header.command_code  = AUDCMD_STARTREC;
  command.header.sub_code      = 0x00;

  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_RECCMPLT)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x) Error subcode(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code, result.error_response_param.error_sub_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  print_dbg("start\n");

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::stopRecorder(void)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_STOP_RECORDER;
  command.header.command_code  = AUDCMD_STOPREC;
  command.header.sub_code      = 0x00;

  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_STOPRECCMPLT)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
#if 0
err_t AudioClass::closeOutputFile(int fd)
{
  do
    {
    } while (read_frames(fd) == 0);

  if (m_codec_type == AS_CODECTYPE_WAV)
    {
      writeWavHeader(fd);
    }

  fclose(fd);

  return true;
}
#endif 

/*--------------------------------------------------------------------------*/
err_t AudioClass::closeOutputFile(File& myFile)
{
  do
    {
      readFrames(myFile);
    } while (CMN_SimpleFifoGetOccupiedSize(&m_recorder_simple_fifo_handle) != 0);


  if (m_codec_type == AS_CODECTYPE_WAV)
    {
      writeWavHeader(myFile);
    }

  myFile.close();

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
/*err_t AudioClass::writeWavHeader(int fd)
{
  ssize_t ret;

  m_wav_format.total_size = m_es_size + sizeof(WavaFormat_t) - 8;
  m_wav_format.data_size = m_es_size;
  fseek(fd, 0, SEEK_SET);

  int ret = fwrite(&m_wav_format, 1, sizeof(WavaFormat_t), fd);
  if (ret < 0)
    {
      print_err("Fail to write file(wav header)\n");
      return false;
    }

  return AUDIOLIB_ECODE_OK;
}*/
/*--------------------------------------------------------------------------*/
err_t AudioClass::writeWavHeader(File& myFile)
{
  myFile.seek(0);

  m_wav_format.total_size = m_es_size + sizeof(WavaFormat_t) - 8;
  m_wav_format.data_size  = m_es_size;

  int ret = myFile.write((uint8_t*)&m_wav_format, sizeof(WavaFormat_t));
  if (ret < 0)
    {
      print_err("Fail to write file(wav header)\n");
      return AUDIOLIB_ECODE_FILEACCESS_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
#if 0
err_t AudioClass::readFrames(int fd)
{
  size_t data_size = CMN_SimpleFifoGetOccupiedSize(&m_recorder_simple_fifo_handle);

  while (data_size > 0)
    {
      int size = (data_size > FIFO_FRAME_SIZE) ? FIFO_FRAME_SIZE : data_size;

      /* TODO assert で良いよね…。*/
      if (CMN_SimpleFifoPoll(&m_recorder_simple_fifo_handle, (void*)m_es_recorder_buf, size) == 0)
        {
          print_err("ERROR: Fail to get data from simple FIFO.\n");
          return AUDIOLIB_ECODE_SIMPLEFIFO_ERROR;
        }

      int ret = fwrite(&m_es_recorder_buf, 1, size, fd);
      m_es_size += ret;
      deta_size -= size;

      if (ret < 0)
        {
          print_err("ERROR: Cannot write recorded data to output file.\n");
          fclose(fd);
          return AUDIOLIB_ECODE_FILEACCESS_ERROR;
        }
    }

  return 0;
}
#endif
/*--------------------------------------------------------------------------*/
err_t AudioClass::readFrames(File& myFile)
{
  size_t data_size = CMN_SimpleFifoGetOccupiedSize(&m_recorder_simple_fifo_handle);
  print_dbg("dsize = %d\n", data_size);

  while (data_size > 0)
    {
      int size = (data_size > FIFO_FRAME_SIZE) ? FIFO_FRAME_SIZE : data_size;

      /* TODO: assert で良いよね…。*/
      if (CMN_SimpleFifoPoll(&m_recorder_simple_fifo_handle, (void*)m_es_recorder_buf, size) == 0)
        {
          print_err("ERROR: Fail to get data from simple FIFO.\n");
          return AUDIOLIB_ECODE_SIMPLEFIFO_ERROR;
        }

      int ret = myFile.write((uint8_t*)&m_es_recorder_buf, size);
      m_es_size += size;
      data_size -= size;

      if (ret < 0)
        {
          print_err("ERROR: Cannot write recorded data to output file.\n");
          myFile.close();
          return AUDIOLIB_ECODE_FILEACCESS_ERROR;
        }
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t AudioClass::readFrames(char* p_buffer, uint32_t buffer_size, uint32_t* read_size)
{
  err_t rst = AUDIOLIB_ECODE_OK;
  if (p_buffer == NULL)
    {
      print_err("ERROR: Buffer area not specified.\n");
      return AUDIOLIB_ECODE_BUFFER_AREA_ERROR;
    }
  if (buffer_size == 0)
    {
      print_err("ERROR: Buffer area size error.\n");
      return AUDIOLIB_ECODE_BUFFER_SIZE_ERROR;
    }

  size_t data_size = CMN_SimpleFifoGetOccupiedSize(&m_recorder_simple_fifo_handle);
  print_dbg("dsize = %d\n", data_size);

  *read_size = 0;
  size_t poll_size = 0;
  if (data_size > 0)
    {
      if (data_size > buffer_size)
        {
          print_err("WARNING: Insufficient buffer area.\n");
          poll_size = (size_t)buffer_size;
          rst = AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA;
        }
      else
        {
          poll_size = data_size;
        }

      if (CMN_SimpleFifoPoll(&m_recorder_simple_fifo_handle, (void*)p_buffer, poll_size) == 0)
        {
          print_err("ERROR: Fail to get data from simple FIFO.\n");
          return AUDIOLIB_ECODE_SIMPLEFIFO_ERROR;
        }
      *read_size = (uint32_t)poll_size;

      m_es_size += data_size;
    }

  return rst;
}

/****************************************************************************
 * Private API on Audio Player
 ****************************************************************************/
extern "C" {
/*--------------------------------------------------------------------------*/
void  input_device_callback(uint32_t size)
{
    /* do nothing */
}
/*--------------------------------------------------------------------------*/
void  output_device_callback(uint32_t size)
{
    /* do nothing */
}

}

/*--------------------------------------------------------------------------*/
err_t AudioClass::set_output(int device)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_INITOUTPUTSELECT;
  command.header.command_code  = AUDCMD_INITOUTPUTSELECT;
  command.header.sub_code      = 0;

  command.init_output_select_param.output_device_sel = device;

  AS_SendAudioCommand(&command);

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_INITOUTPUTSELECTCMPLT)
    {
      sleep(1);
      print_err("ERROR: Command (%x) fails. Result code(%x), subcode = %x\n", command.header.command_code, result.header.result_code,result.error_response_param.error_code);

      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
/*err_t AudioClass::write_fifo(int fd, char *buf, CMN_SimpleFifoHandle *handle)
{

  int vacant_size = CMN_SimpleFifoGetVacantSize(handle);
  if (vacant_size < FIFO_FRAME_SIZE)
    {
      return AUDIOLIB_ECODE_SIMPLEFIFO_ERROR;
    }

  int ret = fread(fd, buf, FIFO_FRAME_SIZE);

  if (ret < 0)
    {
      print_err("Fail to read file. errno:%d\n",get_errno());
      return AUDIOLIB_ECODE_FILEACCESS_ERROR;
    }

       print_dbg("size = %d!\n",ret);

  if (CMN_SimpleFifoOffer(handle, (const void*)(buf), ret) == 0)
    {
      print_err("Simple FIFO is full!\n");
      return AUDIOLIB_ECODE_SIMPLEFIFO_ERROR;
    }

   if(ret == 0)
    {
      fclose(fd);
      return AUDIOLIB_ECODE_FILEEND;
    }

  return AUDIOLIB_ECODE_OK;
}
*/
/*--------------------------------------------------------------------------*/
err_t AudioClass::write_fifo(File& myFile, char *p_es_buf, CMN_SimpleFifoHandle *handle)
{

  int vacant_size = CMN_SimpleFifoGetVacantSize(handle);
  if (vacant_size < FIFO_FRAME_SIZE)
    {
      return AUDIOLIB_ECODE_OK;
    }

  int ret = -1;

  for (int i = 0; i < FIFO_FRAME_SIZE; i++)
    {
      if (myFile.available())
        {
          *(p_es_buf + i) = myFile.read();
        }
      else
        {
          ret = 0;
          break;
        }

      ret = i+1;
    }

  if (ret < 0)
    {
      print_err("Fail to read file. errno:%d\n",get_errno());
      return AUDIOLIB_ECODE_FILEACCESS_ERROR;
    }

  if(ret == 0)
    {
      myFile.close();
      return AUDIOLIB_ECODE_FILEEND;
    }

  if (CMN_SimpleFifoOffer(handle, (const void*)(p_es_buf), ret) == 0)
    {
      print_err("Simple FIFO is full!\n");
      return AUDIOLIB_ECODE_SIMPLEFIFO_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

/****************************************************************************
 * Private API on Audio Recorder
 ****************************************************************************/

err_t AudioClass::init_mic_gain(int dev, int gain)
{
  AudioCommand command;

  command.header.packet_length = LENGTH_INITMICGAIN;
  command.header.command_code  = AUDCMD_INITMICGAIN;
  command.header.sub_code      = 0;

  /* devで、アナログ、デジタルの選択。
  現在、未対応。
  各マイクのGainを個別に調整はしない方向。
  */
  command.init_mic_gain_param.mic_gain[0] = gain;
  command.init_mic_gain_param.mic_gain[1] = gain;
  command.init_mic_gain_param.mic_gain[2] = gain;
  command.init_mic_gain_param.mic_gain[3] = gain;
  command.init_mic_gain_param.mic_gain[4] = 0;
  command.init_mic_gain_param.mic_gain[5] = 0;
  command.init_mic_gain_param.mic_gain[6] = 0;
  command.init_mic_gain_param.mic_gain[7] = 0;

  AS_SendAudioCommand( &command );

  AudioResult result;
  AS_ReceiveAudioResult(&result);

  if (result.header.result_code != AUDRLT_INITMICGAINCMPLT)
    {
      print_err("ERROR: Command (0x%x) fails. Result code(0x%x) Module id(0x%x) Error code(0x%x)\n",
              command.header.command_code, result.header.result_code, result.error_response_param.module_id, result.error_response_param.error_code);
      return AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR;
    }

  return AUDIOLIB_ECODE_OK;
}

