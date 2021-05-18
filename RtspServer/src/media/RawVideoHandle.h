/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	VideoHandle.h
* Description		: 	VideoHandle operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef RAW_VIDEO_HANDLE_H
#define RAW_VIDEO_HANDLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "MediaHandle.h"

using std::string;


#define VIDEO_ENC_FORMAT_H264_NAME        ".h264"
#define VIDEO_ENC_FORMAT_H265_NAME        ".h265"


/*****************************************************************************
-Class			: H264Handle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class H264Handle : public MediaHandle
{
public:
    H264Handle();
    ~H264Handle();
    virtual int Init(char *i_strPath);
    virtual int GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam);
    virtual int GetVideoEncParam(T_VideoEncodeParam *o_ptVideoEncodeParam);
    virtual int GetMediaInfo(T_MediaInfo *o_ptMediaInfo);
    
    static char *m_strVideoFormatName;
private:
    int RemoveH264EmulationBytes(unsigned char *o_pbNaluBuf,int i_iMaxNaluBufLen,unsigned char *i_pbNaluBuf,int i_iNaluLen);
	T_VideoEncodeParam      m_tVideoEncodeParam;
};



/*****************************************************************************
-Class			: H264Handle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class H265Handle : public MediaHandle
{
public:
    H265Handle();
    ~H265Handle();
    virtual int Init(char *i_strPath);
    virtual int GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam);
    virtual int GetVideoEncParam(T_VideoEncodeParam *o_ptVideoEncodeParam);
    virtual int GetMediaInfo(T_MediaInfo *o_ptMediaInfo);
    
    static char *m_strVideoFormatName;
private:
	T_VideoEncodeParam      m_tVideoEncodeParam;
};



/*****************************************************************************
-Class			: VP9Handle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class VP9Handle : public MediaHandle
{
public:
    VP9Handle();
    ~VP9Handle();
    //int Init(char *i_strPath);


    //static char *m_strVideoFormatName;
private:
    T_VideoEncodeParam      m_tVideoEncodeParam;
};









#endif
