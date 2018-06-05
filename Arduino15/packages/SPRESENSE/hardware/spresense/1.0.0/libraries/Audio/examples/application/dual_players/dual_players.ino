/*
 *  dual_players.ino - Dual source sound player example application
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
#include <Audio.h>


SDClass theSD;
AudioClass *theAudio;

int fd;
File mainFile,subFile;


/***----------------------------------------------***/
void setup()
{

  // start audio system
  theAudio = AudioClass::getInstance();

  puts("initialization Audio Library");
  theAudio->begin();

  theAudio->setPlayerMode(AS_OUT_SP);
  theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3,"/mnt/sd0/BIN",AS_SAMPLINGRATE_AUTO,AS_CHANNEL_STEREO);
  theAudio->initPlayer(AudioClass::Player1, AS_CODECTYPE_MP3,"/mnt/sd0/BIN",AS_SAMPLINGRATE_AUTO,AS_CHANNEL_STEREO);


  mainFile = theSD.open("Sound0.mp3");
  subFile =  theSD.open("Sound1.mp3");

  printf("Open! %d\n",mainFile);
  printf("Open! %d\n",subFile);

  int err = theAudio->writeFrames(AudioClass::Player0,mainFile);
  err = theAudio->writeFrames(AudioClass::Player1,subFile);

  if (err != 0)
    {
      printf("File Read Error! =%d\n",err);
    }

  puts("Play!");

  theAudio->setVolume(-80,-80,-80);

  theAudio->startPlayer(AudioClass::Player0);
  theAudio->startPlayer(AudioClass::Player1);

  puts("start!");

}

/***----------------------------------------------***/
void loop()
{

  // put your main code here, to run repeatedly:

   puts("loop!!");
  // for examle. Chack Bottom...

  int err = theAudio->writeFrames(AudioClass::Player0,mainFile);
  err = theAudio->writeFrames(AudioClass::Player1,subFile);

  if (err == 1)
    {
      printf("File End! =%d\n",err);
      sleep(1);
      theAudio->stopPlayer(AudioClass::Player0);
      theAudio->stopPlayer(AudioClass::Player1);
      while(1);
    }
    
}
