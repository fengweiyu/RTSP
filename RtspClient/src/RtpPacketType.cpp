/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RtpPacketType.cpp
* Description		: 	RtpPacketType operation center
* Created			: 	2017.09.28.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/

#include <stdlib.h>//还是需要.h
#include <stdio.h>
#include <string.h>
#include <iostream>//不加.h,c++新的头文件

#include "RtpPacketType.h"

using std::cout;//需要<iostream>
using std::endl;

/*****************************************************************************
-Fuction		: ParseRtpPacketF
-Description	: ParseRtpPacketF
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/28	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
unsigned char RtpPacket::ParseRtpPacketF(unsigned char *i_aucRtpPayloadData)
{

	unsigned char ucRtpPacketTypeMask=0x80; // binary: 1000_0000

	return (i_aucRtpPayloadData[0] & ucRtpPacketTypeMask);
}

/*****************************************************************************
-Fuction		: ParseRtpPacketNRI
-Description	: ParseRtpPacketNRI
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/28	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
unsigned char RtpPacket::ParseRtpPacketNRI(unsigned char *i_aucRtpPayloadData)
{

	unsigned char ucRtpPacketTypeMask= 0x60; // binary: 0110_0000  

	return (i_aucRtpPayloadData[0] & ucRtpPacketTypeMask);
}

/*****************************************************************************
-Fuction		: ParseRtpPacketType
-Description	: ParseRtpPacketType
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/28	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
unsigned char RtpPacket::ParseRtpPacketType(unsigned char *i_aucRtpPayloadData)
{

	unsigned char ucRtpPacketTypeMask= 0x1F; // binary: 0001_1111   取低五位

	return (i_aucRtpPayloadData[0] & ucRtpPacketTypeMask);
}

/*****************************************************************************
-Fuction		: IsThisPacketType
-Description	: IsThisPacketType
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
bool NALU::IsThisPacketType(unsigned char i_ucRtpPacketType)
{
	bool blRet=false;
	if(i_ucRtpPacketType<1||i_ucRtpPacketType>12)//13..23保留
	{
		blRet=false;
		//cout<<"IsNotNALUType,err:"<<i_ucRtpPacketType<<endl;
	}
	else
	{
		blRet=true;
	}
	return blRet;
}

/*****************************************************************************
-Fuction		: IsPacketEnd
-Description	: IsPacketEnd
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
bool NALU::GetEndFlag()
{
	return true;
}

/*****************************************************************************
-Fuction		: CopyVideoData
-Description	: CopyVideoData
-Input			: 
-Output 		: 
-Return 		: 失败返回0，成功返回拷贝的长度
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
unsigned int NALU::CopyVideoData(unsigned char *i_aucRtpPacketData,unsigned int i_dwDataLen,unsigned char *o_aucNaluData)
{
	unsigned  int dwLen=0;
	// NALU start code: 0x00000001 
	o_aucNaluData[0]=0;
	o_aucNaluData[1]=0;
	o_aucNaluData[2]=0;
	o_aucNaluData[3]=1;
	dwLen=4;

	memcpy(o_aucNaluData + dwLen, i_aucRtpPacketData, i_dwDataLen);
	dwLen += i_dwDataLen;
	return dwLen;
}

const  unsigned char FU_A::FU_A_TYPE=28;

/*****************************************************************************
-Fuction		: IsThisPacketType
-Description	: IsThisPacketType
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
bool FU_A::IsThisPacketType(unsigned char i_ucRtpPacketType)
{
	bool blRet=false;
	if(i_ucRtpPacketType!=FU_A_TYPE)
	{
		blRet=false;
		//cout<<"IsNotFU_A Type,err:"<<i_ucRtpPacketType<<endl;
	}
	else
	{
		blRet=true;
	}
	return blRet;
}

/*****************************************************************************
-Fuction		: IsPacketStart
-Description	: IsPacketStart
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
bool FU_A::IsPacketStart(unsigned char *i_aucRtpPayloadData)
{
	bool blRet=false;
	unsigned char ucPacketStartMask = 0x80; // binary:1000_0000
	if((i_aucRtpPayloadData[1] & ucPacketStartMask) ==0)
	{
	}
	else
	{
		blRet=true;
	}	
	return blRet;
}

/*****************************************************************************
-Fuction		: IsPacketEnd
-Description	: IsPacketEnd
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
bool FU_A::IsPacketEnd(unsigned char *i_aucRtpPayloadData)
{
	bool blRet=false;
	unsigned char ucPacketEndMask = 0x40; // binary:0100_0000
	if((i_aucRtpPayloadData[1] & ucPacketEndMask) ==0)
	{
	}
	else
	{
		blRet=true;
	}
	return blRet;
}

/*****************************************************************************
-Fuction		: GetNaluHeader
-Description	: GetNaluHeader
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
unsigned char FU_A::GetNaluHeader(unsigned char *i_aucRtpPayloadData)
{
	unsigned char ucNaluHeader=0;
	unsigned char ucNaluType=0;
	unsigned char ucNaluTypeMask= 0x1F; // binary: 0001_1111	取低五位
	ucNaluType=i_aucRtpPayloadData[1] & ucNaluTypeMask;//FU_A NaluType在第二字节
	ucNaluHeader=RtpPacket::ParseRtpPacketF(i_aucRtpPayloadData)|RtpPacket::ParseRtpPacketNRI(i_aucRtpPayloadData)|ucNaluType;
	return ucNaluHeader;
}

/*****************************************************************************
-Fuction		: CopyVideoData
-Description	: CopyVideoData
-Input			: 
-Output 		: 
-Return 		: 失败返回0，成功返回拷贝的长度
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
unsigned int FU_A::CopyVideoData(unsigned char *i_aucRtpPacketData,unsigned int i_dwDataLen,unsigned char *o_aucNaluData)
{
	unsigned  int dwLen=0;
	const int FU_A_HeaderLen = 2;
	unsigned char ucNaluHeader=0;
	ucNaluHeader=GetNaluHeader(i_aucRtpPacketData);
	m_blStartFlag=IsPacketStart(i_aucRtpPacketData);
	m_blEndFlag=IsPacketEnd(i_aucRtpPacketData);
	if(m_blStartFlag) 
	{
		// NALU start code: 0x00000001 
		o_aucNaluData[0]=0;
		o_aucNaluData[1]=0;
		o_aucNaluData[2]=0;
		o_aucNaluData[3]=1;
		dwLen=4;
		memcpy(o_aucNaluData + dwLen, &ucNaluHeader, sizeof(ucNaluHeader));
		dwLen += sizeof(ucNaluHeader);
	}
	else
	{
	}
	memcpy(o_aucNaluData + dwLen, i_aucRtpPacketData + FU_A_HeaderLen, i_dwDataLen - FU_A_HeaderLen);
	dwLen += i_dwDataLen - FU_A_HeaderLen;
	return dwLen;
}

/*****************************************************************************
-Fuction		: GetEndFlag
-Description	: GetEndFlag
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
bool FU_A::GetEndFlag()
{
	return m_blEndFlag;
}

