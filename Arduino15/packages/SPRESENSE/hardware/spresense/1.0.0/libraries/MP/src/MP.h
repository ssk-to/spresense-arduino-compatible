/*
 *  MP.h - Spresense Arduino Multi-Processer Communication library
 *  Copyright 2019 Sony Semiconductor Solutions Corporation
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

#ifndef _MP_H_
#define _MP_H_

/**
 * @file MP.h
 * @author Sony Semiconductor Solutions Corporation
 * @brief Spresense Arduino Multi-Processer Communication library
 *
 * @details The MP library can manage the Multi-processor communication.
 */

/**
 * @defgroup mp MP Library API
 * @brief Multi-Processor Communication API
 * @{
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <Arduino.h>
#include <sdk/config.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <asmp/asmp.h>
#include <asmp/mptask.h>
#include <asmp/mpshm.h>
#include <asmp/mpmq.h>
#include <asmp/mpmutex.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef BRD_DEBUG
#define MPDBG(format, ...) printf("DEBUG: " format, ##__VA_ARGS__)
#else
#define MPDBG(format, ...)
#endif
#define MPERR(format, ...) printf("ERROR: " format, ##__VA_ARGS__)

#define KEY_SHM   1
#define KEY_MQ    2

#define MP_RECV_BLOCKING    (0)
#define MP_RECV_POLLING     (MPMQ_NONBLOCK)

#define MP_GET_CPUID()      (*(volatile int *)0x4e002040)

#define MP_MAX_SUBID 6

/****************************************************************************
 * class declaration
 ****************************************************************************/

/**
 * @class MPClass
 * @brief This is the interface for MP (Multi-Processor).
 *
 */
class MPClass
{
public:
  MPClass();

  /**
   * @brief Start communication with the other processor
   * @param [in] subid - SubCore number(1~5) launched from MainCore.
   *                     If core is SubCore, call without this argument.
   * @return error code. It returns minus value on failure.
   * @details MainCore boots the specified SubCore. On the other hand, the
   *          launched SubCore notifies MainCore to boot completion by
   *          calling this API.
   */
  int begin(int subid = 0);

  /**
   * @brief End communication with the other processor
   * @param [in] subid - SubCore number(1~5) finalized from MainCore.
   *                     If core is SubCore, this API does nothing.
   * @return error code. It returns minus value on failure.
   * @details MainCore finalizes the specified SubCore.
   */
  int end(int subid = 0);

  /**
   * @brief Send any 32bit-data to the other processor
   * @param [in] msgid - user-defined message ID
   * @param [in] msgdata - user-defined message data (32bit)
   * @param [in] subid - SubCore number(1~5) to send any message.
   *                     If core is SubCore, send to MainCore by default.
   * @return error code. It returns minus value on failure.
   */
  int Send(int8_t msgid, uint32_t msgdata, int subid = 0);

  /**
   * @brief Receive any 32bit-data from the other processor
   * @param [out] msgid - pointer to user-defined message ID
   * @param [out] msgdata - pointer to user-defined message data (32bit)
   * @param [in] subid - SubCore number(1~5) to receive any message.
   *                     If core is SubCore, receive from MainCore by default.
   * @return msgid or error code. It returns minus value on failure.
   */
  int Recv(int8_t *msgid, uint32_t *msgdata, int subid = 0);

  /**
   * @brief Send the address of any message to the other processor
   * @param [in] msgid - user-defined message ID
   * @param [in] msgaddr - pointer to user-defined message address
   * @param [in] subid - SubCore number(1~5) to send any message.
   *                     If core is SubCore, send to MainCore by default.
   * @return error code. It returns minus value on failure.
   */
  int Send(int8_t msgid, void *msgaddr, int subid = 0);

  /**
   * @brief Receive the address of any message from the other processor
   * @param [out] msgid - pointer to user-defined message ID
   * @param [out] msgaddr - pointer to user-defined message address
   * @param [in] subid - SubCore number(1~5) to receive any message.
   *                     If core is SubCore, receive from MainCore by default.
   * @return msgid or error code. It returns minus value on failure.
   */
  int Recv(int8_t *msgid, void *msgaddr, int subid = 0);

  /**
   * @brief Send any object to the other processor
   * @param [in] t - reference to user-defined object
   * @param [in] subid - SubCore number(1~5) to send any message.
   *                     If core is SubCore, send to MainCore by default.
   * @return error code. It returns minus value on failure.
   * @details The size of object must be 128 bytes or less.
   */
  template <typename T> int SendObject(T &t, int subid = 0);

  /**
   * @brief Receive any object from the other processor
   * @param [out] t - reference to user-defined object
   * @param [in] subid - SubCore number(1~5) to receive any message.
   *                     If core is SubCore, receive from MainCore by default.
   * @return msgid or error code. It returns minus value on failure.
   * @details The size of object must be 128 bytes or less.
   */
  template <typename T> int RecvObject(T &t, int subid = 0);

  /**
   * @brief Wait for the object to be sent
   * @param [in] subid - SubCore number(1~5) to send any message.
   *                     If core is SubCore, send to MainCore by default.
   * @return error code. It returns minus value on failure.
   * @details Please call after SendObject().
   */
  int SendWaitComplete(int subid = 0);

  /**
   * @brief Set timeout of receiver
   * @param [in] timeout - waiting time [msec] for reception.
   *             If MP_RECV_BLOCKING is specified, wait forever to receive.
   *             If MP_RECV_POLLING is specified, polling the received data
   *             without blocking.
   */
  void     RecvTimeout(uint32_t timeout);

  /**
   * @brief Get timeout of receiver
   * @return timeout value
   */
  uint32_t GetRecvTimeout();

  /**
   * @brief Convert virtual address to physical address
   * @param [in] virt - virtual address
   * @return physical address
   */
  uint32_t Virt2Phys(void *virt);

#ifndef CONFIG_CXD56_SUBCORE
  /**
   * @brief Allocate memory from shared memory
   * @param [in] size - size. This is align up to 128KByte.
   * @return memory allocated physical address
   */
  void *AllocSharedMemory(size_t size);

  /**
   * @brief Free memory from shared memory
   * @param [in] addr - address allocated by AllocSharedMemory().
   */
  void FreeSharedMemory(void *addr);
#endif

private:
  uint32_t _recvTimeout;
  mpmq_t   _mq[MP_MAX_SUBID];
  struct ResourceManagement {
    uint32_t magic;
    uint32_t cpu_assign;
    uint32_t reserved[2];
    uint32_t resource[4];
  } *_rmng;

  int checkid(int subid);
#ifndef CONFIG_CXD56_SUBCORE
  mptask_t _mptask[MP_MAX_SUBID];
  int load(int subid);
  int unload(int subid);
  sq_queue_t _shmlist;
  struct shm_entry {
    sq_entry_t entry;
    mpshm_t    shm;
    uint32_t   addr;
  };
#endif
};

/****************************************************************************
 * template functions
 ****************************************************************************/

template <typename T> int MPClass::SendObject(T &t, int subid)
{
  int ret;

  if (checkid(subid)) {
    return -EINVAL;
  }

  size_t msgsz = sizeof(T);
  if (msgsz > 127) {
    return -EINVAL;
  }

  ret = Send((int8_t)msgsz, Virt2Phys(&t), subid);
  if (ret < 0) {
    MPDBG("Send(&object) failure. %d\n", ret);
    return ret;
  }

  return ret;
}

template <typename T> int MPClass::RecvObject(T &t, int subid)
{
  int ret;
  void *vp;
  int8_t rsz;

  if (checkid(subid)) {
    return -EINVAL;
  }

  size_t msgsz = sizeof(T);
  if (msgsz > 127) {
    return -EINVAL;
  }

  ret = Recv(&rsz, (uint32_t*)&vp, subid);

  if ((ret <= 0) || (msgsz != ret)) {
    MPDBG("Recv(&object) failure. %d\n", ret);
    mpmq_send(&_mq[subid], -1, 0); // error
    return ret;
  }

  memcpy(&t, vp, msgsz);

  ret = mpmq_send(&_mq[subid], 0, 0); // success
  if (ret < 0) {
    MPDBG("mpmq_send() failure. %d\n", ret);
    return ret;
  }

  return ret;
}

/****************************************************************************
 * extern declaration
 ****************************************************************************/

extern MPClass MP;

/** @} mp */

#endif /* _MP_H_ */