/*
  Audio.h - Audio include file for the Sparduino SDK
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

/**
 * Audio Library for Arduino IDE on SPRITZER.
 *
 * SPRITZER上のArduino 向け Audio Library Classです。
 * こちらのライブラリを使用することで、SPRITZER上での
 *  - 音楽再生
 *  - 音声録音
 *  - 音声通話
 * が可能になります。
 *
 */

#ifndef Audio_h
#define Audio_h

#include <pins_arduino.h>
#include <SDHCI.h>

#ifdef __cplusplus

#include <audio/audio_high_level_api.h>
#include <memutils/simple_fifo/CMN_SimpleFifo.h>

#define WRITE_FIFO_FRAME_NUM  (8)
#define WRITE_FIFO_FRAME_SIZE (2560)
#define WRITE_BUF_SIZE   (WRITE_FIFO_FRAME_NUM * WRITE_FIFO_FRAME_SIZE)

#define READ_FIFO_FRAME_NUM   (10)
#define READ_FIFO_FRAME_SIZE  (3072)
#define READ_BUF_SIZE    (READ_FIFO_FRAME_NUM * READ_FIFO_FRAME_SIZE)

#define FIFO_FRAME_SIZE (\
                          (WRITE_BUF_SIZE > READ_BUF_SIZE) ?\
                          (WRITE_FIFO_FRAME_SIZE) : (READ_FIFO_FRAME_SIZE)\
                        )

#define SIMPLE_FIFO_BUF_SIZE (\
                               ((WRITE_BUF_SIZE) > (READ_BUF_SIZE)) ? \
                               (WRITE_BUF_SIZE) : (READ_BUF_SIZE)\
                             )

extern "C" void  outputDeviceCallback(uint32_t);

/*--------------------------------------------------------------------------*/
/**
 * Audil library log output definition
 */
#define AUDIOLIB_LOG_DEBUG

#define print_err printf

#ifdef AUDIOLIB_LOG_DEBUG
#define print_dbg(...) printf(__VA_ARGS__)
#else
#define print_dbg(x...)
#endif

/*--------------------------------------------------------------------------*/
#define CHUNKID_RIFF        ("RIFF")
#define FORMAT_WAVE         ("WAVE")
#define SUBCHUNKID_FMT      ("fmt ")
#define SUBCHUNKID_DATA     ("data")
#define AUDIO_FORMAT_PCM    (0x0001)
#define FMT_SIZE            (0x10)

#define AS_CODECTYPE_PCM  5

/*--------------------------------------------------------------------------*/
/**
 * Audio Library Error Code Definitions.
 */

#define AUDIOLIB_ECODE_OK                  0
#define AUDIOLIB_ECODE_SHARED_MEMORY_ERROR 1
#define AUDIOLIB_ECODE_SIMPLEFIFO_ERROR    2
#define AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR  3
#define AUDIOLIB_ECODE_FILEACCESS_ERROR    4
#define AUDIOLIB_ECODE_FILEEND             5
#define AUDIOLIB_ECODE_BUFFER_AREA_ERROR   6
#define AUDIOLIB_ECODE_BUFFER_SIZE_ERROR   7
#define AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA   8

/*--------------------------------------------------------------------------*/
/**
 * Audio Library Type Definitions.
 */

typedef unsigned int err_t;

typedef struct
{
  uint8_t   riff[4];    /* "RIFF" */
  uint32_t  total_size;
  uint8_t   wave[4];    /* "WAVE" */
  uint8_t   fmt[4];     /* "fmt " */
  uint32_t  fmt_size;   /* fmt chunk size */
  uint16_t  format;     /* format type */
  uint16_t  channel;
  uint32_t  rate;       /* sampling rate */
  uint32_t  avgbyte;    /* rate * block */
  uint16_t  block;      /* channels * bit / 8 */
  uint16_t  bit;        /* bit length */
  uint8_t   data[4];    /* "data" */
  uint32_t  data_size;
} WavaFormat_t;

/*--------------------------------------------------------------------------*/

/**
 * Audio Library Class Definitions.
 */

class AudioClass
{
public:

  /**
   * Player ID
   *
   * Audio Libraryでは2つのplayerを同時に使用することが出来ます。
   * 使用するplayerを指定する際にはPlayer IDを使用して下さい。
   */

  typedef enum
  {
    Player0,
    Player1
  } PlayerId;

  /**
   * Initialize the audio library and HW modules.
   *
   * この関数は、Audioライブラリを使用する際に、1回だけ呼び出します。
   * 複数回呼び出すと、エラーが発生しますが、"end()" を呼び出した場合、再度、この関数を呼ぶ必要があります。
   *
   * 本関数の中で、必要な共有メモリ管理ライブラリの初期化、タスク間通信ライブラリの初期化、
   * Audio MWの初期化、ES供給用のFIFOの初期化、エラー発生時callback設定のなどを行っていきます。
   *
   */
  err_t begin(void);

  /**
   * Finalization the audio library and HW modules.
   *
   * この関数は、Audioライブラリがbeginを呼ばれて、活性化された際に1回だけ呼び出します。
   * "begin()"を呼ぶ前に呼んでしまったり、複数回呼び出すと、エラーが発生します。
   *
   * 本関数の中で、必要な共有メモリ管理ライブラリの終了処理、タスク間通信ライブラリの終了処理、
   * Audio MWの終了処理、ES供給用のFIFOの破棄、エラー発生時callback設定のクリアのなどを行っていきます。
   *
   */
  err_t end(void);

  /*TODO:外部からは、setModeとして、引数で、Modeを決める方向で検討。*/

  /**
   * Set Audio Library Mode to Music Player.
   *
   * この関数は、Audioライブラリの動作モードをMusic Playerに切り替えるものです。
   * Modeに関しては、別紙の状態遷移表に従います。
   * 状態が、"Music Playerモード"に遷移した後では呼べません。
   * 元の状態に戻す場合は、setReadyMode() を呼んでください。
   *
   * 本関数の中で、音楽再生に必要なHWの設定、ESバッファの設定などを行っていきます。
   *
   */

  /* TODO:AS_OUT_SPの定義する位置。 */
  err_t setPlayerMode(
      uint8_t device  /**< 出力デバイスの指定。AS_SETPLAYER_OUTPUTDEVICE_SPHP or AS_SETPLAYER_OUTPUTDEVICE_I2SOUTPUT。 */
  );

  /**
   * Set Audio Library Mode to Sound Recorder.
   *
   * この関数は、Audioライブラリの動作モードをSound Recorderに切り替えるものです。
   * Modeに関しては、別紙の状態遷移表に従います。
   * 状態が、"Sound Recorderモード"に遷移した後では呼べません。
   * 元の状態に戻す場合は、setReadyMode() を呼んでください。
   *
   * 本関数の中で、音声記録に必要なHWの設定、ESバッファの設定などを行っていきます。
   *
   */
  /* TODO:マクロの名前。長い。INはいらない。 */
  err_t setRecorderMode(
      uint8_t device /**< 入力デバイスの指定。Select from enumulation "AsSetRecorderStsInputDevice"。
      AS_SETRECDR_STS_INPUTDEVICE_MIC_A or AS_SETRECDR_STS_INPUTDEVICE_MIC_D or AS_SETRECDR_STS_INPUTDEVICE_I2S_IN。*/
  );

  /**
   * Set Audio Library Mode to Ready.
   *
   * この関数は、Audioライブラリの動作モードを初期状態に切り替えるものです。
   * Modeに関しては、別紙の状態遷移表に従います。
   * 状態が、"Readyモード"に遷移した後では呼べません。begin直後は、Ready状態になっています。
   *
   * 本関数の中で、各モードで使用したリソースの解放、HWを待機状態への変更するなどを行っていきます。
   *
   */
  err_t setReadyMode(void);

  /**
   * Initialize player.
   *
   * この関数は、Player動作の初期化及び、設定を行います。
   * 音楽再生が行われていなければ、何度でも呼び出せます。
   * この関数によって、
   *  - 圧縮コーデック
   *  - サンプリングレート
   *  - チャンネル数
   * を決めることができます。
   *
   */
  err_t initPlayer(
      PlayerId id,    /**< Player IDの指定 */
      uint8_t codec,  /**< 圧縮Codecの指定。Select from enumulation "AsInitPlayerCodecType”。
                           AS_INITPLAYER_MP3 = 0 or AS_INITPLAYER_WAV or AS_INITPLAYER_AAC or AS_INITPLAYER_OPUS or AS_INITPLAYER_MEDIA */
      uint32_t fs,    /**< サンプリングレートの指定。AsInitPlayerSamplingRateIndexを使用してもよいですし、周波数の値を直接入れてもよいです。*/
      uint8_t channel /**< チャンネル数の指定。AsInitPlayerChannelNumberIndexを使用してもよいですし、チャンネル数を直接入れてもよいです。*/
  );

  /**
   * Initialize recorder
   *
   * この関数は、Recoder動作の初期化及び、設定を行います。
   * 音声記録が行われていなければ、何度でも呼び出せます。
   * この関数によって、
   *  - 圧縮コーデック
   *  - サンプリングレート
   *  - チャンネル数
   * を決めることができます。
   *
   */
  err_t initRecorder(
      uint8_t codec,  /**< 圧縮Codecの指定。Select from enumulation "AsInitRecorderCodecType"。
                           AS_INITREC_MP3 or AS_INITREC_WAV or AS_INITREC_OPUS */
      uint32_t fs,    /**< サンプリングレートの指定。AsInitRecorderSamplingRateIndexを使用してもよいですし、周波数の値を直接入れてもよいです。*/
      uint8_t channel /**< チャンネル数の指定。AsInitRecorderChannelNumberIndexを使用してもよいですし、チャンネル数を直接入れてもよいです。*/
  );

  /**
   * Start Player
   *
   * この関数は、Playerの開始を行います。
   * 一度、この関数を呼ぶとPlayerは、Playing状態になるので、StopPlayerを呼ぶまで呼び出せません。
   *
   * Playを開始すると、Stream Dataの読み出しバッファからAccess Unit分のデータの読み出しを開始します。
   * そのため、Playを開始するためには、事前に、PlayerのバッファにStream Dataを供給した状態にする必要があります。
   * startPlay前に、必ず、writeFramesを呼び出してください。
   * writeFramesの呼び出しを行わずに実行すると、ES_UNDER_FLOW_ERRが発生します。
   *
   */
  err_t startPlayer(
      PlayerId id    /**< Player IDの指定*/
  );

  /**
   * Start Recorder
   *
   * この関数は、Recorderの開始を行います。
   * 一度、この関数を呼ぶとRecodrderは、Recording状態になるので、StopRecorderを呼ぶまで呼び出せません。
   *
   * また、WAVデータでの書き込みの場合、ファイルの先頭でWav Headerを作成する必要があるため、
   * writeWavHeader関数を呼び出す必要があります。
   *
   */
  err_t startRecorder(void);

  /**
   * Stop Player
   *
   * この関数は、Playerの停止を行います。
   * 関数は、startPlayerを呼び出し、Playing状態になったときのみ呼び出すことが可能です。
   * Playを停止すると、Stream Dataの読み出しを停止を開始し、最終フレームまで発音すると停止されます。
   * 完全にAudio出力が停止（100ms程度かかります）されるまで、次のAPIを受け付けません。
   * ※今後仕様変更されます。
   *
   */
  err_t stopPlayer(
      PlayerId id /**< Player IDの指定 */
  );

  /**
   * Stop Recorder
   *
   * この関数は、Recorderの停止を行います。
   * 関数は、startRecorderを呼び出し、Recording状態になったときのみ呼び出すことが可能です。
   * Recorderを停止すると、音声キャプチャの停止を行い、最後にキャプチャした音声データの書き込みまで行います。
   * この関数が戻ってくれば、記録処理は終了していることを保証します。
   *
   */
  err_t stopRecorder(void);

  /**
   * Set Beep Sound
   *
   * この関数は、Beep音の設定を行います。
   * Beep音のOn/Off、音量、音程（周波数）が指定できます。PlayerModeもしくは、ReadyModeで実行できます。
   *
   */
  err_t setBeep(
      char  enable, /**< Beep音のon/off指定。enable(On) = 1, disable(Off) = 0。*/
      short volume, /**< Beep音の音量指定。0?100までの値で指定できる。*/
      short frequency /**< Beep音の周波数（音程）を指定できる。周波数の値をそのまま指定する。(example, 1000)*/
  );

  /**
   * Set Player Volume
   *
   * この関数は、Player再生時の音量を設定できます。
   * PlayerModeで実行できます。
   */
  err_t setVolume(
      int volume /**< マスタ音量の指定。Range of volume is 0 - 100。*/
  );

  err_t setVolume(
      int master,  /**< マスタ音量の指定。Range of volume is 0 - 100。*/
      int player0, /**< Player0音量の指定。Range of volume is 0 - 100。*/
      int player1  /**< Player1音量の指定。Range of volume is 0 - 100。*/
  );

  /**
   * Set Player L/R Gain
   *
   * この関数は、Player再生時のL/R gainを設定できます。
   * PlayerModeで実行できます。
   */
  err_t setLRgain(
      PlayerId id,          /**< Player ID の指定 */
      unsigned char l_gain, /**< L gain */
      unsigned char r_gain  /**< R gain */
  );

  /** APIs for Player Mode */
  /**
   * Write Stream Data from a file to FIFO by some frames.(now 5 frames)
   *
   * この関数は、Fileクラスで指定した音声ファイルから、StreamデータをAudioライブラリ内に持つFIFOに
   * 数フレーム分のデータ分（現在は5フレーム分）、書き込みます。
   * PlayerModeで実行できます。
   *
   * このFIFOは、StopPlayerを呼び出すか、ReadyModeに遷移した際に、クリアされます。
   *
   * 音楽再生中は、定期的にこの関数を呼び出してください。
   * この関数の戻り値が、1のとき、発音すべき音声データがないことを示しますので、StopPlayerを呼び出し、再生を停止してください。
   */
  err_t writeFrames(
      PlayerId id, /**< Player ID の指定 */
      File& myfile /**< 音声ファイルを制御しているFileクラスのインスタンスを指定します。*/
  );


  /** APIs for Recorder Mode */
  /**
   * Write WAV Header.
   *
   * この関数は、ファイルフォーマットがWAVファイルでの音声記録時に呼び出す関数です。
   * InitRecoderのcodecが、wavの時に、必ずStartRecoderの前に呼び出してください。
   * 他のCodecを指定した場合は、呼び出さないでください。
   *
   */
  err_t writeWavHeader(
      File& /**< 音声ファイルを制御しているFileクラスのインスタンスを指定します。*/
  );

  /**
   * Read Stream Data from FIFO to a file by some frames.(now 5 frames)
   *
   * この関数は、Audioライブラリ内に持つFIFOから、生成されたStreamデータをFileクラスで指定したファイルに
   * 数フレーム分のデータ分（現在は5フレーム分）、書き込みます。
   * RecorderModeで実行できます。
   *
   * 音声記録中は、定期的にこの関数を呼び出してください。
   */
  err_t readFrames(File&);


  /**
   * Close Outputfile
   *
   * この関数は、ESを書き込んでいるファイルのclose処理を行う関数です。
   * StopRecorderの呼び出し後、必ず呼び出すようにしてください。
   * RecorderModeで実行できます。
   */
  err_t closeOutputFile(File&);

  /**
   * Audio Object Interfaces
   */

  err_t objIf_createStaticPools(uint8_t layout_no);
  err_t objIf_createMediaPlayer(PlayerId id);
  err_t objIf_createOutputMixer(void);
  err_t objIf_activateMediaPlayer(PlayerId id, uint8_t output_device, MediaPlayerCallback mpcb);
  err_t objIf_activateOutputMixer(AsOutputMixerHandle handle, OutputMixerCallback omcb);
  err_t objIf_initMediaPlayer(PlayerId id, uint8_t codec_type, uint32_t sampling_rate, uint8_t channel_number);
  err_t objIf_startMediaPlayer(PlayerId id, DecodeDoneCallback dccb);
  err_t objIf_stopMediaPlayer(PlayerId id);
  err_t objIf_sendDataOutputMixer(AsOutputMixerHandle handle, MemMgrLite::MemHandle mh, uint32_t sample, uint32_t size, bool is_end, bool is_valid);
  err_t objIf_reqNextMediaPlayerProcess(PlayerId id, AsRequestNextType type);
  err_t objIf_deactMediaPlayer(PlayerId id);
  err_t objIf_deactOutputMixer(void);

  err_t objIf_activateBaseband(void);
  err_t objIf_deactivateBaseband(void);

  /**
   * To get instance of AudioClass
   */
  static AudioClass* getInstance()
    {
      static AudioClass instance;
      return &instance;
    }

  /**
   * Read Stream Data from FIFO to a file by some frames.(now 5 frames)
   *
   * この関数は、Audioライブラリ内に持つFIFOから、生成されたStreamデータを指定したバッファー領域に書き込みます。
   * RecorderModeで実行できます。
   *
   * 音声記録中は、定期的にこの関数を呼び出してください。
   */
  err_t readFrames(
      char*     p_buffer,    /**< バッファー領域のアドレス */
      uint32_t  buffer_size, /**< バッファーのサイズ */
      uint32_t* read_size    /**< 読込みサイズ */
  );

private:

  /**
   * To avoid create multiple instance
   */

  AudioClass() {}
  AudioClass(const AudioClass&);
  AudioClass& operator=(const AudioClass&);
  ~AudioClass() {}

  char m_es_player0_buf[FIFO_FRAME_SIZE];
  char m_es_player1_buf[FIFO_FRAME_SIZE];

  CMN_SimpleFifoHandle m_player0_simple_fifo_handle;
  CMN_SimpleFifoHandle m_player1_simple_fifo_handle;
  uint32_t m_player0_simple_fifo_buf[SIMPLE_FIFO_BUF_SIZE/sizeof(uint32_t)];
  uint32_t m_player1_simple_fifo_buf[SIMPLE_FIFO_BUF_SIZE/sizeof(uint32_t)];

  /* TODO: AsPlayerInputDeviceHdlrForRAM?  RAMを削除、これに付随した、EMMC削除、union削除 */
  AsPlayerInputDeviceHdlrForRAM m_player0_input_device_handler;
  AsPlayerInputDeviceHdlrForRAM m_player1_input_device_handler;

  AsRecorderOutputDeviceHdlr    m_output_device_handler;
  int                           m_es_size;
  WavaFormat_t                  m_wav_format;
  int                           m_codec_type;

  File theFile; /* for  auto file read */

  /* Private Functions */

  /* Functions for initialization on begin/end */
  err_t activateAudio(void);

  err_t initAttention(void);
  err_t powerOn(void);
  err_t powerOff(void);

  /* Functions for initialization Encoder */

  err_t init_recorder_wav(AudioCommand* command, uint32_t sampling_rate, uint8_t channel_number);
  err_t init_recorder_mp3(AudioCommand* command, uint32_t sampling_rate, uint8_t channel_number);
  err_t init_recorder_opus(AudioCommand* command, uint32_t sampling_rate, uint8_t channel_number);
  err_t init_recorder_pcm(AudioCommand* command, uint32_t sampling_rate, uint8_t channel_number);

  /* Functions for initialization on player mode. */
  err_t set_output(int);

  err_t write_fifo(int, char*, CMN_SimpleFifoHandle*);
  err_t write_fifo(File&, char*, CMN_SimpleFifoHandle*);

  /* Functions for initialization on recorder mode. */
  err_t init_mic_gain(int, int);
};

extern AudioClass Audio;

#endif //__cplusplus
#endif //Audio_h

