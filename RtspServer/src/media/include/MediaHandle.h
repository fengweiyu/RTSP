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


typedef struct MediaFrameParam
{
    unsigned char *pbFrameBuf;
    int iFrameBufLen;
    int iFrameBufMaxLen;

    unsigned char *pbFrameStartPos;
    int iFrameProcessedLen;
    int iFrameLen;
    unsigned int dwNaluCount;
    unsigned int a_dwNaluEndOffset[MAX_NALU_CNT_ONE_FRAME];

    int iFrameType;
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
    
private:
    MediaHandle             *m_pMediaHandle;
	FILE                    *m_pMediaFile;
	//unsigned int 			m_dwFileReadOffset;
	
};









#endif
