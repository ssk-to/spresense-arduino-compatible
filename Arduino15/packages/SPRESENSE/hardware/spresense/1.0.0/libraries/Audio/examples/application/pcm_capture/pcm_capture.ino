#include <SPI.h>

#include <Audio.h>
//#include <fcntl.h>
#include <arch/board/board.h>


AudioClass *theAudio;

const int32_t recoding_frames = 400;
const int32_t buffer_size = 6144;
char buffer[buffer_size];

void setup()
{
  // put your setup code here, to run once:

  theAudio = AudioClass::getInstance();

  theAudio->begin();

  puts("initialization Audio Library");

  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC_A);
  theAudio->initRecorder(AS_CODECTYPE_PCM,AS_SAMPLINGRATE_48000,AS_CHANNEL_4CH); 
  puts("Init Recorder!");

  puts("Rec!");
  theAudio->startRecorder();
}

void loop() {
  // put your main code here, to run repeatedly:

  // for Example, Chack Bottom

  static int cnt = 0;
  uint32_t read_size;

  if (cnt>recoding_frames)
    {
      puts("End Recording");
      theAudio->stopRecorder();
      theAudio->readFrames(buffer, buffer_size, &read_size);
      exit(1);
    }

  int err = theAudio->readFrames(buffer, buffer_size, &read_size);

  if (err != AUDIOLIB_ECODE_OK && err != AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA)
    {
      printf("Error End! =%d\n",err);
      sleep(1);
      theAudio->stopRecorder();
      exit(1);
    }

  // ここで適当にBufferエリアのデータに加工・解析などを行ってください。
  if (read_size != 0)
  {
    printf("Record Data: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
  }
  volatile int i;
  for(i=0; i<100000;i++);
  // ここまで。適当にBufferエリアのデータに加工・解析などを行ってください。

  cnt++;

}



