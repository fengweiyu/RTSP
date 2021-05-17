/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	MediaHandle.cpp
* Description		: 	Media operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "MediaHandle.h"
#include <string.h>
#include <iostream>
#include "Definition.h"
#include "RawVideoHandle.h"
#include "RawAudioHandle.h"

using std::cout;//需要<iostream>
using std::endl;

/*****************************************************************************
-Fuction		: MediaHandle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
MediaHandle::MediaHandle()
{
	m_pMediaHandle =NULL;
    m_pMediaFile = NULL;
	
}

/*****************************************************************************
-Fuction		: ~VideoHandle
-Description	: ~VideoHandle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
MediaHandle::~MediaHandle()
{
	if(m_pMediaHandle !=NULL)
	{
        delete m_pMediaHandle;
	}
    if(NULL != m_pMediaFile)
    {
        fclose(m_pMediaFile);
    }
}

/*****************************************************************************
-Fuction		: VideoHandle::Init
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int MediaHandle::Init(char *i_strPath)
{
    int iRet=FALSE;
    if(NULL == i_strPath)
    {
        cout<<"Init NULL"<<endl;
        return iRet;
    }
    m_pMediaFile = fopen(i_strPath,"rb");
    if(NULL == m_pMediaFile)
    {
        cout<<"Init "<<i_strPath<<"failed !"<<endl;
        return iRet;
    } 
    
    //暂时不支持复合流
    if(NULL != strstr(i_strPath,H264Handle::m_strVideoFormatName))
    {
        m_pMediaHandle=new H264Handle();
        if(NULL !=m_pMediaHandle)
            iRet=m_pMediaHandle->Init(i_strPath);
        return iRet;
    }
    if(NULL != strstr(i_strPath,H265Handle::m_strVideoFormatName))
    {
        m_pMediaHandle=new H265Handle();
        if(NULL !=m_pMediaHandle)
            iRet=m_pMediaHandle->Init(i_strPath);
        return iRet;
    }
    if(NULL != strstr(i_strPath,G711Handle::m_strAudioFormatName))
    {
        m_pMediaHandle=new G711Handle();
        if(NULL !=m_pMediaHandle)
            iRet=m_pMediaHandle->Init(i_strPath);
        return iRet;
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
int MediaHandle::GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam)
{
    int iRet=FALSE;
    int iReadLen = 0;
    if(NULL == m_ptMediaFrameParam)
    {
        cout<<"GetNextFrame NULL"<<endl;
        return iRet;
    }
    iReadLen = fread(m_ptMediaFrameParam->pbFrameBuf, 1, m_ptMediaFrameParam->iFrameBufMaxLen, m_pMediaFile);
    if(iReadLen <= 0)
    {
        cout<<"fread err"<<endl;
        return iRet;
    }
    m_ptMediaFrameParam->iFrameBufLen = iReadLen;
    if(NULL !=m_pMediaHandle)
    {
        iRet = m_pMediaHandle->GetNextFrame(m_ptMediaFrameParam);
        if(TRUE == iRet)
        {
            fseek(m_pMediaFile,m_ptMediaFrameParam->iFrameProcessedLen,SEEK_SET);
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
int MediaHandle::GetVideoEncParam(T_VideoEncodeParam *o_ptVideoEncodeParam)
{
    int iRet=FALSE;
    if(NULL !=m_pMediaHandle)
    {
        iRet=m_pMediaHandle->GetVideoEncParam(o_ptVideoEncodeParam);
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
int MediaHandle::GetMediaInfo(T_MediaInfo *o_ptMediaInfo)
{
    int iRet=FALSE;
    if(NULL !=m_pMediaHandle)
    {
        iRet=m_pMediaHandle->GetMediaInfo(o_ptMediaInfo);
    }
	return iRet;
}

