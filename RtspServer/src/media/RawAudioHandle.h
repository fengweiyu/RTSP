/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RawAudioHandle.h
* Description		: 	RawAudioHandle operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef RAW_AUDIO_HANDLE_H
#define RAW_AUDIO_HANDLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "MediaHandle.h"

using std::string;


#define AUDIO_ENC_FORMAT_G711_NAME        	".G711"
#define AUDIO_ENC_FORMAT_AAC_NAME        	".AAC"




/*****************************************************************************
-Class			: G711Handle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class G711Handle : public MediaHandle
{
public:
    G711Handle();
    ~G711Handle();
    virtual int Init(char *i_strPath);
    virtual int GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam);
    virtual int GetMediaInfo(T_MediaInfo *o_ptMediaInfo);
    
    static char  *m_strAudioFormatName;
    static int    m_iAudioFixLen;
private:
	//FILE *                  m_pAudioFile;
};




/*****************************************************************************
-Class			: G711Handle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class AACHandle : public MediaHandle
{
public:
    AACHandle();
    ~AACHandle();
    virtual int Init(char *i_strPath);
    virtual int GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam);
    virtual int GetMediaInfo(T_MediaInfo *o_ptMediaInfo);
    
    static char  *m_strAudioFormatName;
private:
	//FILE *                  m_pAudioFile;
};






#endif

