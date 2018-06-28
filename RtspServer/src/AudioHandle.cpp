/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	AudioHandle.cpp
* Description		: 	AudioHandle operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include <string.h>
#include <iostream>
#include "Definition.h"
#include "AudioHandle.h"


using std::cout;//ÐèÒª<iostream>
using std::endl;

/*****************************************************************************
-Fuction		: AudioHandle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
AudioHandle::AudioHandle()
{
	m_pAudioHandle =NULL;
}

/*****************************************************************************
-Fuction		: ~AudioHandle
-Description	: ~AudioHandle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
AudioHandle::~AudioHandle()
{
	m_pAudioHandle =NULL;

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
int AudioHandle::Init(char *i_strFilePath)
{
    int iRet=FALSE;
    if(NULL == i_strFilePath)
    {
        cout<<"Init NULL"<<endl;
    }
    else
    {
        if(NULL != strstr(i_strFilePath,G711Handle::m_strAudioFormatName))
        {
            m_pAudioHandle=new G711Handle();
            if(NULL !=m_pAudioHandle)
                iRet=m_pAudioHandle->Init(i_strFilePath);
        }
        else
        {
            cout<<"AudioHandle Init err,UknowFormat:"<<i_strFilePath<<endl;
        }
    }
    
	return iRet;
}

/*****************************************************************************
-Fuction		: AudioHandle::GetNextAudioFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int AudioHandle::GetNextAudioFrame(unsigned char *o_pbAudioBuf,int *o_iAudioBufSize,int i_iBufMaxSize)
{
    int iRet=FALSE;
    if(NULL !=m_pAudioHandle)
    {
        iRet=m_pAudioHandle->GetNextAudioFrame(o_pbAudioBuf,o_iAudioBufSize,i_iBufMaxSize);
    }
	return iRet;
}

char * G711Handle::m_strAudioFormatName=(char *)AUDIO_FORMAT_G711;
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
    m_pAudioFile = NULL;
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
    m_pAudioFile = NULL;

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
int G711Handle::Init(char *i_strFilePath)
{
    int iRet=FALSE;
    if(NULL == i_strFilePath)
    {
        cout<<"Init NULL"<<endl;
    }
    else
    {
        m_pAudioFile = fopen(i_strFilePath,"rb");
        if(NULL == m_pAudioFile)
        {
            cout<<"Init "<<i_strFilePath<<"failed !"<<endl;
            iRet = FALSE;
        }      
        else
        {
            iRet=TRUE;
        }
    }
    
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
int G711Handle::GetNextAudioFrame(unsigned char *o_pbAudioBuf,int *o_iAudioBufSize,int i_iBufMaxSize)
{
    int iRet=FALSE;
	int iRetSize=0;
	if(o_pbAudioBuf==NULL ||i_iBufMaxSize<AUDIO_BUFFER_MAX_SIZE ||NULL==m_pAudioFile||NULL == o_iAudioBufSize)
	{
        cout<<"GetNextAudioFrame err:"<<i_iBufMaxSize<<"m_pVideoFile:"<<m_pAudioFile<<endl;
        iRet=FALSE;
	}
	else
	{
        iRetSize = fread(o_pbAudioBuf, 1, AUDIO_BUFFER_MAX_SIZE, m_pAudioFile);
        if (iRetSize > 0) 
        {
            *o_iAudioBufSize = iRetSize;
            iRet=TRUE;
        }
	
	}
	return iRet;
}



