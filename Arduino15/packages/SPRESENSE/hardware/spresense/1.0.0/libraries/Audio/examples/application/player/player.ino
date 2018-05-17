#include <SDHCI.h>

#include <Audio.h>
#include <fcntl.h>

SDClass theSD;

AudioClass *theAudio;

int fd;
File myFile;


void setup()
{
  // start audio system
  theAudio = AudioClass::getInstance();

  theAudio->begin();

  puts("initialization Audio Library");

  theAudio->setPlayerMode(AS_OUT_SP);
  theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3,AS_SAMPLINGRATE_48000,AS_CHANNEL_STEREO);

  myFile = theSD.open("Sound.mp3");
  printf("Open! %d\n",myFile);

  int err = theAudio->writeFrames(AudioClass::Player0, myFile);

  if (err != AUDIOLIB_ECODE_OK)
    {
      printf("File Read Error! =%d\n",err);
    }

  puts("Play!");

  theAudio->setVolume(-160, 0, 0);
  theAudio->startPlayer(AudioClass::Player0);
}

void loop()
{

  // put your main code here, to run repeatedly:

  puts("loop!!");

  // for examle. Chack Bottom...

  int err = theAudio->writeFrames(AudioClass::Player0, myFile);

  if (err == AUDIOLIB_ECODE_FILEEND)
    {
      printf("File End! =%d\n",err);
      sleep(1);
      theAudio->stopPlayer(AudioClass::Player0);
      while(1);
    }

  usleep(40000);
}
