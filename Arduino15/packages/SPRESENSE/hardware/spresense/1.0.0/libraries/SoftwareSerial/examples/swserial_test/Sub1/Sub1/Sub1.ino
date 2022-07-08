/*
 *  Sub1.ino - MP Example for MP Shared Memory
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

#if (SUBCORE != 1)
#error "Core selection is wrong!!"
#endif

#include <MP.h>
#include <MPMutex.h>
#include <SoftwareSerial.h>

//#define MEMSIZE (128 * 1024) // 128KB
#define MEMSIZE (128*10) // 1280

//#define SWSERIAL_BAUDRATE  115200
#define SWSERIAL_BAUDRATE  38400

#define MSG_ID_ADDR 10
#define MSG_ID_ADDROK 11
#define MSG_ID_DATA 20

#define BSIZE (MEMSIZE - 4*sizeof(uint16_t))
#define TSIZE (BSIZE/2)

//#define _MP_DEBUG_2_

SoftwareSerial mySerial(3, 9); // RX, TX
/* Create a MPMutex object */
MPMutex mutex(MP_MUTEX_ID0);

struct __attribute__((packed)) swsdata
{
  uint16_t wptr;
  uint16_t rptr;
  uint16_t over_flow;
  uint16_t resv;
  char data[BSIZE];  
};

static void *addr;
static struct swsdata *sdata;
//static bool over_flow;
    
void setup()
{
  int8_t msgid;
  int k = 0;
  int  ret = 0;
 
  addr = NULL;

  mySerial.begin(SWSERIAL_BAUDRATE); // ソフトウェアシリアルの初期化

  MP.begin();
  MP.RecvTimeout(MP_RECV_BLOCKING);
 
  /* Receive address of a shared memory from MainCore */
  while(1){
    ret = MP.Recv(&msgid, &addr);
    if(ret == MSG_ID_ADDR)
      {
        MPLog("MSG_ID_ADDR received\n");
        msgid = MSG_ID_ADDROK;
        if(addr != NULL)
          {
            sdata = (struct swsdata *)addr;
            sdata->wptr = 0;
            sdata->rptr = 0;
            sdata->over_flow = false;
          }
        MP.Send(msgid, addr);      
        MPLog("MSG_ID_ADDROK sent\n");
        break;
      }
  }
}
char tmpb[TSIZE];

void loop()
{
  int cnt;
  int k;
  int ret = 0;
  bool flag;
  int8_t msgid;
    
  cnt = 0;
  k   = 0;
  flag = 0;

#if 0
  if(!(ret = mutex.Trylock()))
    {
      MPLog("wptr:%x\n",(sdata->wptr));
      (sdata->data)[(sdata->wptr)++] = 0x00;
      (sdata->data)[(sdata->wptr)++] = 0x01;
      (sdata->data)[(sdata->wptr)++] = 0x02;
      (sdata->data)[(sdata->wptr)++] = 0x03;
      (sdata->data)[(sdata->wptr)++] = 0x04;
      (sdata->data)[(sdata->wptr)++] = 0x05;
      (sdata->data)[(sdata->wptr)++] = 0x06;
      (sdata->data)[(sdata->wptr)++] = 0x07;        
      (sdata->data)[(sdata->wptr)++] = 0x08;
      (sdata->data)[(sdata->wptr)++] = 0x09;
      (sdata->data)[(sdata->wptr)++] = 0x0A;
      (sdata->data)[(sdata->wptr)++] = 0x0B;
      (sdata->data)[(sdata->wptr)++] = 0x0C;
      (sdata->data)[(sdata->wptr)++] = 0x0D;
      (sdata->data)[(sdata->wptr)++] = 0x0E;   
      (sdata->data)[(sdata->wptr)++] = 0x0F; 
      MPLog("wptr:%x\n",sdata->wptr);
    }
  mutex.Unlock();
  while(1);
#endif  
#if 1  
  /* temporaliry */
  while(cnt < TSIZE){
    while(!mySerial.available());
    tmpb[cnt++] = mySerial.read();
  }
#ifdef _MP_DEBUG_2_
  MPLog("cnt:%d\n",cnt);
#endif
    
  if(!(ret = mutex.Trylock()))
    {
#ifdef _MP_DEBUG_2_
      MPLog("mutex Lock\n");
#endif
      if(sdata->wptr < sdata->rptr) flag = true;

      for(k = 0; k < cnt; k++){
#ifdef _MP_DEBUG_2_
        MPLog("wptr:%d data:%c \n",sdata->wptr, tmpb[k]);
#endif
        (sdata->data)[(sdata->wptr)++] = tmpb[k];
        if(flag == true){
          if(sdata->wptr >= sdata->rptr)
          {
            sdata->over_flow = true;
            MPLog("overflow occurred!!\n");
            break;
          }
        }
        if(sdata->wptr >= BSIZE)
          {
            sdata->wptr = 0;
#ifdef _MP_DEBUG_2_
            MPLog("wptr >= BSIZE; wptr:%d rptr:%d \n",sdata->wptr, sdata->rptr);
#endif
          }
      }
      mutex.Unlock();
#ifdef _MP_DEBUG_2_
      MPLog("mutex Unlock\n");
#endif
      cnt = 0;
      flag = 0;
      
      msgid = MSG_ID_DATA;
      MP.Send(msgid, addr);  /* addr is a dummy */   
#ifdef _MP_DEBUG_2_
      MPLog("MSG_ID_DATA sent\n");
#endif
    }
#endif
}
