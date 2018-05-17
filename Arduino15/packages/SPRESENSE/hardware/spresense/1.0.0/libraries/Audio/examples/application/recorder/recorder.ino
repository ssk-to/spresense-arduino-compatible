#include <SDHCI.h>

#include <Audio.h>
//#include <fcntl.h>
#include <arch/board/board.h>


SDClass theSD;
AudioClass *theAudio;

File myFile;



void setup()
{
  theAudio = AudioClass::getInstance();

  theAudio->begin();

  puts("initialization Audio Library");

  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC_A);
  theAudio->initRecorder(AS_CODECTYPE_MP3,AS_SAMPLINGRATE_48000,AS_CHANNEL_STEREO); 
  puts("Init Recorder!");

  myFile = theSD.open("Sound.mp3", FILE_WRITE);
  puts("Open!");

  puts("Rec!");

  theAudio->startRecorder();
}

void loop() {
  // put your main code here, to run repeatedly:

  // for Example, Chack Bottom

  static int cnt = 0;

  if (cnt>400)
    {
      puts("End Recording");
      theAudio->stopRecorder();
      theAudio->closeOutputFile(myFile);
      exit(1);
    }

  int err = theAudio->readFrames(myFile);

  if (err != AUDIOLIB_ECODE_OK)
    {
      printf("File End! =%d\n",err);
      sleep(1);
      theAudio->stopRecorder();
      theAudio->closeOutputFile(myFile);
      exit(1);
    }

  usleep(1);

  cnt++;
}



