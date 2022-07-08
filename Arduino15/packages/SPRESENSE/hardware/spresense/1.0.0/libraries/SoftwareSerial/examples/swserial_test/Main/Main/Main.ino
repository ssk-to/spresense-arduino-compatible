/*
 *  Main.ino - MP Example for MP Shared Memory
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

#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <MP.h>
#include <MPMutex.h>

//#define MEMSIZE (128 * 1024) // 128KB
#define MEMSIZE (128*10) // 1280
#define MSG_ID_ADDR 10
#define MSG_ID_ADDROK 11
#define MSG_ID_DATA 20

#define BSIZE (MEMSIZE - 4*sizeof(uint16_t))

int subcore = 1;

struct __attribute__((packed)) swsdata
{
  uint16_t wptr;
  uint16_t rptr;
  uint16_t over_flow;
  uint16_t resv;
  char data[BSIZE]; 
};

static void *addr;
static struct swsdata *sdata_s;

/* Create a MPMutex object */
MPMutex mutex(MP_MUTEX_ID0);

void setup()
{
  int k = 0;
  int ret = 0;
  
  Serial.begin(115200);
  while (!Serial);
  Serial.println("### UART1 start");
    
  /* Boot SubCore */
  MP.begin(subcore);

  /* Allocate Shared Memory */
  uint8_t *addr = (uint8_t *)MP.AllocSharedMemory(MEMSIZE);
  if (!addr) {
    printf("Error: out of memory\n");
    return;
  }
  sdata_s = (struct swsdata *) addr;

  printf("SharedMemory Address=@%08lx\n", (uint32_t)addr);

  /* memory fill */
  memset(addr, 0x00, MEMSIZE);

#if 0
  for(k = 0; k < MEMSIZE; k++)
  {
    printf(" %02x",addr[k]);
    if((k+1)%16 == 0) printf("\n");
  }
#endif
  /* Send shared memory address to SubCore */
  int8_t msgid = MSG_ID_ADDR;
  MP.Send(msgid, addr, subcore);

  /* Receive from SubCore */
  void *raddr;
  while(1){
    ret = MP.Recv(&msgid, &raddr, subcore);
    if(ret == MSG_ID_ADDROK)
      {
#if 0 /* debug  */
      if(!(ret = mutex.Trylock()))
        {
          for(k = 0; k < MEMSIZE; k++)
            {
              printf("%02x ",((char *)addr)[k]);
              if((k+1)%16 == 0) printf("\n");
            }
        }
      mutex.Unlock();
#endif
      break;
      }
  }
  printf("subcore ready!\n");
  usleep(1000); /* wait */
}

int tmp[BSIZE];

void loop()
{
  int32_t dsize = 0;
  int ret = 0;
  int16_t k = 0;
  int32_t rsize = 0;
  int8_t msgid;
  void *raddr;
  uint16_t w_ptr = 0;
  uint16_t r_ptr = 0;
  bool overf = false;
    
  printf("sdata_s:%p\n", sdata_s);

#if 0 /* debug */  
          for(k = 0; k < MEMSIZE; k++)
            {
              printf("%02x ",((char *)sdata_s)[k]);
              if((k+1)%16 == 0) printf("\n");
            }
          printf("wptr:%02x ",sdata_s->wptr);
          printf("rptr:%02x ",sdata_s->rptr);
#endif
          
  while(1)
    {
      ret = MP.Recv(&msgid, &raddr, subcore);
      if(ret == MSG_ID_DATA)
        {
          dsize = 0;
          if(!(ret = mutex.Trylock()))
            {
              w_ptr = sdata_s->wptr;
              r_ptr = sdata_s->rptr;
              overf = (bool)sdata_s->over_flow;
              mutex.Unlock();          
            if( w_ptr > BSIZE )
              {
                printf("Illegal error !! over buffer area\n");
                /* Free Shared Memory */
//              MP.FreeSharedMemory(addr);
//              exit(0);
              }
            if(overf == true)
              {
                printf("overflow occurred!!\n");
                dsize = BSIZE;
                for(k = 0; k < dsize; k++)
                  printf("%c",(sdata_s->data)[r_ptr+k]); /* output OK */
                if(!(ret = mutex.Trylock()))
                  {
                    sdata_s->over_flow = (uint16_t)false;
                    sdata_s->rptr = w_ptr;
                    mutex.Unlock();  
                  }
                printf("wptr:%d rptr:%d\n",w_ptr, sdata_s->rptr);
              }
            else
              {
                /* temporarily */
                dsize = w_ptr - r_ptr;                 
                if(dsize > 0)
                  {
                    printf("dsize:%d > 0\n", dsize);
//                  Serial.write(&((sdata_s->data)[sdata_s->rptr]), dsize);
                    if(BSIZE < r_ptr + dsize)
                      {
                        rsize = dsize - BSIZE;
                      }
                    else
                      {
                        rsize = dsize;                
                      }
                    for(k = 0; k < rsize; k++)
                      {
                        printf("%c",(sdata_s->data)[r_ptr + k]); /* output OK */
//                        if((k+1)%16 == 0) printf("\n"); /* output OK */
                      }
                    if(!(ret = mutex.Trylock()))
                      {
                        sdata_s->rptr = r_ptr + rsize;
                        if(sdata_s->rptr >= BSIZE) sdata_s->rptr = 0;
                        printf("wptr:%d rptr:%d\n",w_ptr, sdata_s->rptr);
                        mutex.Unlock();
                      }
                  }
                else  /* 2nd area written */
                  {
                    rsize = BSIZE + dsize; /* dsize is a negative value */
                    printf("rsize:%d\n", rsize);
                    for(k = 0; k < rsize; k++)
                      {
                        printf("%c",(sdata_s->data)[r_ptr + k]); /* output OK */
                      }
                    if(!(ret = mutex.Trylock()))
                      {
                        sdata_s->rptr = r_ptr + rsize;
                      }
                    if(sdata_s->rptr >= BSIZE) sdata_s->rptr = 0;
                    printf("wptr:%d rptr:%d\n",sdata_s->wptr, sdata_s->rptr);

                    if(!(ret = mutex.Trylock()))
                      {
                        sdata_s->rptr = r_ptr + rsize;
                        if(sdata_s->rptr >= BSIZE) sdata_s->rptr = 0;
                        printf("wptr:%d rptr:%d\n",w_ptr, sdata_s->rptr);
                        mutex.Unlock();
                      }                  
                  }
              }
           }
          mutex.Unlock();      
      }
    }
}
