/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	AudioHandle.h
* Description		: 	AudioHandle operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef AUDIO_HANDLE_H
#define AUDIO_HANDLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string>

using std::string;


#define AUDIO_FORMAT_G711       ".G711"

#define AUDIO_BUFFER_MAX_SIZE               320

/*****************************************************************************
-Class			: AudioHandle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class AudioHandle
{
public:
    AudioHandle();
    ~AudioHandle();
    int Init(char *i_strFilePath);
    int GetNextAudioFrame(unsigned char *o_pbAudioBuf,int *o_iAudioBufSize,int i_iBufMaxSize);

private:
    AudioHandle             *m_pAudioHandle;
    
};


/*****************************************************************************
-Class			: G711Handle
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class G711Handle : public AudioHandle
{
public:
    G711Handle();
    ~G711Handle();
    int Init(char *i_strFilePath);
    int GetNextAudioFrame(unsigned char *o_pbAudioBuf,int *o_iAudioBufSize,int i_iBufMaxSize);
    
    static char *m_strAudioFormatName;
private:
	FILE *                  m_pAudioFile;
};









#endif

