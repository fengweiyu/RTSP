/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	MediaHandle.h
* Description		: 	MediaHandle operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef MEDIA_HANDLE_H
#define MEDIA_HANDLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string>

using std::string;


#define FRAME_BUFFER_MAX_SIZE               (2*1024*1024)   //2m
#define MAX_NALU_CNT_ONE_FRAME              8
#define VIDEO_ENC_PARAM_BUF_MAX_LEN     	64

typedef enum
{
	STREAM_TYPE_UNKNOW = 0,
    STREAM_TYPE_VIDEO_STREAM,
    STREAM_TYPE_AUDIO_STREAM,
    STREAM_TYPE_MUX_STREAM,
}E_StreamType;

typedef enum
{
	VIDEO_ENCODE_TYPE_H264 = 0,
    VIDEO_ENCODE_TYPE_H265,
    VIDEO_ENCODE_TYPE_VP8,
    VIDEO_ENCODE_TYPE_VP9,
}E_VideoEncodeType;

typedef enum
{
	AUDIO_ENCODE_TYPE_AAC = 0,
    AUDIO_ENCODE_TYPE_G711U,
    AUDIO_ENCODE_TYPE_G711A,
    AUDIO_ENCODE_TYPE_G726,
}E_AudioEncodeType;

typedef enum
{
	FRAME_TYPE_UNKNOW = 0,
    FRAME_TYPE_VIDEO_I_FRAME,
    FRAME_TYPE_VIDEO_P_FRAME,
    FRAME_TYPE_VIDEO_B_FRAME,
    FRAME_TYPE_AUDIO_FRAME,
        
}E_FrameType;


typedef struct MediaInfo
{
    E_StreamType eStreamType;
    E_VideoEncodeType eVideoEncType;
    unsigned int dwVideoSampleRate;
    E_AudioEncodeType eAudioEncType;
    unsigned int dwAudioSampleRate;
}T_MediaInfo;


typedef struct MediaFrameParam
{
    unsigned char *pbFrameBuf;//缓冲区
    int iFrameBufMaxLen;//缓冲区总大小
    int iFrameBufLen;//缓冲区读到数据的总大小
    int iFrameProcessedLen;
    
	//输出1帧数据结果
    unsigned char *pbFrameStartPos;
    int iFrameLen;
    unsigned int dwNaluCount;
    unsigned int a_dwNaluEndOffset[MAX_NALU_CNT_ONE_FRAME];

    E_FrameType eFrameType;
    int iVideoEncType;
    int iAudioEncType;
    unsigned int dwTimeStamp;
}T_MediaFrameParam;


typedef struct VideoEncodeParam
{
	unsigned char abSPS[VIDEO_ENC_PARAM_BUF_MAX_LEN];
	int iSizeOfSPS;
	unsigned char abPPS[VIDEO_ENC_PARAM_BUF_MAX_LEN];
	int iSizeOfPPS;
	unsigned char abVPS[VIDEO_ENC_PARAM_BUF_MAX_LEN];
	int iSizeOfVPS;
}T_VideoEncodeParam;





/*****************************************************************************
-Class			: MediaHandle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class MediaHandle
{
public:
    MediaHandle();
    virtual ~MediaHandle();
    virtual int Init(char *i_strPath);
    virtual int GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam);
    virtual int GetVideoEncParam(T_VideoEncodeParam *o_ptVideoEncodeParam);
    virtual int GetMediaInfo(T_MediaInfo *o_ptMediaInfo);

protected:
	T_MediaInfo m_tMediaInfo;
	
private:
    MediaHandle             *m_pMediaHandle;
	FILE                    *m_pMediaFile;
	//unsigned int 			m_dwFileReadOffset;
	
};









#endif
