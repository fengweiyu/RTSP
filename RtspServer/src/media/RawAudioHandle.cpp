/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RawAudioHandle.cpp
* Description		: 	RawAudioHandle operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include <string.h>
#include <iostream>
#include "Definition.h"
#include "RawAudioHandle.h"


using std::cout;//ÐèÒª<iostream>
using std::endl;


char * G711Handle::m_strAudioFormatName = (char *)AUDIO_ENCODE_FORMAT_G711;
int G711Handle::m_iAudioFixLen = AUDIO_BUFFER_G711_FIX_LEN;

/*****************************************************************************
-Fuction		: G711Handle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
G711Handle::G711Handle()
{
}
/*****************************************************************************
-Fuction		: ~G711Handle
-Description	: ~G711Handle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
G711Handle::~G711Handle()
{

}

/*****************************************************************************
-Fuction		: AudioHandle::Init
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int G711Handle::Init(char *i_strPath)
{
    int iRet=FALSE;
    if(NULL == i_strFilePath)
    {
        cout<<"Init NULL"<<endl;
        return iRet;
    }
    iRet = TRUE;
	return iRet;
}

/*****************************************************************************
-Fuction		: VideoHandle::GetNextVideoFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int G711Handle::GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam)
{
    int iRet=FALSE;
	int iRetSize=0;
	
	if(m_ptMediaFrameParam == NULL ||m_ptMediaFrameParam->iFrameBufLen < G711Handle::m_iAudioFixLen)
	{
        cout<<"G711Handle GetNextFrame err:"<<m_ptMediaFrameParam->iFrameBufLen<<endl;
        return iRet;
    }
    m_ptMediaFrameParam->pbFrameStartPos = m_ptMediaFrameParam->pbFrameBuf;
    m_ptMediaFrameParam->iFrameLen = m_iAudioFixLen;
	if(NULL != m_ptMediaFrameParam->pbFrameStartPos)
	{
        m_ptMediaFrameParam->iFrameProcessedLen = m_ptMediaFrameParam->pbFrameStartPos - m_ptMediaFrameParam->pbFrameBuf + m_ptMediaFrameParam->iFrameLen;
        iRet = TRUE;
	}
	return iRet;
}



