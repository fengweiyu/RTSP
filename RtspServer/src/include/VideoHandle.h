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
#ifndef VIDEO_HANDLE_H
#define VIDEO_HANDLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string>

using std::string;


#define VIDEO_FORMAT_H264       ".h264"

#define VIDEO_BUFFER_MAX_SIZE               (2*1024*1024)   //2m
#define SPS_PPS_BUF_MAX_LEN                 64

/*****************************************************************************
-Class			: VideoHandle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class VideoHandle
{
public:
    VideoHandle();
    ~VideoHandle();
    int Init(char *i_strPath);
    int GetNextVideoFrame(unsigned char *o_pbVideoBuf,int *o_iVideoBufSize,int i_iBufMaxSize);
    int FindH264Nalu(unsigned char *i_pbVideoBuf,int i_iVideoBufLen,unsigned char **o_ppbNaluStartPos,int *o_iNaluLen,unsigned char *o_bNaluType);
    int TrySetSPS_PPS(unsigned char *i_pbNaluBuf,int i_iNaluLen);
    int GetSPS_PPS(unsigned char *o_pbSpsBuf,int *o_piSpsBufLen,unsigned char *o_pbPpsBuf,int *o_piPpsBufLen);
    int RemoveH264EmulationBytes(unsigned char *o_pbNaluBuf,int i_iMaxNaluBufLen,unsigned char *i_pbNaluBuf,int i_iNaluLen);
    
private:
    VideoHandle             *m_pVideoHandle;

};


/*****************************************************************************
-Class			: H264Handle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class H264Handle : public VideoHandle
{
public:
    H264Handle();
    ~H264Handle();
    int Init(char *i_strPath);
    int GetNextVideoFrame(unsigned char *o_pbVideoBuf,int *o_iVideoBufSize,int i_iBufMaxSize);
    int FindH264Nalu(unsigned char *i_pbVideoBuf,int i_iVideoBufLen,unsigned char **o_ppbNaluStartPos,int *o_iNaluLen,unsigned char *o_bNaluType);
    int TrySetSPS_PPS(unsigned char *i_pbNaluBuf,int i_iNaluLen);
    int GetSPS_PPS(unsigned char *o_pbSpsBuf,int *o_piSpsBufLen,unsigned char *o_pbPpsBuf,int *o_piPpsBufLen);
    int RemoveH264EmulationBytes(unsigned char *o_pbNaluBuf,int i_iMaxNaluBufLen,unsigned char *i_pbNaluBuf,int i_iNaluLen);

    static char *m_strVideoFormatName;
private:
	FILE *                  m_pVideoFile;
	unsigned char           m_abSPS[SPS_PPS_BUF_MAX_LEN];
	unsigned char           m_abPPS[SPS_PPS_BUF_MAX_LEN];
	int                     m_iSPS_Len;
	int                     m_iPPS_Len;
};



/*****************************************************************************
-Class			: VP9Handle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class VP9Handle : public VideoHandle
{
public:
    VP9Handle();
    ~VP9Handle();
    //int Init(char *i_strPath);


    //static char *m_strVideoFormatName;
private:
	FILE *                  m_pVideoFile;
	string                  m_strSPS;
	string                  m_strPPS;

};









#endif
