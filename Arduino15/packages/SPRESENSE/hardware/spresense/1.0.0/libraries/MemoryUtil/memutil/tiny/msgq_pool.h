/* This file is generated automatically. */
/****************************************************************************
 * msgq_pool.h
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

#ifndef MSGQ_POOL_H_INCLUDED
#define MSGQ_POOL_H_INCLUDED

#include "msgq_id.h"

extern const MsgQueDef MsgqPoolDefs[NUM_MSGQ_POOLS] = {
   /* n_drm, n_size, n_num, h_drm, h_size, h_num */
  { 0x00000000, 0, 0, 0x00000000, 0, 0, 0 }, /* MSGQ_NULL */
  { 0xfd8b4, 88, 10, 0xffffffff, 0, 0 }, /* MSGQ_AUD_MGR */
  { 0xfdc24, 40, 2, 0xffffffff, 0, 0 }, /* MSGQ_AUD_APP */
  { 0xfdc74, 20, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_DSP */
  { 0xfdcd8, 48, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_PLY */
  { 0xfddc8, 20, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_PFDSP0 */
  { 0xfde2c, 20, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_PFDSP1 */
  { 0xfde90, 48, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_SUB_PLY */
  { 0xfdf80, 48, 8, 0xffffffff, 0, 0 }, /* MSGQ_AUD_OUTPUT_MIX */
  { 0xfe100, 32, 16, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RND_PLY */
  { 0xfe300, 16, 2, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RND_PLY_SYNC */
  { 0xfe320, 32, 16, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RND_SUB */
  { 0xfe520, 16, 2, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RND_SUB_SYNC */
  { 0xfe540, 48, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RECORDER */
  { 0xfe630, 24, 16, 0xffffffff, 0, 0 }, /* MSGQ_AUD_CAP */
  { 0xfe7b0, 16, 2, 0xffffffff, 0, 0 }, /* MSGQ_AUD_CAP_SYNC */
  { 0xfe7d0, 52, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_SOUND_EFFECT */
  { 0xfe8d4, 20, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RCG_CMD */
  { 0xfe938, 24, 16, 0xffffffff, 0, 0 }, /* MSGQ_AUD_CAP_MIC */
  { 0xfeab8, 16, 2, 0xffffffff, 0, 0 }, /* MSGQ_AUD_CAP_MIC_SYNC */
  { 0xfead8, 24, 16, 0xffffffff, 0, 0 }, /* MSGQ_AUD_CAP_I2S */
  { 0xfec58, 16, 2, 0xffffffff, 0, 0 }, /* MSGQ_AUD_CAP_I2S_SYNC */
  { 0xfec78, 32, 16, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RND_SPHP */
  { 0xfee78, 16, 2, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RND_SPHP_SYNC */
  { 0xfee98, 32, 16, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RND_I2S */
  { 0xff098, 16, 2, 0xffffffff, 0, 0 }, /* MSGQ_AUD_RND_I2S_SYNC */
  { 0xff0b8, 48, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_FRONTEND */
  { 0xff1a8, 20, 5, 0xffffffff, 0, 0 }, /* MSGQ_AUD_PREDSP */
  { 0xff20c, 80, 16, 0xffffffff, 0, 0 }, /* MSGQ_SEN_MGR */
};

#endif /* MSGQ_POOL_H_INCLUDED */
