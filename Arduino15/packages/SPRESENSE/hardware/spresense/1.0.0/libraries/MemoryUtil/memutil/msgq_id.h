/* This file is generated automatically. */
/****************************************************************************
 * msgq_id.h
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

#ifndef MSGQ_ID_H_INCLUDED
#define MSGQ_ID_H_INCLUDED

/* Message area size: 9908 bytes */

#define MSGQ_TOP_DRM 0xfd000
#define MSGQ_END_DRM 0xff6b4

/* Message area fill value after message poped */

#define MSG_FILL_VALUE_AFTER_POP 0x0

/* Message parameter type match check */

#define MSG_PARAM_TYPE_MATCH_CHECK false

/* Message queue pool IDs */

#define MSGQ_NULL 0
#define MSGQ_AUD_MGR 1
#define MSGQ_AUD_APP 2
#define MSGQ_AUD_DSP 3
#define MSGQ_AUD_SUB_DSP 4
#define MSGQ_AUD_PLY 5
#define MSGQ_AUD_PFDSP0 6
#define MSGQ_AUD_PFDSP1 7
#define MSGQ_AUD_SUB_PLY 8
#define MSGQ_AUD_OUTPUT_MIX 9
#define MSGQ_AUD_RND_PLY 10
#define MSGQ_AUD_RND_PLY_SYNC 11
#define MSGQ_AUD_RND_SUB 12
#define MSGQ_AUD_RND_SUB_SYNC 13
#define MSGQ_AUD_RECORDER 14
#define MSGQ_AUD_CAP 15
#define MSGQ_AUD_CAP_SYNC 16
#define MSGQ_AUD_SOUND_EFFECT 17
#define MSGQ_AUD_RCG_CMD 18
#define MSGQ_AUD_CAP_MIC 19
#define MSGQ_AUD_CAP_MIC_SYNC 20
#define MSGQ_AUD_CAP_I2S 21
#define MSGQ_AUD_CAP_I2S_SYNC 22
#define MSGQ_AUD_RND_SPHP 23
#define MSGQ_AUD_RND_SPHP_SYNC 24
#define MSGQ_AUD_RND_I2S 25
#define MSGQ_AUD_RND_I2S_SYNC 26
#define MSGQ_AUD_FRONTEND 27
#define MSGQ_AUD_PREDSP 28
#define MSGQ_SEN_MGR 29
#define NUM_MSGQ_POOLS 30

/* User defined constants */

/************************************************************************/
#define MSGQ_AUD_MGR_QUE_BLOCK_DRM 0xfd044
#define MSGQ_AUD_MGR_N_QUE_DRM 0xfd7f8
#define MSGQ_AUD_MGR_N_SIZE 88
#define MSGQ_AUD_MGR_N_NUM 10
#define MSGQ_AUD_MGR_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_MGR_H_SIZE 0
#define MSGQ_AUD_MGR_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_APP_QUE_BLOCK_DRM 0xfd088
#define MSGQ_AUD_APP_N_QUE_DRM 0xfdb68
#define MSGQ_AUD_APP_N_SIZE 40
#define MSGQ_AUD_APP_N_NUM 2
#define MSGQ_AUD_APP_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_APP_H_SIZE 0
#define MSGQ_AUD_APP_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_DSP_QUE_BLOCK_DRM 0xfd0cc
#define MSGQ_AUD_DSP_N_QUE_DRM 0xfdbb8
#define MSGQ_AUD_DSP_N_SIZE 20
#define MSGQ_AUD_DSP_N_NUM 5
#define MSGQ_AUD_DSP_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_DSP_H_SIZE 0
#define MSGQ_AUD_DSP_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_SUB_DSP_QUE_BLOCK_DRM 0xfd110
#define MSGQ_AUD_SUB_DSP_N_QUE_DRM 0xfdc1c
#define MSGQ_AUD_SUB_DSP_N_SIZE 20
#define MSGQ_AUD_SUB_DSP_N_NUM 5
#define MSGQ_AUD_SUB_DSP_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_SUB_DSP_H_SIZE 0
#define MSGQ_AUD_SUB_DSP_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_PLY_QUE_BLOCK_DRM 0xfd154
#define MSGQ_AUD_PLY_N_QUE_DRM 0xfdc80
#define MSGQ_AUD_PLY_N_SIZE 48
#define MSGQ_AUD_PLY_N_NUM 5
#define MSGQ_AUD_PLY_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_PLY_H_SIZE 0
#define MSGQ_AUD_PLY_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_PFDSP0_QUE_BLOCK_DRM 0xfd198
#define MSGQ_AUD_PFDSP0_N_QUE_DRM 0xfdd70
#define MSGQ_AUD_PFDSP0_N_SIZE 20
#define MSGQ_AUD_PFDSP0_N_NUM 5
#define MSGQ_AUD_PFDSP0_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_PFDSP0_H_SIZE 0
#define MSGQ_AUD_PFDSP0_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_PFDSP1_QUE_BLOCK_DRM 0xfd1dc
#define MSGQ_AUD_PFDSP1_N_QUE_DRM 0xfddd4
#define MSGQ_AUD_PFDSP1_N_SIZE 20
#define MSGQ_AUD_PFDSP1_N_NUM 5
#define MSGQ_AUD_PFDSP1_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_PFDSP1_H_SIZE 0
#define MSGQ_AUD_PFDSP1_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_SUB_PLY_QUE_BLOCK_DRM 0xfd220
#define MSGQ_AUD_SUB_PLY_N_QUE_DRM 0xfde38
#define MSGQ_AUD_SUB_PLY_N_SIZE 48
#define MSGQ_AUD_SUB_PLY_N_NUM 5
#define MSGQ_AUD_SUB_PLY_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_SUB_PLY_H_SIZE 0
#define MSGQ_AUD_SUB_PLY_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_OUTPUT_MIX_QUE_BLOCK_DRM 0xfd264
#define MSGQ_AUD_OUTPUT_MIX_N_QUE_DRM 0xfdf28
#define MSGQ_AUD_OUTPUT_MIX_N_SIZE 48
#define MSGQ_AUD_OUTPUT_MIX_N_NUM 8
#define MSGQ_AUD_OUTPUT_MIX_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_OUTPUT_MIX_H_SIZE 0
#define MSGQ_AUD_OUTPUT_MIX_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RND_PLY_QUE_BLOCK_DRM 0xfd2a8
#define MSGQ_AUD_RND_PLY_N_QUE_DRM 0xfe0a8
#define MSGQ_AUD_RND_PLY_N_SIZE 32
#define MSGQ_AUD_RND_PLY_N_NUM 16
#define MSGQ_AUD_RND_PLY_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RND_PLY_H_SIZE 0
#define MSGQ_AUD_RND_PLY_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RND_PLY_SYNC_QUE_BLOCK_DRM 0xfd2ec
#define MSGQ_AUD_RND_PLY_SYNC_N_QUE_DRM 0xfe2a8
#define MSGQ_AUD_RND_PLY_SYNC_N_SIZE 16
#define MSGQ_AUD_RND_PLY_SYNC_N_NUM 2
#define MSGQ_AUD_RND_PLY_SYNC_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RND_PLY_SYNC_H_SIZE 0
#define MSGQ_AUD_RND_PLY_SYNC_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RND_SUB_QUE_BLOCK_DRM 0xfd330
#define MSGQ_AUD_RND_SUB_N_QUE_DRM 0xfe2c8
#define MSGQ_AUD_RND_SUB_N_SIZE 32
#define MSGQ_AUD_RND_SUB_N_NUM 16
#define MSGQ_AUD_RND_SUB_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RND_SUB_H_SIZE 0
#define MSGQ_AUD_RND_SUB_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RND_SUB_SYNC_QUE_BLOCK_DRM 0xfd374
#define MSGQ_AUD_RND_SUB_SYNC_N_QUE_DRM 0xfe4c8
#define MSGQ_AUD_RND_SUB_SYNC_N_SIZE 16
#define MSGQ_AUD_RND_SUB_SYNC_N_NUM 2
#define MSGQ_AUD_RND_SUB_SYNC_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RND_SUB_SYNC_H_SIZE 0
#define MSGQ_AUD_RND_SUB_SYNC_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RECORDER_QUE_BLOCK_DRM 0xfd3b8
#define MSGQ_AUD_RECORDER_N_QUE_DRM 0xfe4e8
#define MSGQ_AUD_RECORDER_N_SIZE 48
#define MSGQ_AUD_RECORDER_N_NUM 5
#define MSGQ_AUD_RECORDER_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RECORDER_H_SIZE 0
#define MSGQ_AUD_RECORDER_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_CAP_QUE_BLOCK_DRM 0xfd3fc
#define MSGQ_AUD_CAP_N_QUE_DRM 0xfe5d8
#define MSGQ_AUD_CAP_N_SIZE 24
#define MSGQ_AUD_CAP_N_NUM 16
#define MSGQ_AUD_CAP_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_CAP_H_SIZE 0
#define MSGQ_AUD_CAP_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_CAP_SYNC_QUE_BLOCK_DRM 0xfd440
#define MSGQ_AUD_CAP_SYNC_N_QUE_DRM 0xfe758
#define MSGQ_AUD_CAP_SYNC_N_SIZE 16
#define MSGQ_AUD_CAP_SYNC_N_NUM 2
#define MSGQ_AUD_CAP_SYNC_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_CAP_SYNC_H_SIZE 0
#define MSGQ_AUD_CAP_SYNC_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_SOUND_EFFECT_QUE_BLOCK_DRM 0xfd484
#define MSGQ_AUD_SOUND_EFFECT_N_QUE_DRM 0xfe778
#define MSGQ_AUD_SOUND_EFFECT_N_SIZE 52
#define MSGQ_AUD_SOUND_EFFECT_N_NUM 5
#define MSGQ_AUD_SOUND_EFFECT_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_SOUND_EFFECT_H_SIZE 0
#define MSGQ_AUD_SOUND_EFFECT_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RCG_CMD_QUE_BLOCK_DRM 0xfd4c8
#define MSGQ_AUD_RCG_CMD_N_QUE_DRM 0xfe87c
#define MSGQ_AUD_RCG_CMD_N_SIZE 20
#define MSGQ_AUD_RCG_CMD_N_NUM 5
#define MSGQ_AUD_RCG_CMD_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RCG_CMD_H_SIZE 0
#define MSGQ_AUD_RCG_CMD_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_CAP_MIC_QUE_BLOCK_DRM 0xfd50c
#define MSGQ_AUD_CAP_MIC_N_QUE_DRM 0xfe8e0
#define MSGQ_AUD_CAP_MIC_N_SIZE 24
#define MSGQ_AUD_CAP_MIC_N_NUM 16
#define MSGQ_AUD_CAP_MIC_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_CAP_MIC_H_SIZE 0
#define MSGQ_AUD_CAP_MIC_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_CAP_MIC_SYNC_QUE_BLOCK_DRM 0xfd550
#define MSGQ_AUD_CAP_MIC_SYNC_N_QUE_DRM 0xfea60
#define MSGQ_AUD_CAP_MIC_SYNC_N_SIZE 16
#define MSGQ_AUD_CAP_MIC_SYNC_N_NUM 2
#define MSGQ_AUD_CAP_MIC_SYNC_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_CAP_MIC_SYNC_H_SIZE 0
#define MSGQ_AUD_CAP_MIC_SYNC_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_CAP_I2S_QUE_BLOCK_DRM 0xfd594
#define MSGQ_AUD_CAP_I2S_N_QUE_DRM 0xfea80
#define MSGQ_AUD_CAP_I2S_N_SIZE 24
#define MSGQ_AUD_CAP_I2S_N_NUM 16
#define MSGQ_AUD_CAP_I2S_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_CAP_I2S_H_SIZE 0
#define MSGQ_AUD_CAP_I2S_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_CAP_I2S_SYNC_QUE_BLOCK_DRM 0xfd5d8
#define MSGQ_AUD_CAP_I2S_SYNC_N_QUE_DRM 0xfec00
#define MSGQ_AUD_CAP_I2S_SYNC_N_SIZE 16
#define MSGQ_AUD_CAP_I2S_SYNC_N_NUM 2
#define MSGQ_AUD_CAP_I2S_SYNC_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_CAP_I2S_SYNC_H_SIZE 0
#define MSGQ_AUD_CAP_I2S_SYNC_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RND_SPHP_QUE_BLOCK_DRM 0xfd61c
#define MSGQ_AUD_RND_SPHP_N_QUE_DRM 0xfec20
#define MSGQ_AUD_RND_SPHP_N_SIZE 32
#define MSGQ_AUD_RND_SPHP_N_NUM 16
#define MSGQ_AUD_RND_SPHP_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RND_SPHP_H_SIZE 0
#define MSGQ_AUD_RND_SPHP_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RND_SPHP_SYNC_QUE_BLOCK_DRM 0xfd660
#define MSGQ_AUD_RND_SPHP_SYNC_N_QUE_DRM 0xfee20
#define MSGQ_AUD_RND_SPHP_SYNC_N_SIZE 16
#define MSGQ_AUD_RND_SPHP_SYNC_N_NUM 2
#define MSGQ_AUD_RND_SPHP_SYNC_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RND_SPHP_SYNC_H_SIZE 0
#define MSGQ_AUD_RND_SPHP_SYNC_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RND_I2S_QUE_BLOCK_DRM 0xfd6a4
#define MSGQ_AUD_RND_I2S_N_QUE_DRM 0xfee40
#define MSGQ_AUD_RND_I2S_N_SIZE 32
#define MSGQ_AUD_RND_I2S_N_NUM 16
#define MSGQ_AUD_RND_I2S_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RND_I2S_H_SIZE 0
#define MSGQ_AUD_RND_I2S_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_RND_I2S_SYNC_QUE_BLOCK_DRM 0xfd6e8
#define MSGQ_AUD_RND_I2S_SYNC_N_QUE_DRM 0xff040
#define MSGQ_AUD_RND_I2S_SYNC_N_SIZE 16
#define MSGQ_AUD_RND_I2S_SYNC_N_NUM 2
#define MSGQ_AUD_RND_I2S_SYNC_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_RND_I2S_SYNC_H_SIZE 0
#define MSGQ_AUD_RND_I2S_SYNC_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_FRONTEND_QUE_BLOCK_DRM 0xfd72c
#define MSGQ_AUD_FRONTEND_N_QUE_DRM 0xff060
#define MSGQ_AUD_FRONTEND_N_SIZE 48
#define MSGQ_AUD_FRONTEND_N_NUM 5
#define MSGQ_AUD_FRONTEND_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_FRONTEND_H_SIZE 0
#define MSGQ_AUD_FRONTEND_H_NUM 0
/************************************************************************/
#define MSGQ_AUD_PREDSP_QUE_BLOCK_DRM 0xfd770
#define MSGQ_AUD_PREDSP_N_QUE_DRM 0xff150
#define MSGQ_AUD_PREDSP_N_SIZE 20
#define MSGQ_AUD_PREDSP_N_NUM 5
#define MSGQ_AUD_PREDSP_H_QUE_DRM 0xffffffff
#define MSGQ_AUD_PREDSP_H_SIZE 0
#define MSGQ_AUD_PREDSP_H_NUM 0
/************************************************************************/
#define MSGQ_SEN_MGR_QUE_BLOCK_DRM 0xfd7b4
#define MSGQ_SEN_MGR_N_QUE_DRM 0xff1b4
#define MSGQ_SEN_MGR_N_SIZE 80
#define MSGQ_SEN_MGR_N_NUM 16
#define MSGQ_SEN_MGR_H_QUE_DRM 0xffffffff
#define MSGQ_SEN_MGR_H_SIZE 0
#define MSGQ_SEN_MGR_H_NUM 0

#endif /* MSGQ_ID_H_INCLUDED */
