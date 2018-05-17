
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
  theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3,AS_SAMPLINGRATE_48000,AS_CHANNEL_STEREO); 
  theAudio->initPlayer(AudioClass::Player1, AS_CODECTYPE_MP3,AS_SAMPLINGRATE_48000,AS_CHANNEL_STEREO); 


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
