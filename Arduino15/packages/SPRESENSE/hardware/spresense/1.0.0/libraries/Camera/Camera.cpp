/*
 *  Camera.cpp - Camera implementation file for the Spresense SDK
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

/**
 * @file Camera.cpp
 * @author Sony Semiconductor Solutions Corporation
 * @brief Camera Library for Arduino IDE on Spresense.
 * @details Camera Classes for SPRESENSE Camera library.
 */

#include <string.h>
#include <fcntl.h>
#include <sched.h>

#include <Camera.h>

//==================================================================================
// ImgBuff implementation.
ImgBuff::ImgBuff()
  : ref_count(0), buff(NULL), width(0), height(0), idx(-1), buf_type(V4L2_BUF_TYPE_VIDEO_CAPTURE), cam_ref(NULL)
{
}

ImgBuff::ImgBuff(enum v4l2_buf_type type, int w, int h, CAM_IMAGE_PIX_FMT fmt, CameraClass *cam)
  : ref_count(0), buff(NULL), width(0), height(0), idx(-1), buf_type(type), pix_fmt(CAM_IMAGE_PIX_FMT_NONE), buf_size(0), actual_size(0), cam_ref(NULL)
{
  buf_size = calc_img_size(w, h, fmt);
  if( buf_size >= 1 && generate_imgmem(buf_size) ){
    sem_init(&my_sem, 0, 1);
    cam_ref = cam;
    width = w;
    height = h;
  }
}

ImgBuff::~ImgBuff()
{
  if( buff != NULL ){
    free(buff);
    sem_destroy(&my_sem);
    buff = NULL;
  }
}

bool ImgBuff::generate_imgmem(size_t s)
{
  if( s > 0 ){

    // SPRESENSE Camera CamImage memory is needed 4 byte alignment.
    buff = (uint8_t *)memalign(SPRESENSE_CAMIMAGE_MEM_ALIGN, s);

    if( buff != NULL ) return true;
  }

  return false;
}

void ImgBuff::incRef()
{
  lock();
  ref_count++;
  unlock();
}

bool ImgBuff::decRef()
{
  bool ret = false;
  lock();
  ref_count--;
  if(ref_count <= 0) ret = true;
  unlock();
  return ret;
}

void ImgBuff::delete_inst(ImgBuff *buf)
{
  if( buf != NULL ){
    if( buf->decRef() ){
      if( buf->cam_ref != NULL ){
        buf->cam_ref->release_buf(buf);
      }else{
        delete buf;
      }
    }
  }
}

size_t ImgBuff::calc_img_size(int w, int h, CAM_IMAGE_PIX_FMT fmt)
{
  size_t ret = -1;

  if( w >= 1 && h >= 1 ){
    switch(fmt){
      case CAM_IMAGE_PIX_FMT_RGB565:
      case CAM_IMAGE_PIX_FMT_YUV422:
        ret = w * h * 2;
        break;
      case CAM_IMAGE_PIX_FMT_JPG:
        // JPG file's size is not fixed.
        // Max size is uncomplessed size.
        ret = w * h * 2;
        break;
      default:
        break;
    }
  }

  return ret;
}

void ImgBuff::update_actual_size(size_t sz)
{
  if( sz >= buf_size ) sz = buf_size;
  actual_size = buf_size;
}



//=====================================================================
// CamImage implementation.

CamImage::CamImage(const CamImage &obj)
{
  img_buff = obj.img_buff;
  img_buff->incRef();
}

CamImage &CamImage::operator=(const CamImage &obj)
{
  ImgBuff::delete_inst(img_buff);

  img_buff = obj.img_buff;
  img_buff->incRef();

  return (*this);
}

CamErr CamImage::convertPixFormat(CAM_IMAGE_PIX_FMT fmt)
{
  // TODO
  return CAM_ERR_SUCCESS;
}


CamImage::CamImage(enum v4l2_buf_type type, int w, int h, CAM_IMAGE_PIX_FMT fmt, CameraClass *cam)
{
  img_buff = new ImgBuff(type, w, h, fmt, cam);

  if( !img_buff->is_valid() ){
    delete img_buff;
    img_buff = NULL;
  }
}

CamImage::~CamImage()
{
  ImgBuff::delete_inst(img_buff);
}


//==================================================================================
// CameraClass implementation.

#define VIDEO_DEV_FILE_NAME "/dev/video"

//===============================================================
// Private : Camera singleton instance.
CameraClass *CameraClass::instance = NULL;


//===============================================================
// Public : singleton getter of Camera instance.
CameraClass CameraClass::getInstance()
{
  if( CameraClass::instance == NULL )
    CameraClass::instance = new CameraClass(VIDEO_DEV_FILE_NAME);
  return *CameraClass::instance;
}


#define STILL_STATUS_NO_INIT (0)
#define STILL_STATUS_QUEUED (1)
#define STILL_STATUS_TAKING (2)

//===============================================================
// Public : Constructor.
CameraClass::CameraClass(const char *path)
{
  video_init_stat = video_initialize(path);
  video_fd = -1;
  video_imgs = NULL;
  video_buf_num = 0;
  still_img = NULL;
  loop_dqbuf_en = false;
  video_cb = NULL;
  still_cb = NULL;
  dq_tid = -1;
  still_status = STILL_STATUS_NO_INIT;
  sem_init(&video_cb_access_sem, 0, 1);
  sem_init(&still_cb_access_sem, 0, 1);
}


//===============================================================
// Public : Destructor.
CameraClass::~CameraClass()
{
  video_uninitialize();
  CameraClass::instance = NULL;
}


//===============================================================
// Private : Validate video frame parameters.
bool CameraClass::check_video_fmtparam(int w, int h, CAM_VIDEO_FPS fps, CAM_IMAGE_PIX_FMT fmt)
{
  if( ((CAM_IMGSIZE_QVGA_H    == w) && (CAM_IMGSIZE_QVGA_V    == h)) ||
      ((CAM_IMGSIZE_VGA_H     == w) && (CAM_IMGSIZE_VGA_V     == h)) ||
      ((CAM_IMGSIZE_QUADVGA_H == w) && (CAM_IMGSIZE_QUADVGA_V == h)) ||
      ((CAM_IMGSIZE_HD_H      == w) && (CAM_IMGSIZE_HD_V      == h)) ||
      ((CAM_IMGSIZE_FULLHD_H  == w) && (CAM_IMGSIZE_FULLHD_V  == h)) ||
      ((CAM_IMGSIZE_5M_H      == w) && (CAM_IMGSIZE_5M_V      == h)) ||
      ((CAM_IMGSIZE_3M_H      == w) && (CAM_IMGSIZE_3M_V      == h)) ){
    return true;
  }
  return false;
}


//===============================================================
// Private : Set V4S frame paramters.
CamErr CameraClass::set_frame_parameters( enum v4l2_buf_type type, int video_width, int video_height, int buf_num, CAM_IMAGE_PIX_FMT video_fmt )
{
  struct v4l2_requestbuffers req = {0};
  struct v4l2_format fmt         = {0};

  // Set Buffer Mode.
  req.type = type;
  req.memory = V4L2_MEMORY_USERPTR;
  req.count = buf_num;
  req.mode = V4L2_BUF_MODE_RING; /*  Freeze as ring mode. */
  if( ioctl(video_fd, VIDIOC_REQBUFS, (unsigned long)&req) < 0 ){
    return CAM_ERR_ILLIGAL_DEVERR;
  }

  // Set Format.
  fmt.type                = type;
  fmt.fmt.pix.width       = video_width;
  fmt.fmt.pix.height      = video_height;
  fmt.fmt.pix.field       = V4L2_FIELD_ANY;
  fmt.fmt.pix.pixelformat = video_fmt;
  if( ioctl(video_fd, VIDIOC_S_FMT, (unsigned long)&fmt) < 0 ){
    return CAM_ERR_ILLIGAL_DEVERR;
  }
  
  return CAM_ERR_SUCCESS;
}


//===============================================================
// Private : Create Video frame buffers.
CamErr CameraClass::create_videobuff(int w, int h, int buff_num, CAM_IMAGE_PIX_FMT fmt)
{
  int i;

  video_imgs = (CamImage **)malloc(sizeof(CamImage *) * buff_num);
  if( video_imgs == NULL )
    return CAM_ERR_NO_MEMORY;
  
  for(i=0; i<buff_num; i++){
    video_imgs[i] = new CamImage(V4L2_BUF_TYPE_VIDEO_CAPTURE, w, h, fmt, this);
    if( video_imgs[i] == NULL ){
      while(i>0){
        i--;
        delete video_imgs[i];
      }
      delete video_imgs;
      return CAM_ERR_NO_MEMORY;
    }
    video_imgs[i]->setIdx(i);
  }

  video_buf_num = buff_num;

  return CAM_ERR_SUCCESS;
}

//===============================================================
// Private : Create Still picture buffers.
CamErr CameraClass::create_stillbuff(int w, int h, CAM_IMAGE_PIX_FMT fmt)
{
  if(still_img != NULL){
    // TODO How to dequeue the enqueued buffer.
    delete still_img;
  }

  still_img = new CamImage(V4L2_BUF_TYPE_STILL_CAPTURE, w, h, fmt, this);

  if( still_img == NULL ){
    return CAM_ERR_NO_MEMORY;
  }

  still_img->setIdx(0);

  return CAM_ERR_SUCCESS;
}



//===============================================================
// Private : Delete Video buffers.
void CameraClass::delete_videobuff()
{
  if( video_imgs ){
    for(int i=0; i<video_buf_num; i++){
      if( video_imgs[i] )
        delete video_imgs[i];
    }
    delete video_imgs;
  }

  video_imgs = NULL;
  video_buf_num = 0;
}


//===============================================================
// Private : Enqueue All Video buffes.
CamErr CameraClass::enqueue_video_buffs()
{
  CamErr err = CAM_ERR_SUCCESS;

  for (int i = 0; i < video_buf_num; i++) {
    err = enqueue_video_buff(video_imgs[i]);
    if( err != CAM_ERR_SUCCESS )
      break;
  }

  return err;
}


//===============================================================
// Private : Enqueue One Video buffe.
CamErr CameraClass::enqueue_video_buff(CamImage * img)
{
  v4l2_buffer_t buf;

  memset(&buf, 0, sizeof(v4l2_buffer_t));
  buf.type = img->getType();
  buf.memory = V4L2_MEMORY_USERPTR;
  buf.index = img->getIdx();
  buf.m.userptr = (unsigned long)img->getImgBuff();
  buf.length = img->img_buff->buf_size;

  if( ioctl(video_fd, VIDIOC_QBUF, (unsigned long)&buf) < 0 ){
    // No dequeue successed buffers.
    // because devie file "close" will clean up everthing.
    // TODO
    return CAM_ERR_ILLIGAL_DEVERR;
  }

  return CAM_ERR_SUCCESS;
}


//===============================================================
// Private : Check if device is ready to use.
bool CameraClass::is_device_ready()
{
  return (video_init_stat == 0) && (video_fd >= 0);
}


//===============================================================
// Conversion table to parameter of time par frame of S_PARM
static struct fps_to_timeparframe {
  CAM_VIDEO_FPS fps;
  uint32_t n;
  uint32_t d; 
} fps2tpf[] = {
  {CAM_VIDEO_FPS_5,   1, 5},
  {CAM_VIDEO_FPS_6,   1, 6},
  {CAM_VIDEO_FPS_7_5, 2, 15},
  {CAM_VIDEO_FPS_15,  1, 15},
  {CAM_VIDEO_FPS_30,  1, 30},
  {CAM_VIDEO_FPS_60,  1, 60},
  {CAM_VIDEO_FPS_120, 1, 120},
};
#define SIZE_OF_TPF (sizeof(fps2tpf)/sizeof(fps2tpf[0]))


//===============================================================
// Private : Video frame rate setting.
CamErr CameraClass::set_video_frame_rate(CAM_VIDEO_FPS fps)
{
  struct v4l2_streamparm param = {0};
  struct fps_to_timeparframe *tpf = NULL;

  for(unsigned int i=0; i<SIZE_OF_TPF; i++){
    if( fps2tpf[i].fps == fps ){
      tpf = &fps2tpf[i];
    }
  }

  if( tpf == NULL ){
    return CAM_ERR_INVALID_PARAM;
  }

  // memset(&param, 0, sizeof(struct v4l2_streamparm));

  param.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  param.parm.capture.timeperframe.numerator = tpf->n;
  param.parm.capture.timeperframe.denominator = tpf->d;

  if( ioctl(video_fd, VIDIOC_S_PARM, (unsigned long)&param) < 0 ){
    return CAM_ERR_ILLIGAL_DEVERR;
  }

  return CAM_ERR_SUCCESS;
}

CamErr CameraClass::create_dq_thread()
{
  CamErr err = CAM_ERR_SUCCESS;

  struct sched_param param;

  pthread_attr_init(&dq_tattr);
  dq_tattr.stacksize = CAM_DQ_THREAD_STACK_SIZE;
  param.sched_priority = CAM_DQ_THREAD_STACK_PRIO;
  pthread_attr_setschedparam(&dq_tattr, &param);

  loop_dqbuf_en = true;
  if( pthread_create(
        &dq_tid,
        &dq_tattr,
        (pthread_startroutine_t)CameraClass::frame_handle_thread,
        (void *)this) ){

    loop_dqbuf_en = false;
    err = CAM_ERR_CANT_CREATE_THREAD;
  }

  return err;
}


//===============================================================
// Public : Start to use the Camera.
CamErr CameraClass::begin( int video_width, int video_height, CAM_VIDEO_FPS fps, int buff_num, CAM_IMAGE_PIX_FMT video_fmt )
{
  CamErr ret = CAM_ERR_SUCCESS;

  if( !check_video_fmtparam(video_width, video_height, fps, video_fmt) ){
    return CAM_ERR_INVALID_PARAM;
  }

  if( video_fmt != CAM_IMAGE_PIX_FMT_YUV422 ){ // NOTE:: Only YUV422 is supported on this version.
    return CAM_ERR_INVALID_PARAM;
  }

  if( buff_num <= 0 ){
    return CAM_ERR_INVALID_PARAM;
  }

  if( video_init_stat ){
    return CAM_ERR_NO_DEVICE;
  }

  if( video_fd >= 0 ){
    return CAM_ERR_ALREADY_INITIALIZED;
  }

  video_fd = open(VIDEO_DEV_FILE_NAME, 0);
  if( video_fd < 0 ){
    return CAM_ERR_NO_DEVICE;
  }

  // Start Dequeue Buff thread.
  ret = create_dq_thread();
  if( ret != CAM_ERR_SUCCESS ){
    goto label_err_no_memaligned;
  }

  // Set Video Frame parameters.
  ret = set_frame_parameters(V4L2_BUF_TYPE_VIDEO_CAPTURE, video_width, video_height, buff_num, video_fmt);
  if( ret != CAM_ERR_SUCCESS ){
    goto label_err_no_memaligned;
  }

  // Set Video Frame Rate.
  ret= set_video_frame_rate(fps);
  if( ret != CAM_ERR_SUCCESS ){
    goto label_err_no_memaligned;
  }

  // Create Buffer
  ret = create_videobuff(video_width, video_height, buff_num, video_fmt);
  if( ret != CAM_ERR_SUCCESS ){
    goto label_err_no_memaligned;
  }

  // Set Buffer into V4S
  ret = enqueue_video_buffs();
  if( ret != CAM_ERR_SUCCESS ){
    goto label_err_with_memaligned;
  }
  
  ret = set_video_frame_rate(fps);
  if( ret != CAM_ERR_SUCCESS ){
    goto label_err_with_memaligned;
  }

  return ret; // Success begin.

  label_err_with_memaligned:
  delete_videobuff();

  label_err_no_memaligned:
  close(video_fd);
  video_fd = -1;

  return ret;
}


//===============================================================
// Public : Start video streaming.
CamErr CameraClass::startStreaming( bool enable, camera_cb_t cb )
{
  camera_cb_t old_cb;
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  CamErr err = CAM_ERR_SUCCESS;
  unsigned long req = enable ? VIDIOC_STREAMON : VIDIOC_STREAMOFF;
  
  if( is_device_ready() ){
    lock_video_cb();
    old_cb = video_cb;
    video_cb = cb;
    unlock_video_cb();

    if( ioctl(video_fd, req, (unsigned long)&type) < 0 ){
      err =  CAM_ERR_ILLIGAL_DEVERR;
      lock_video_cb();
      video_cb = old_cb;
      unlock_video_cb();
    }
  }else{
    err = CAM_ERR_NOT_INITIALIZED;
  }

  return err;
}


//===============================================================
// Private : Set EXT_CTRLS of V4S.
CamErr CameraClass::set_ext_ctrls(uint16_t ctl_cls, uint16_t cid, int32_t value)
{
  struct v4l2_ext_controls param = {0};
  struct v4l2_ext_control ctl_param = {0};

  if( is_device_ready() ){
    ctl_param.id = cid;
    ctl_param.value = value;

    param.ctrl_class = ctl_cls;
    param.count = 1;
    param.controls = &ctl_param;

    if( ioctl(video_fd, VIDIOC_S_EXT_CTRLS, (unsigned long)&param) < 0 ){
      return CAM_ERR_NO_DEVICE;
    }
    return CAM_ERR_SUCCESS;

  }else{
    return CAM_ERR_NOT_INITIALIZED;
  }
}


//===============================================================
// Public : Turn on/off Auto White Balance.
CamErr CameraClass::setAutoWhiteBalance(bool enable)
{
    return set_ext_ctrls(
              V4L2_CTRL_CLASS_USER,
              V4L2_CID_AUTO_WHITE_BALANCE,
              enable ? 1 : 0 );
}


//===============================================================
// Public : Turn on/off Auto Exposure.
CamErr CameraClass::setAutoExposure(bool enable)
{
    return set_ext_ctrls(
              V4L2_CTRL_CLASS_CAMERA,
              V4L2_CID_EXPOSURE_AUTO,
              enable ? V4L2_EXPOSURE_AUTO : V4L2_EXPOSURE_MANUAL );
}


//===============================================================
// Public : Turn on/off Auto ISO Sensitivity.
CamErr CameraClass::setAutoISOSensitive(bool enable)
{
    return set_ext_ctrls(
              V4L2_CTRL_CLASS_CAMERA,
              V4L2_CID_ISO_SENSITIVITY_AUTO,
              enable ? V4L2_ISO_SENSITIVITY_AUTO : V4L2_ISO_SENSITIVITY_MANUAL );
}


//===============================================================
// Public : Set ISO Sensitivity value in manual.
CamErr CameraClass::setISOSensitivity(int iso_sense )
{
    return set_ext_ctrls(
              V4L2_CTRL_CLASS_CAMERA,
              V4L2_CID_ISO_SENSITIVITY,
              (uint32_t)iso_sense );
}


//===============================================================
// Public : Auto White Balance Mode.
CamErr CameraClass::setAutoWhiteBalanceMode(CAM_WHITE_BALANCE wb)
{
    return set_ext_ctrls(
              V4L2_CTRL_CLASS_CAMERA,
              V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE,
              (uint32_t)wb);
}

//===============================================================
// Public : Scene Mode.
CamErr CameraClass::setSceneMode(CAM_SCENE_MODE mode )
{
    return set_ext_ctrls(
              V4L2_CTRL_CLASS_CAMERA,
              V4L2_CID_SCENE_MODE,
              (uint32_t)mode );
}

//===============================================================
// Public : Color Effect
CamErr CameraClass::setColorEffect(CAM_COLOR_FX effect  )
{
    return set_ext_ctrls(
              V4L2_CTRL_CLASS_USER,
              V4L2_CID_COLORFX,
              (uint32_t)effect );
}

int CameraClass::get_still_status()
{
  int ret;

  lock_still_cb();
  ret = still_status;
  unlock_still_cb();

  return ret;
}

void CameraClass::set_still_status(int status)
{
  lock_still_cb();
  still_status = status;
  unlock_still_cb();
}


//===============================================================
// Public : Still Picture Format.
CamErr CameraClass::setStillPictiureImageFormat( int img_width, int img_height, CAM_IMAGE_PIX_FMT img_fmt )
{
  CamErr err = CAM_ERR_SUCCESS;

  if( is_device_ready() && get_still_status() != STILL_STATUS_TAKING ){
    if( check_video_fmtparam( img_width, img_height, CAM_VIDEO_FPS_NONE, img_fmt ) ){

      err = set_frame_parameters(V4L2_BUF_TYPE_STILL_CAPTURE, img_width, img_height, 1, img_fmt);
      if( err == CAM_ERR_SUCCESS ){

        err = create_stillbuff(img_width, img_height, img_fmt);
        if( err == CAM_ERR_SUCCESS ){

          err = enqueue_video_buff(still_img);

          set_still_status(STILL_STATUS_QUEUED);
        }
      }
    }else{
      err = CAM_ERR_INVALID_PARAM;
    }
  }else{
    err = CAM_ERR_NOT_INITIALIZED;
  }

  return err;
}

//===============================================================
// Public : Take a Picture.
CamErr CameraClass::takePicture( camera_cb_t cb )
{
  CamErr err = CAM_ERR_SUCCESS;
  camera_cb_t old_cb;

  long unsigned int take_num = 1;
  if( is_device_ready() && get_still_status() == STILL_STATUS_QUEUED ){
    lock_still_cb();
    old_cb = still_cb;
    still_cb = cb;
    unlock_still_cb();

    if( ioctl(video_fd, VIDIOC_TAKEPICT_START, take_num) < 0 ){
      err = CAM_ERR_ILLIGAL_DEVERR;

      lock_still_cb();
      still_cb = old_cb;
      unlock_still_cb();
    }else{
      set_still_status(STILL_STATUS_TAKING);
    }

  }else{
    err = CAM_ERR_NOT_INITIALIZED;
  }

  return err;
}


//===============================================================
// Public : Finish to use the Camera.
void CameraClass::end()
{
  if( is_device_ready() ){
  }
}

int CameraClass::ioctl_dequeue_stream_buf( struct v4l2_buffer *buf)
{
  return ioctl(video_fd, VIDIOC_DQBUF, (unsigned long)buf);
}

CamImage *CameraClass::search_vimg(int index)
{
  CamImage *img = NULL;

  for(int i=0; i<video_buf_num; i++){
    if( video_imgs[i]->isIdx(index) ){
      img = video_imgs[i];
      break;
    }
  }

  return img;
}

//===============================================================
// Private Static : Frame buffer handling thread.
void CameraClass::frame_handle_thread(void *arg)
{
  struct v4l2_buffer buf;
  CameraClass *cam = (CameraClass *)arg;

  while(cam->loop_dqbuf_en){
    // TODO poll cam->video_fd to wait Still and Video captured image.
    // And wait to finish define I/F to find which stream is available.

    memset(&buf, 0, sizeof(v4l2_buffer_t));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // TODO
    buf.memory = V4L2_MEMORY_USERPTR;

    if( cam->ioctl_dequeue_stream_buf(&buf) == 0 ){

      cam->lock_video_cb();

      if( cam->video_cb != NULL ){
        CamImage *img = cam->search_vimg(buf.index);
        if(img != NULL){
          img->setActualSize((size_t)buf.bytesused);
          cam->video_cb(*img);
        }
      }

      cam->unlock_video_cb();
    }
    usleep(1); /* Sleep for task dispatch */
  }

}

//===============================================================
// Private Static : 
void CameraClass::release_buf(ImgBuff *buf)
{
  for(int i=0; i<video_buf_num; i++){
    if(video_imgs[i]->isIdx(buf->idx)){
      enqueue_video_buff(video_imgs[i]);
      break;
    }
  }
}


/** Global instance */
CameraClass theCamera = CameraClass::getInstance();

