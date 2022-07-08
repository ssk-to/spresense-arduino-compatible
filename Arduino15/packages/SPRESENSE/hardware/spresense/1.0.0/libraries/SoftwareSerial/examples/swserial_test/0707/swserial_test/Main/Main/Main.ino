/*
 *  Main.ino - SWSerial 
 *  Copyright 2022 Sony Semiconductor Solutions Corporation
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
#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <MP.h>
#include <MPMutex.h>
#include <LowPower.h>

#define MEMSIZE (128*10) // 1280
//#define MEMSIZE (128*20) // 2560  /* 200000用 */

#define MSG_ID_ADDR   10
#define MSG_ID_ADDROK 11
#define MSG_ID_DATA   20
#define MSG_ID_DATAOK 21

#define BSIZE (MEMSIZE - 4*sizeof(uint16_t))
#define TSIZE (BSIZE/2)

#define SELF_UART_BAUDRATE  115200

//#define _HANDSHAKE_ENA_

int subcore = 1;
#define _PR_ON_ /* Disabled when 200000 bps test */

MPMutex mutex(MP_MUTEX_ID0);

struct __attribute__((packed)) swsdata
{
  uint16_t wptr;
  uint16_t rptr;
  uint16_t over_flow;
  uint16_t resv;
  char data[BSIZE];  
};

static uint8_t *addr;
static struct swsdata *sdata_s;

void setup()
{
  int ret = 0;
  void *raddr;
  int8_t msgid;

  /* Set Serial baudrate. */
//  Serial.begin(SELF_UART_BAUDRATE);
  printf("clkMode:%d \n",LowPower.getClockMode());
        
  /* Allocate Shared Memory */
  addr = (uint8_t *)MP.AllocSharedMemory(MEMSIZE);
  if (!addr) {
    printf("Error: out of memory\n");
    return;
  }
  printf("SharedMemory Address=@%08lx\n", (uint32_t)addr);
  sdata_s = (struct swsdata *)addr;

#if 0
  if(!(ret = mutex.Trylock()))
    {
      /* memory fill */
      memset(addr, 0x00, MEMSIZE);
      sdata_s->wptr = 0;
      sdata_s->rptr = 0;
      sdata_s->over_flow = false;
      mutex.Unlock(); 
    }
  else
    {
      MP.FreeSharedMemory(addr);
      printf("Error at no lock \n");
      while(1);
    }
#endif
    
  /* Boot SubCore */
  MP.begin(subcore);
  
  /* Send shared memory address to SubCore */
  msgid = MSG_ID_ADDR;
  MP.Send(msgid, addr, subcore);

  /* Receive from SubCore */
  while(1){
    ret = MP.Recv(&msgid, &raddr, subcore);
    if(ret == MSG_ID_ADDROK)
      {
        break;
      }
  }
  sleep(1);
}

void loop()
{
  int k;
  uint16_t rsize;
  uint16_t dsize;
  uint16_t overf;
  uint16_t w_ptr;
  uint16_t r_ptr;
  int ret = 0;
  void *raddr;
  int8_t msgid;
  int ptr = 0;
      
  printf("sdata_s:%p\n", sdata_s);
#if 0 /* debug */  
          for(k = 0; k < MEMSIZE; k++)
            {
              printf("%02x ",((char *)sdata_s)[k]);
              if((k+1)%16 == 0) printf("\n");
            }
          printf("wptr:%02x ",sdata_s->wptr);
          printf("rptr:%02x ",sdata_s->rptr);

  while(1);
#endif
          
  while(1)
    {
      ret = MP.Recv(&msgid, &raddr, subcore);
      if(ret == MSG_ID_DATA)
        {
#ifdef _HANDSHAKE_ENA_          
          msgid = MSG_ID_DATAOK;
          MP.Send(msgid, addr, subcore);
#endif  
          dsize = 0;
          if(!(ret = mutex.Trylock()))
            {
              w_ptr = sdata_s->wptr;
              r_ptr = sdata_s->rptr;
              overf = (bool)sdata_s->over_flow;
              mutex.Unlock();
            }
          if( w_ptr >= BSIZE )
            {
                printf("Illegal error !! over buffer area\n");
                /* Free Shared Memory */
//              MP.FreeSharedMemory(addr);
                err_loop();
            }
          if(overf == true)
              {
                printf("overflow occurred!!\n");
                dsize = BSIZE;
                for(k = 0; k < dsize; k++)
                  {
                    if(r_ptr + k >= BSIZE)
                      {
                        ptr = r_ptr + k -BSIZE;
                      }
                    else
                      {
                        ptr = r_ptr + k;
                      }
                    printf("%c",(sdata_s->data)[ptr]); /* output OK */
                  }
                if(!(ret = mutex.Trylock()))
                  {
                    sdata_s->over_flow = (uint16_t)false;
                    if(r_ptr + dsize >= BSIZE) sdata_s->rptr = r_ptr + dsize - BSIZE;
                    else sdata_s->rptr = r_ptr + dsize;
                    mutex.Unlock(); 
                    overf == false; 
                  }
                printf("wptr:%d rptr:%d\n",w_ptr, sdata_s->rptr);
            }
          else
            {
                /* temporarily */
                dsize = w_ptr - r_ptr;                 
                if(dsize > 0)
                  {
#ifdef _PR_ON_
                    printf("dsize:%d > 0\n", dsize);
#endif
//                  Serial.write(&((sdata_s->data)[sdata_s->rptr]), dsize);
#if 0 /*  */
                    if(BSIZE < r_ptr + dsize)
                      {
                        rsize = dsize - BSIZE;
                      }
                    else
                      {
                        rsize = dsize;                
                      }
#endif
                    for(k = 0; k < rsize; k++)
                      {
                        if(r_ptr + k >= BSIZE)
                          {
                            ptr = r_ptr + k -BSIZE;
                          }
                        else
                          {
                            ptr = r_ptr + k;
                          }
                        printf("%c",(sdata_s->data)[ptr]); /* output OK */
//                        if((k+1)%16 == 0) printf("\n"); /* output OK */
                      }
                    if(!(ret = mutex.Trylock()))
                      {
                        if(r_ptr + rsize >= BSIZE) sdata_s->rptr = r_ptr + rsize - BSIZE;
                        else sdata_s->rptr = r_ptr + rsize;
#ifdef _PR_ON_
                        printf("wptr:%d rptr:%d\n",w_ptr, sdata_s->rptr);
#endif
                        mutex.Unlock();
                      }
                  }
                else  /* 2nd area written */
                  {
                    rsize = BSIZE + dsize; /* dsize is a negative value */
#ifdef _PR_ON_
                    printf("rsize:%d\n", rsize);
#endif                    
                    for(k = 0; k < rsize; k++)
                      {
                        if(r_ptr + k >= BSIZE)
                          {
                            ptr = r_ptr + k -BSIZE;
                          }
                        else
                          {
                            ptr = r_ptr + k;
                          }                        
                        printf("%c",(sdata_s->data)[ptr]); /* output OK */
                      }
                    if(!(ret = mutex.Trylock()))
                      {
                        if(r_ptr + rsize >= BSIZE) sdata_s->rptr = r_ptr + rsize - BSIZE;
                        else sdata_s->rptr = r_ptr + rsize;
#ifdef _PR_ON_
                        printf("wptr:%d rptr:%d\n",w_ptr, sdata_s->rptr);
#endif
                        mutex.Unlock();
                      }                  
                  }
            }
         }
    }
}

void err_loop()
{
  while(1){
    sleep(1);
    printf("stopped by illegal error \n");    
  }
}
