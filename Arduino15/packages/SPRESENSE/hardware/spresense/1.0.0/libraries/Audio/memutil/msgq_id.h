/*
 * msgq_id.h -- Message queue pool ID and macro definition.
 *
 * This file was created by msgq_layout.conf
 * !!! CAUTION! don't edit this file manually !!!
 *
 *   Notes: (C) Copyright 2012 Sony Corporation
 */

#ifndef MSGQ_ID_H_INCLUDED
#define MSGQ_ID_H_INCLUDED

/* Message area size: 8404 bytes */
#define MSGQ_TOP_DRM	0xfc000
#define MSGQ_END_DRM	0xfe0d4

/* Message area fill value after message poped */
#define MSG_FILL_VALUE_AFTER_POP	0x0

/* Message parameter type match check */
#define MSG_PARAM_TYPE_MATCH_CHECK	false

/* Message queue pool IDs */
#define MSGQ_NULL	0
#define MSGQ_AUD_MGR	1
#define MSGQ_AUD_APP	2
#define MSGQ_AUD_DSP	3
#define MSGQ_AUD_PLY	4
#define MSGQ_AUD_SUB_PLY	5
#define MSGQ_AUD_OUTPUT_MIX	6
#define MSGQ_AUD_RND_PLY	7
#define MSGQ_AUD_RND_PLY_SYNC	8
#define MSGQ_AUD_RND_SUB	9
#define MSGQ_AUD_RND_SUB_SYNC	10
#define MSGQ_AUD_RECORDER	11
#define MSGQ_AUD_MEDIA_REC_SINK	12
#define MSGQ_AUD_CAP	13
#define MSGQ_AUD_CAP_SYNC	14
#define MSGQ_AUD_SOUND_EFFECT	15
#define MSGQ_AUD_RCG_CMD	16
#define MSGQ_AUD_CAP_MIC	17
#define MSGQ_AUD_CAP_MIC_SYNC	18
#define MSGQ_AUD_CAP_I2S	19
#define MSGQ_AUD_CAP_I2S_SYNC	20
#define MSGQ_AUD_RND_SPHP	21
#define MSGQ_AUD_RND_SPHP_SYNC	22
#define MSGQ_AUD_RND_I2S	23
#define MSGQ_AUD_RND_I2S_SYNC	24
#define NUM_MSGQ_POOLS	25

/* User defined constants */

/************************************************************************/
#define MSGQ_AUD_MGR_QUE_BLOCK_DRM	0xfc044
#define MSGQ_AUD_MGR_N_QUE_DRM	0xfc6a4
#define MSGQ_AUD_MGR_N_SIZE	88
#define MSGQ_AUD_MGR_N_NUM	4
#define MSGQ_AUD_MGR_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_MGR_H_SIZE	0
#define MSGQ_AUD_MGR_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_APP_QUE_BLOCK_DRM	0xfc088
#define MSGQ_AUD_APP_N_QUE_DRM	0xfc804
#define MSGQ_AUD_APP_N_SIZE	40
#define MSGQ_AUD_APP_N_NUM	2
#define MSGQ_AUD_APP_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_APP_H_SIZE	0
#define MSGQ_AUD_APP_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_DSP_QUE_BLOCK_DRM	0xfc0cc
#define MSGQ_AUD_DSP_N_QUE_DRM	0xfc854
#define MSGQ_AUD_DSP_N_SIZE	20
#define MSGQ_AUD_DSP_N_NUM	5
#define MSGQ_AUD_DSP_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_DSP_H_SIZE	0
#define MSGQ_AUD_DSP_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_PLY_QUE_BLOCK_DRM	0xfc110
#define MSGQ_AUD_PLY_N_QUE_DRM	0xfc8b8
#define MSGQ_AUD_PLY_N_SIZE	48
#define MSGQ_AUD_PLY_N_NUM	5
#define MSGQ_AUD_PLY_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_PLY_H_SIZE	0
#define MSGQ_AUD_PLY_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_SUB_PLY_QUE_BLOCK_DRM	0xfc154
#define MSGQ_AUD_SUB_PLY_N_QUE_DRM	0xfc9a8
#define MSGQ_AUD_SUB_PLY_N_SIZE	48
#define MSGQ_AUD_SUB_PLY_N_NUM	5
#define MSGQ_AUD_SUB_PLY_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_SUB_PLY_H_SIZE	0
#define MSGQ_AUD_SUB_PLY_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_OUTPUT_MIX_QUE_BLOCK_DRM	0xfc198
#define MSGQ_AUD_OUTPUT_MIX_N_QUE_DRM	0xfca98
#define MSGQ_AUD_OUTPUT_MIX_N_SIZE	48
#define MSGQ_AUD_OUTPUT_MIX_N_NUM	8
#define MSGQ_AUD_OUTPUT_MIX_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_OUTPUT_MIX_H_SIZE	0
#define MSGQ_AUD_OUTPUT_MIX_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RND_PLY_QUE_BLOCK_DRM	0xfc1dc
#define MSGQ_AUD_RND_PLY_N_QUE_DRM	0xfcc18
#define MSGQ_AUD_RND_PLY_N_SIZE	32
#define MSGQ_AUD_RND_PLY_N_NUM	16
#define MSGQ_AUD_RND_PLY_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RND_PLY_H_SIZE	0
#define MSGQ_AUD_RND_PLY_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RND_PLY_SYNC_QUE_BLOCK_DRM	0xfc220
#define MSGQ_AUD_RND_PLY_SYNC_N_QUE_DRM	0xfce18
#define MSGQ_AUD_RND_PLY_SYNC_N_SIZE	16
#define MSGQ_AUD_RND_PLY_SYNC_N_NUM	8
#define MSGQ_AUD_RND_PLY_SYNC_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RND_PLY_SYNC_H_SIZE	0
#define MSGQ_AUD_RND_PLY_SYNC_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RND_SUB_QUE_BLOCK_DRM	0xfc264
#define MSGQ_AUD_RND_SUB_N_QUE_DRM	0xfce98
#define MSGQ_AUD_RND_SUB_N_SIZE	32
#define MSGQ_AUD_RND_SUB_N_NUM	16
#define MSGQ_AUD_RND_SUB_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RND_SUB_H_SIZE	0
#define MSGQ_AUD_RND_SUB_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RND_SUB_SYNC_QUE_BLOCK_DRM	0xfc2a8
#define MSGQ_AUD_RND_SUB_SYNC_N_QUE_DRM	0xfd098
#define MSGQ_AUD_RND_SUB_SYNC_N_SIZE	16
#define MSGQ_AUD_RND_SUB_SYNC_N_NUM	8
#define MSGQ_AUD_RND_SUB_SYNC_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RND_SUB_SYNC_H_SIZE	0
#define MSGQ_AUD_RND_SUB_SYNC_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RECORDER_QUE_BLOCK_DRM	0xfc2ec
#define MSGQ_AUD_RECORDER_N_QUE_DRM	0xfd118
#define MSGQ_AUD_RECORDER_N_SIZE	48
#define MSGQ_AUD_RECORDER_N_NUM	5
#define MSGQ_AUD_RECORDER_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RECORDER_H_SIZE	0
#define MSGQ_AUD_RECORDER_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_MEDIA_REC_SINK_QUE_BLOCK_DRM	0xfc330
#define MSGQ_AUD_MEDIA_REC_SINK_N_QUE_DRM	0xfd208
#define MSGQ_AUD_MEDIA_REC_SINK_N_SIZE	36
#define MSGQ_AUD_MEDIA_REC_SINK_N_NUM	17
#define MSGQ_AUD_MEDIA_REC_SINK_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_MEDIA_REC_SINK_H_SIZE	0
#define MSGQ_AUD_MEDIA_REC_SINK_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_CAP_QUE_BLOCK_DRM	0xfc374
#define MSGQ_AUD_CAP_N_QUE_DRM	0xfd46c
#define MSGQ_AUD_CAP_N_SIZE	24
#define MSGQ_AUD_CAP_N_NUM	16
#define MSGQ_AUD_CAP_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_CAP_H_SIZE	0
#define MSGQ_AUD_CAP_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_CAP_SYNC_QUE_BLOCK_DRM	0xfc3b8
#define MSGQ_AUD_CAP_SYNC_N_QUE_DRM	0xfd5ec
#define MSGQ_AUD_CAP_SYNC_N_SIZE	16
#define MSGQ_AUD_CAP_SYNC_N_NUM	8
#define MSGQ_AUD_CAP_SYNC_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_CAP_SYNC_H_SIZE	0
#define MSGQ_AUD_CAP_SYNC_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_SOUND_EFFECT_QUE_BLOCK_DRM	0xfc3fc
#define MSGQ_AUD_SOUND_EFFECT_N_QUE_DRM	0xfd66c
#define MSGQ_AUD_SOUND_EFFECT_N_SIZE	52
#define MSGQ_AUD_SOUND_EFFECT_N_NUM	5
#define MSGQ_AUD_SOUND_EFFECT_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_SOUND_EFFECT_H_SIZE	0
#define MSGQ_AUD_SOUND_EFFECT_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RCG_CMD_QUE_BLOCK_DRM	0xfc440
#define MSGQ_AUD_RCG_CMD_N_QUE_DRM	0xfd770
#define MSGQ_AUD_RCG_CMD_N_SIZE	20
#define MSGQ_AUD_RCG_CMD_N_NUM	5
#define MSGQ_AUD_RCG_CMD_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RCG_CMD_H_SIZE	0
#define MSGQ_AUD_RCG_CMD_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_CAP_MIC_QUE_BLOCK_DRM	0xfc484
#define MSGQ_AUD_CAP_MIC_N_QUE_DRM	0xfd7d4
#define MSGQ_AUD_CAP_MIC_N_SIZE	24
#define MSGQ_AUD_CAP_MIC_N_NUM	16
#define MSGQ_AUD_CAP_MIC_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_CAP_MIC_H_SIZE	0
#define MSGQ_AUD_CAP_MIC_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_CAP_MIC_SYNC_QUE_BLOCK_DRM	0xfc4c8
#define MSGQ_AUD_CAP_MIC_SYNC_N_QUE_DRM	0xfd954
#define MSGQ_AUD_CAP_MIC_SYNC_N_SIZE	16
#define MSGQ_AUD_CAP_MIC_SYNC_N_NUM	8
#define MSGQ_AUD_CAP_MIC_SYNC_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_CAP_MIC_SYNC_H_SIZE	0
#define MSGQ_AUD_CAP_MIC_SYNC_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_CAP_I2S_QUE_BLOCK_DRM	0xfc50c
#define MSGQ_AUD_CAP_I2S_N_QUE_DRM	0xfd9d4
#define MSGQ_AUD_CAP_I2S_N_SIZE	24
#define MSGQ_AUD_CAP_I2S_N_NUM	16
#define MSGQ_AUD_CAP_I2S_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_CAP_I2S_H_SIZE	0
#define MSGQ_AUD_CAP_I2S_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_CAP_I2S_SYNC_QUE_BLOCK_DRM	0xfc550
#define MSGQ_AUD_CAP_I2S_SYNC_N_QUE_DRM	0xfdb54
#define MSGQ_AUD_CAP_I2S_SYNC_N_SIZE	16
#define MSGQ_AUD_CAP_I2S_SYNC_N_NUM	8
#define MSGQ_AUD_CAP_I2S_SYNC_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_CAP_I2S_SYNC_H_SIZE	0
#define MSGQ_AUD_CAP_I2S_SYNC_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RND_SPHP_QUE_BLOCK_DRM	0xfc594
#define MSGQ_AUD_RND_SPHP_N_QUE_DRM	0xfdbd4
#define MSGQ_AUD_RND_SPHP_N_SIZE	32
#define MSGQ_AUD_RND_SPHP_N_NUM	16
#define MSGQ_AUD_RND_SPHP_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RND_SPHP_H_SIZE	0
#define MSGQ_AUD_RND_SPHP_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RND_SPHP_SYNC_QUE_BLOCK_DRM	0xfc5d8
#define MSGQ_AUD_RND_SPHP_SYNC_N_QUE_DRM	0xfddd4
#define MSGQ_AUD_RND_SPHP_SYNC_N_SIZE	16
#define MSGQ_AUD_RND_SPHP_SYNC_N_NUM	8
#define MSGQ_AUD_RND_SPHP_SYNC_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RND_SPHP_SYNC_H_SIZE	0
#define MSGQ_AUD_RND_SPHP_SYNC_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RND_I2S_QUE_BLOCK_DRM	0xfc61c
#define MSGQ_AUD_RND_I2S_N_QUE_DRM	0xfde54
#define MSGQ_AUD_RND_I2S_N_SIZE	32
#define MSGQ_AUD_RND_I2S_N_NUM	16
#define MSGQ_AUD_RND_I2S_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RND_I2S_H_SIZE	0
#define MSGQ_AUD_RND_I2S_H_NUM	0
/************************************************************************/
#define MSGQ_AUD_RND_I2S_SYNC_QUE_BLOCK_DRM	0xfc660
#define MSGQ_AUD_RND_I2S_SYNC_N_QUE_DRM	0xfe054
#define MSGQ_AUD_RND_I2S_SYNC_N_SIZE	16
#define MSGQ_AUD_RND_I2S_SYNC_N_NUM	8
#define MSGQ_AUD_RND_I2S_SYNC_H_QUE_DRM	0xffffffff
#define MSGQ_AUD_RND_I2S_SYNC_H_SIZE	0
#define MSGQ_AUD_RND_I2S_SYNC_H_NUM	0
#endif /* MSGQ_ID_H_INCLUDED */
