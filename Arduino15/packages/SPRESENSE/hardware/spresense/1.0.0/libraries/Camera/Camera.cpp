/*
  Camera.cpp - SPI implement file for the SPRESENSE SDK
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
#include <arch/board/board.h>

#include <sdk/config.h>

#include <sys/ioctl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <asmp/mpshm.h>

#include "Camera.h"

//***************************************************************************
// Definitions
//***************************************************************************

#define KEY_SHM (0)

static uint32_t capture_size;
static sem_t sem;
// Configuration ************************************************************
// C++ initialization requires CXX initializer support
extern "C" {
static void complete_capture(uint8_t code, uint8_t last_frame,
                             uint32_t size, uint32_t addr)
{
  /* Save capture image information */
  printf("complete_capture.code %d\n", code);
  printf("capture size %d\n", size);

  capture_size = size;

  sem_post(&sem);
}
}
/****************************************************************************
 * Common API on Camera Class
 ****************************************************************************/
err_t CameraClass::begin(void)
{
	
  err_t ret=0;

  fd = open("/dev/imager0", 0);
  if (fd < 0)
    {
      printf("failed to open imager driver : %d:%d\n", fd, errno);
      return 1;
    }
  ret = ioctl(fd, IMGIOC_SETSTATE, STATE_ISX012_ACTIVE);
  if (ret < 0)
    {
      printf("IMGIOC_SETSTATE failed. %d\n", ret);
    }

  cis_param.format                = FORMAT_CISIF_JPEG;
  cis_param.yuv_param.hsize       = 0;
  cis_param.yuv_param.vsize       = 0;
  cis_param.yuv_param.notify_size = 0;
  cis_param.yuv_param.notify_func = NULL;
  cis_param.yuv_param.comp_func   = NULL;
  cis_param.jpg_param.notify_size = 0;
  cis_param.jpg_param.notify_func = NULL;
  cis_param.jpg_param.comp_func   = complete_capture;

  sem_init(&sem, 0, 0);

  /* Get MP shared memory for JPEG capture */

  ret = mpshm_init(&shm, KEY_SHM, 896*1024);
  if (ret < 0)
    {
      printf("mpshm_init() failure. %d\n", ret);
      return ret;
    }

  uint8_t *vbuffer = (uint8_t *)mpshm_attach(&shm, 0);
  if (!vbuffer)
    {
      printf("mpshm_attach() failure.\n");
      return ret;
    }

  buffer = (uint8_t *)mpshm_virt2phys(&shm, (void *)vbuffer);

  return ret;
}

/*--------------------------------------------------------------------------*/
err_t CameraClass::end(void)
{
  err_t ret = 0;

  /* Finalize MP shared memory */

  mpshm_detach(&shm);
  mpshm_destroy(&shm);
  sem_destroy(&sem);

  return ret;
}

/*--------------------------------------------------------------------------*/
err_t CameraClass::initialize(cisif_param_t param)
{
  err_t ret = cxd56_cisifinit(&param);
  if (ret != OK)
    {
      printf("camera_main: cxd56_cisifinit failed: %d\n", ret);
    }

  return ret;

}

/*--------------------------------------------------------------------------*/
err_t CameraClass::initialize()
{
  err_t ret = cxd56_cisifinit(&cis_param);
  if (ret != OK)
    {
      printf("camera_main: cxd56_cisifinit failed: %d\n", ret);
    }
  return ret;

}

/*--------------------------------------------------------------------------*/
err_t CameraClass::read(uint8_t** addr, int* size)
{
  cis_area.strg_addr = buffer;
  cis_area.strg_size = *size;

   err_t ret = cxd56_cisifcaptureframe(NULL, &cis_area);
   if (ret != OK) 
	{
		printf("camera_main: cxd56_cisifcaptureframe failed: %d\n", ret);
	}

  sem_wait(&sem);

  *addr = buffer;
  *size = capture_size;

  return ret;

}

/*--------------------------------------------------------------------------*/
err_t CameraClass::read(File& myFile)
{
  cis_area.strg_addr = buffer;
  cis_area.strg_size = 1920*1200;

   err_t ret = cxd56_cisifcaptureframe(NULL, &cis_area);
   if (ret != OK) 
	{
		printf("camera_main: cxd56_cisifcaptureframe failed: %d\n", ret);
	}

  sem_wait(&sem);

  /* Save to SD card */

  myFile.write(buffer, capture_size);
  myFile.close();

  return ret;

}
