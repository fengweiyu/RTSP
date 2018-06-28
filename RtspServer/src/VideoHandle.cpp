/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	VideoHandle.cpp
* Description		: 	VideoHandle operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "Definition.h"
#include "VideoHandle.h"
#include <string.h>
#include <iostream>
//#include "RtspServer.h"//移动VIDEO_BUFFER_MAX_SIZE 宏后则可以去掉这行

using std::cout;//需要<iostream>
using std::endl;

/*****************************************************************************
-Fuction		: VideoHandle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
VideoHandle::VideoHandle()
{
	m_pVideoHandle =NULL;
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
VideoHandle::~VideoHandle()
{
	m_pVideoHandle =NULL;

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
int VideoHandle::Init(char *i_strPath)
{
    int iRet=FALSE;
    if(NULL == i_strPath)
    {
        cout<<"Init NULL"<<endl;
    }
    else
    {
        if(NULL != strstr(i_strPath,H264Handle::m_strVideoFormatName))
        {
            m_pVideoHandle=new H264Handle();
            if(NULL !=m_pVideoHandle)
                iRet=m_pVideoHandle->Init(i_strPath);
        }
        else
        {
            cout<<"Init err,UknowFormat:"<<i_strPath<<endl;
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
int VideoHandle::GetNextVideoFrame(unsigned char *o_pbVideoBuf,int *o_iVideoBufSize,int i_iBufMaxSize)
{
    int iRet=FALSE;
    if(NULL !=m_pVideoHandle)
    {
        iRet=m_pVideoHandle->GetNextVideoFrame(o_pbVideoBuf,o_iVideoBufSize,i_iBufMaxSize);
    }
	return iRet;
}

/*****************************************************************************
-Fuction		: VideoHandle::FindH264Nalu
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int VideoHandle::FindH264Nalu(unsigned char *i_pbVideoBuf,int i_iVideoBufLen,unsigned char **o_ppbNaluStartPos,int *o_iNaluLen,unsigned char *o_bNaluType)
{
    int iRet=FALSE;
    if(NULL !=m_pVideoHandle)
    {
        iRet=m_pVideoHandle->FindH264Nalu(i_pbVideoBuf,i_iVideoBufLen,o_ppbNaluStartPos,o_iNaluLen,o_bNaluType);
    }
	return iRet;
}

/*****************************************************************************
-Fuction		: VideoHandle::TrySetSPS_PPS
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int VideoHandle::TrySetSPS_PPS(unsigned char *i_pbNaluBuf,int i_iNaluLen)
{
    int iRet=FALSE;
    if(NULL !=m_pVideoHandle)
    {
        iRet=m_pVideoHandle->TrySetSPS_PPS(i_pbNaluBuf,i_iNaluLen);
    }
	return iRet;
}

/*****************************************************************************
-Fuction		: VideoHandle::GetSPS_PPS
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int VideoHandle::GetSPS_PPS(unsigned char *o_pbSpsBuf,int *o_piSpsBufLen,unsigned char *o_pbPpsBuf,int *o_piPpsBufLen)
{
    int iRet=FALSE;
    if(NULL !=m_pVideoHandle)
    {
        iRet=m_pVideoHandle->GetSPS_PPS(o_pbSpsBuf,o_piSpsBufLen,o_pbPpsBuf,o_piPpsBufLen);
    }
	return iRet;
}

/*****************************************************************************
-Fuction		: VideoHandle::RemoveH264EmulationBytes
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int VideoHandle::RemoveH264EmulationBytes(unsigned char *o_pbNaluBuf,int i_iMaxNaluBufLen,unsigned char *i_pbNaluBuf,int i_iNaluLen)
{
    int iNaluLen=0;
    if(NULL !=m_pVideoHandle)
    {
        iNaluLen=m_pVideoHandle->RemoveH264EmulationBytes(o_pbNaluBuf,i_iMaxNaluBufLen,i_pbNaluBuf,i_iNaluLen);
    }
	return iNaluLen;
}

char * H264Handle::m_strVideoFormatName=(char *)VIDEO_FORMAT_H264;
/*****************************************************************************
-Fuction		: H264Handle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
H264Handle::H264Handle()
{
    m_pVideoFile = NULL;
	memset(m_abSPS,0,sizeof(m_abSPS));
	memset(m_abPPS,0,sizeof(m_abPPS));
    m_iSPS_Len = 0;
    m_iPPS_Len = 0;
}
/*****************************************************************************
-Fuction		: ~H264Handle
-Description	: ~H264Handle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
H264Handle::~H264Handle()
{
    m_pVideoFile = NULL;

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
int H264Handle::Init(char *i_strPath)
{
    int iRet=FALSE;
    if(NULL == i_strPath)
    {
        cout<<"Init NULL"<<endl;
    }
    else
    {
        m_pVideoFile = fopen(i_strPath,"rb");
        if(NULL == m_pVideoFile)
        {
            cout<<"Init "<<i_strPath<<"failed !"<<endl;
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
int H264Handle::GetNextVideoFrame(unsigned char *o_pbVideoBuf,int *o_iVideoBufSize,int i_iBufMaxSize)
{
    int iRet=FALSE;
    int iVideoBufSize=0;
    unsigned char abReadDataBuf[1024]={0};
    int iReadDataLen=0;
	//int iRemainDataLen = 0;
	int iRetSize=0;
	int iNaluStartPos=0;
	if(o_pbVideoBuf==NULL ||i_iBufMaxSize<VIDEO_BUFFER_MAX_SIZE ||NULL==m_pVideoFile||NULL == o_iVideoBufSize)
	{
        cout<<"GetNextVideoFrame err:"<<i_iBufMaxSize<<"m_pVideoFile:"<<m_pVideoFile<<endl;
        iRet=FALSE;
	}
	else
	{
        while ((iRetSize = fread(abReadDataBuf + iReadDataLen, 1, sizeof(abReadDataBuf) - iReadDataLen, m_pVideoFile)) > 0) 
        {
            iNaluStartPos = 3;//略过第一个开始码,保证一开始读取的就是一帧(nalu)数据
            iReadDataLen += iRetSize;
            while (iNaluStartPos < iReadDataLen - 3 && !(abReadDataBuf[iNaluStartPos] == 0 &&  abReadDataBuf[iNaluStartPos+1] == 0 && (abReadDataBuf[iNaluStartPos+2] == 1
            || (abReadDataBuf[iNaluStartPos+2] == 0 && abReadDataBuf[iNaluStartPos+3] == 1)))) 
            {
                iNaluStartPos++;
            }
            memcpy(o_pbVideoBuf + iVideoBufSize, abReadDataBuf, iNaluStartPos);
            iVideoBufSize +=iNaluStartPos;
            
            memmove(abReadDataBuf, abReadDataBuf +iNaluStartPos, iReadDataLen - iNaluStartPos);//拷贝除去nalu之后的数据，拷贝后大于3表示读取了一个nalu则退出
            iReadDataLen -= iNaluStartPos;//如果前面memcpy拷贝的不够一个nalu，则memmove无效，综合来看memmove后续可以去掉
            if (iReadDataLen >= 3)//注意!!!,考虑恰巧读到是一个nalu+3个开始码的情况所以使用>=而不是> 
            {//???,考虑恰巧每次读到是一个nalu+小于3个的开始码的情况,此时会一直拷贝不会退出循环，
            //或许这里应该使用iReadDataLen>=0,同时还需修改相关逻辑，比如结合开始码和结束码(下一个的开始码)一起判断(可以设开始和结束两个标记)
                fseek(m_pVideoFile, -iReadDataLen, SEEK_CUR);//文件指针从当前位置向前移动多读取的部分,即多出nalu的部分
                break;
            }
        }
        if(iVideoBufSize>i_iBufMaxSize)
        {//增加容错处理
            cout<<"GetNextVideoFrame too long err:"<<iVideoBufSize<<endl;
        }
        else
        {
            if(iRetSize<=0)
            {
                cout<<"fread err:"<<iRetSize<<endl;
                fseek(m_pVideoFile, 0, SEEK_SET);
            }
            else
            {
                *o_iVideoBufSize = iVideoBufSize;
                iRet=TRUE;
            }
        }
	}
	return iRet;
}

/*****************************************************************************
-Fuction		: VideoHandle::FindH264Nalu
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H264Handle::FindH264Nalu(unsigned char *i_pbVideoBuf,int i_iVideoBufLen,unsigned char **o_ppbNaluStartPos,int *o_iNaluLen,unsigned char *o_bNaluType)
{
    int iRet=FALSE;

    if(NULL == i_pbVideoBuf || NULL == o_ppbNaluStartPos ||NULL ==o_iNaluLen || NULL ==  o_bNaluType)
    {
        cout<<"FindH264Nalu NULL"<<endl;
    }
    else
    {
        unsigned char *pbNaluStartPos=NULL;
        unsigned char *pbVideoBuf=i_pbVideoBuf;
        int iVideoBufLen=i_iVideoBufLen;
        while (iVideoBufLen >= 3) 
        {
            if (pbVideoBuf[0] == 0 && pbVideoBuf[1] == 0 && pbVideoBuf[2] == 1) 
            {
                if (!pbNaluStartPos) 
                {//一个nalu的开始码
                    if (iVideoBufLen < 4)
                    {
                        iRet=FALSE;//保险起见，由于初始化了其实可以去掉
                        pbNaluStartPos=NULL;
                        break;//返回结果
                    }
                    else
                    {
                        pbNaluStartPos = pbVideoBuf;
                        *o_bNaluType = pbVideoBuf[3] & 0x1f;
                    }
                } 
                else 
                {//另一个nalu的开始码，即前一个nalu的结束码，也就是找到一整个nalu了
                    *o_iNaluLen = (pbVideoBuf - pbNaluStartPos);
                    iRet=TRUE;
                    break;//返回结果
                }
                pbVideoBuf += 3;
                iVideoBufLen  -= 3;
                continue;
            }
            if (iVideoBufLen >= 4 && pbVideoBuf[0] == 0 && pbVideoBuf[1] == 0 && pbVideoBuf[2] == 0 && pbVideoBuf[3] == 1) 
            {
                if (!pbNaluStartPos) 
                {//一个nalu的开始码
                    if (iVideoBufLen < 5)
                    {
                        iRet=FALSE;//保险起见，由于初始化了其实可以去掉
                        pbNaluStartPos=NULL;
                        break;//返回结果
                    }
                    else
                    {
                        pbNaluStartPos= pbVideoBuf;
                        *o_bNaluType = pbVideoBuf[4] & 0x1f;
                    }
                } 
                else 
                {//另一个nalu的开始码，即前一个nalu的结束码，也就是找到一整个nalu了
                    *o_iNaluLen = (pbVideoBuf - pbNaluStartPos);
                    iRet=TRUE;
                    break;//返回结果
                }
                pbVideoBuf += 4;
                iVideoBufLen  -= 4;
                continue;
            }
            pbVideoBuf ++;
            iVideoBufLen --;
        }
        if(NULL != pbNaluStartPos && FALSE==iRet)
        {
            *o_iNaluLen = (pbVideoBuf - pbNaluStartPos + iVideoBufLen);//整个buf里就一个nalu的情况
            iRet=TRUE;
        }
    }
    return iRet;
}


/*****************************************************************************
-Fuction		: VideoHandle::TrySetSPS_PPS
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H264Handle::TrySetSPS_PPS(unsigned char *i_pbNaluBuf,int i_iNaluLen)
{
    int iRet=FALSE;
    unsigned char bNaluType = 0;
    
    if (m_iSPS_Len > 0 && m_iPPS_Len > 0) 
    {//只需要第一个SPS和PPS
        iRet=TRUE;
    }
    else
    {
        if (i_pbNaluBuf[0] == 0 && i_pbNaluBuf[1] == 0 && i_pbNaluBuf[2] == 1) {
            bNaluType = i_pbNaluBuf[3] & 0x1f;
            i_pbNaluBuf += 3;
            i_iNaluLen   -= 3;
        }
        if (i_pbNaluBuf[0] == 0 && i_pbNaluBuf[1] == 0 && i_pbNaluBuf[2] == 0 && i_pbNaluBuf[3] == 1) {
            bNaluType = i_pbNaluBuf[4] & 0x1f;
            i_pbNaluBuf += 4;
            i_iNaluLen   -= 4;
        }
        if (i_iNaluLen < 1)
        {
            iRet=FALSE;
        }
        else
        {
            if (bNaluType == 7 && 0 == m_iSPS_Len) 
            {
                cout<<"SPS Len:"<<i_iNaluLen<<endl;
                if ((unsigned int)i_iNaluLen > sizeof(m_abSPS))
                    i_iNaluLen = sizeof(m_abSPS);
                    
                memcpy(m_abSPS, i_pbNaluBuf, i_iNaluLen);
                m_iSPS_Len = i_iNaluLen;
            }
            if (bNaluType == 8 && 0 == m_iPPS_Len) 
            {
                cout<<"PPS Len:"<<i_iNaluLen<<endl;
                if ((unsigned int)i_iNaluLen > sizeof(m_abPPS))
                    i_iNaluLen = sizeof(m_abPPS);
                    
                memcpy(m_abPPS, i_pbNaluBuf, i_iNaluLen);
                m_iPPS_Len = i_iNaluLen;
            }
            iRet=TRUE;
        }
    }
    return iRet;
}


/*****************************************************************************
-Fuction		: VideoHandle::GetSPS_PPS
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H264Handle::GetSPS_PPS(unsigned char *o_pbSpsBuf,int *o_piSpsBufLen,unsigned char *o_pbPpsBuf,int *o_piPpsBufLen)
{
    int iRet=FALSE;
    if(NULL == o_pbSpsBuf ||NULL == o_piSpsBufLen ||NULL == o_pbPpsBuf ||NULL == o_piPpsBufLen)
    {
        cout<<"GetSPS_PPS NULL"<<endl;
    }
    else
    {
        memcpy(o_pbSpsBuf,m_abSPS,m_iSPS_Len);
        *o_piSpsBufLen =m_iSPS_Len;
        memcpy(o_pbPpsBuf,m_abPPS,m_iPPS_Len);
        *o_piPpsBufLen =m_iPPS_Len;
        
        iRet=TRUE;
    }
    
	return iRet;
}


/*****************************************************************************
-Fuction		: VideoHandle::RemoveH264EmulationBytes
-Description	: 去掉h264中防止竞争的字节（脱壳操作）
-Input			: i_pbNaluBuf i_iNaluLen i_iMaxNaluBufLen
-Output 		: o_pbNaluBuf
-Return 		: iNaluLen //返回脱壳操作后的长度
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H264Handle::RemoveH264EmulationBytes(unsigned char *o_pbNaluBuf,int i_iMaxNaluBufLen,unsigned char *i_pbNaluBuf,int i_iNaluLen)
{
    int iNaluLen=0;
    
    int i = 0;
    while (i < i_iNaluLen && iNaluLen+1 < i_iMaxNaluBufLen) 
    {
      if (i+2 < i_iNaluLen && i_pbNaluBuf[i] == 0 && i_pbNaluBuf[i+1] == 0 && i_pbNaluBuf[i+2] == 3) 
      {
        o_pbNaluBuf[iNaluLen] = o_pbNaluBuf[iNaluLen+1] = 0;
        iNaluLen += 2;
        i += 3;
      } 
      else 
      {
        o_pbNaluBuf[iNaluLen] = i_pbNaluBuf[i];
        iNaluLen += 1;
        i += 1;
      }
    }
    
    return iNaluLen;
}

