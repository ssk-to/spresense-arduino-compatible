/*
 *  player_ObjIf.ino - Object I/F based sound player example application
 *  Copyright 2018 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <SDHCI.h>

#include <MediaPlayer.h>
#include <OutputMixer.h>
#include <MemoryUtil.h>
#include <fcntl.h>

SDClass theSD;

MediaPlayer *thePlayer;
OutputMixer *theMixer;

int fd;
File myFile;

static void outputmixer_done_callback(MsgQueId requester_dtq,
                                      MsgType reply_of,
                                      AsOutputMixDoneParam *done_param)
{
  return;
}

static void outmixer_send_callback(int32_t identifier, bool is_end)
{
  AsRequestNextParam next;

  next.type = (!is_end) ? AsNextNormalRequest : AsNextStopResRequest;

  AS_RequestNextPlayerProcess(AS_PLAYER_ID_0, next);

  return;
}

static bool mediaplayer_done_callback(AsPlayerEvent event, uint32_t result, uint32_t sub_result)
{
  printf("mp cb %x %x %x\n", event, result, sub_result);

  return true;
}

void mediaplayer_decode_callback(AsPcmDataParam pcm_param)
{
  {
    /* You can process a data here. */
    
    signed short *ptr = (signed short *)pcm_param.mh.getPa();

    for (unsigned int cnt = 0; cnt < pcm_param.size; cnt += 4)
      {
        *ptr = *ptr + 0;
        ptr++;
        *ptr = *ptr + 0;
        ptr++;
      }
  }
  
  theMixer->sendData(OutputMixer0,
                     outmixer_send_callback,
                     pcm_param);
}

void setup()
{
  // Initialize memory pools and message libs
  
  initMemoryPools();
  createStaticPools(MEM_LAYOUT_PLAYER);
  
  // start audio system
  
  thePlayer = MediaPlayer::getInstance();
  theMixer  = OutputMixer::getInstance();

  thePlayer->begin();
  
  puts("initialization Audio Library");

  // Activate Baseband

  theMixer->activateBaseband();

  // Create Objects

  thePlayer->create(MediaPlayer::Player0);

  theMixer->create();

  // Activate Objects

  thePlayer->activate(MediaPlayer::Player0, AS_SETPLAYER_OUTPUTDEVICE_SPHP, mediaplayer_done_callback);

  theMixer->activate(OutputMixer0, outputmixer_done_callback);

  usleep(100 * 1000);


  // Init Player

  thePlayer->init(MediaPlayer::Player0, AS_CODECTYPE_MP3, "/mnt/sd0/BIN", AS_SAMPLINGRATE_AUTO, AS_CHANNEL_STEREO);

  myFile = theSD.open("Sound.mp3");
  printf("Open! %d\n", myFile);

  int err = thePlayer->writeFrames(MediaPlayer::Player0, myFile);

  if (err != MEDIAPLAYER_ECODE_OK)
    {
      printf("File Read Error! =%d\n",err);
    }

  puts("Play!");

  theMixer->setVolume(-160, 0, 0);

  // Start Player

  thePlayer->start(MediaPlayer::Player0, mediaplayer_decode_callback);
}

void loop()
{

  // put your main code here, to run repeatedly:

  puts("loop!!");

  // for examle. Chack Bottom...

  int err = thePlayer->writeFrames(MediaPlayer::Player0, myFile);

  if (err == MEDIAPLAYER_ECODE_FILEEND)
    {
      printf("File End! =%d\n",err);
      sleep(1);
      thePlayer->stop(MediaPlayer::Player0);
      while(1);
    }

  usleep(40000);
}
