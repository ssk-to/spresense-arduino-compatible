/* This file is generated automatically. */
/****************************************************************************
 * pool_layout.h
 *
 *   Copyright 2019 Sony Semiconductor Solutions Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Sony Semiconductor Solutions Corporation nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef POOL_LAYOUT_H_INCLUDED
#define POOL_LAYOUT_H_INCLUDED

#include "memutils/memory_manager/MemMgrTypes.h"

namespace MemMgrLite {

MemPool*  static_pools_block[NUM_MEM_SECTIONS][NUM_MEM_POOLS];
MemPool** static_pools[NUM_MEM_SECTIONS] = {
  static_pools_block[0],
  static_pools_block[1],
};
uint8_t layout_no[NUM_MEM_SECTIONS] = {
  BadLayoutNo,
  BadLayoutNo,
};
uint8_t pool_num[NUM_MEM_SECTIONS] = {
  NUM_MEM_S0_POOLS,
  NUM_MEM_S1_POOLS,
};
extern const PoolSectionAttr MemoryPoolLayouts[NUM_MEM_SECTIONS][NUM_MEM_LAYOUTS][17] = {
  {  /* Section:0 */
    {/* Layout:0 */
     /* pool_ID          type       seg fence  addr        size         */
      { S0_DEC_ES_MAIN_BUF_POOL        , BasicType,   4, true, 0x000c0008, 0x00006000 },  /* COMMON_WORK_AREA */
      { S0_REND_PCM_BUF_POOL           , BasicType,   5, true, 0x000c6010, 0x00015f90 },  /* COMMON_WORK_AREA */
      { S0_SRC_WORK_MAIN_BUF_POOL      , BasicType,   1, true, 0x000dbfa8, 0x00002000 },  /* COMMON_WORK_AREA */
      { S0_DEC_ES_SUB_BUF_POOL         , BasicType,   4, true, 0x000ddfb0, 0x00006000 },  /* COMMON_WORK_AREA */
      { S0_REND_PCM_SUB_BUF_POOL       , BasicType,   5, true, 0x000e3fb8, 0x0000a028 },  /* COMMON_WORK_AREA */
      { S0_SRC_WORK_SUB_BUF_POOL       , BasicType,   1, true, 0x000edfe8, 0x00002000 },  /* COMMON_WORK_AREA */
      { S0_DEC_APU_CMD_POOL            , BasicType,  10, true, 0x000efff0, 0x00000398 },  /* COMMON_WORK_AREA */
      { S0_PF0_PCM_BUF_POOL            , BasicType,   1, true, 0x000f0390, 0x00004650 },  /* COMMON_WORK_AREA */
      { S0_PF1_PCM_BUF_POOL            , BasicType,   1, true, 0x000f49e8, 0x00004650 },  /* COMMON_WORK_AREA */
      { S0_PF0_APU_CMD_POOL            , BasicType,  10, true, 0x000f9040, 0x00000398 },  /* COMMON_WORK_AREA */
      { S0_PF1_APU_CMD_POOL            , BasicType,  10, true, 0x000f93e0, 0x00000398 },  /* COMMON_WORK_AREA */
      { S0_NULL_POOL, 0, 0, false, 0, 0 },
    },
    {/* Layout:1 */
     /* pool_ID          type       seg fence  addr        size         */
      { S0_OUTPUT_BUF_POOL             , BasicType,   5, true, 0x000c0008, 0x0000f000 },  /* COMMON_WORK_AREA */
      { S0_MIC_IN_BUF_POOL             , BasicType,   5, true, 0x000cf010, 0x00014000 },  /* COMMON_WORK_AREA */
      { S0_ENC_APU_CMD_POOL            , BasicType,   3, true, 0x000e3018, 0x00000114 },  /* COMMON_WORK_AREA */
      { S0_SRC_APU_CMD_POOL            , BasicType,   3, true, 0x000e3138, 0x00000114 },  /* COMMON_WORK_AREA */
      { S0_PRE_APU_CMD_POOL            , BasicType,   3, true, 0x000e3258, 0x00000114 },  /* COMMON_WORK_AREA */
      { S0_NULL_POOL, 0, 0, false, 0, 0 },
    },
    {/* Layout:2 */
     /* pool_ID          type       seg fence  addr        size         */
      { S0_MIC_IN_BUF_POOL             , BasicType,   5, true, 0x000c0008, 0x00000960 },  /* COMMON_WORK_AREA */
      { S0_I2S_IN_BUF_POOL             , BasicType,   5, true, 0x000c0970, 0x000012c0 },  /* COMMON_WORK_AREA */
      { S0_HP_OUT_BUF_POOL             , BasicType,   5, true, 0x000c1c38, 0x000012c0 },  /* COMMON_WORK_AREA */
      { S0_I2S_OUT_BUF_POOL            , BasicType,   5, true, 0x000c2f00, 0x000012c0 },  /* COMMON_WORK_AREA */
      { S0_MFE_OUT_BUF_POOL            , BasicType,   8, true, 0x000c41c8, 0x00000500 },  /* COMMON_WORK_AREA */
      { S0_NULL_POOL, 0, 0, false, 0, 0 },
    },
    {/* Layout:3 */
     /* pool_ID          type       seg fence  addr        size         */
      { S0_DEC_ES_MAIN_BUF_POOL        , BasicType,   4, true, 0x000c0008, 0x00006000 },  /* COMMON_WORK_AREA */
      { S0_REND_PCM_BUF_POOL           , BasicType,   5, true, 0x000c6010, 0x00005014 },  /* COMMON_WORK_AREA */
      { S0_SRC_WORK_MAIN_BUF_POOL      , BasicType,   1, true, 0x000cb030, 0x00001000 },  /* COMMON_WORK_AREA */
      { S0_DEC_ES_SUB_BUF_POOL         , BasicType,   4, true, 0x000cc038, 0x00004000 },  /* COMMON_WORK_AREA */
      { S0_REND_PCM_SUB_BUF_POOL       , BasicType,   5, true, 0x000d0040, 0x00005014 },  /* COMMON_WORK_AREA */
      { S0_SRC_WORK_SUB_BUF_POOL       , BasicType,   1, true, 0x000d5060, 0x00001000 },  /* COMMON_WORK_AREA */
      { S0_DEC_APU_CMD_POOL            , BasicType,  10, true, 0x000d6068, 0x00000398 },  /* COMMON_WORK_AREA */
      { S0_PF0_PCM_BUF_POOL            , BasicType,   1, true, 0x000d6408, 0x00001004 },  /* COMMON_WORK_AREA */
      { S0_PF1_PCM_BUF_POOL            , BasicType,   1, true, 0x000d7418, 0x00001004 },  /* COMMON_WORK_AREA */
      { S0_PF0_APU_CMD_POOL            , BasicType,  10, true, 0x000d8428, 0x00000398 },  /* COMMON_WORK_AREA */
      { S0_PF1_APU_CMD_POOL            , BasicType,  10, true, 0x000d87c8, 0x00000398 },  /* COMMON_WORK_AREA */
      { S0_OUTPUT_BUF_POOL             , BasicType,   2, true, 0x000d8b68, 0x00001800 },  /* COMMON_WORK_AREA */
      { S0_MIC_IN_BUF_POOL             , BasicType,   5, true, 0x000da370, 0x00003c00 },  /* COMMON_WORK_AREA */
      { S0_ENC_APU_CMD_POOL            , BasicType,   3, true, 0x000ddf78, 0x00000114 },  /* COMMON_WORK_AREA */
      { S0_SRC_APU_CMD_POOL            , BasicType,   3, true, 0x000de098, 0x00000114 },  /* COMMON_WORK_AREA */
      { S0_PRE_APU_CMD_POOL            , BasicType,   3, true, 0x000de1b8, 0x00000114 },  /* COMMON_WORK_AREA */
      { S0_NULL_POOL, 0, 0, false, 0, 0 },
    },
  },
  {  /* Section:1 */
    {/* Layout:0 */
     /* pool_ID          type       seg fence  addr        size         */
      { S1_SENSOR_DSP_CMD_BUF_POOL     , BasicType,   8, false, 0x000f9780, 0x00001800 },  /* COMMON_WORK_AREA */
      { S1_SENSOR_DATA_BUF_POOL        , BasicType,   8, false, 0x000faf80, 0x00001800 },  /* COMMON_WORK_AREA */
      { S1_NULL_POOL, 0, 0, false, 0, 0 },
    },
  },
}; /* end of MemoryPoolLayouts */

}  /* end of namespace MemMgrLite */

#endif /* POOL_LAYOUT_H_INCLUDED */
