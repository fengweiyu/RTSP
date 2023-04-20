/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RtspServer.cpp
* Description		: 	RtspServer operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

#include "Definition.h"
#include "RtpPacket.h"
#include "Base64.hh"
#include "Tools.h"
#include "RtspServer.h"

using std::cout;//��Ҫ<iostream>
using std::hex;
using std::endl;
using std::stringstream;

static T_VideoEncTypeToSdpEncName g_atVideoEncTypeToSdpEncName[]={
	{VIDEO_ENCODE_TYPE_H264,SDP_H264_ENC_FORMAT_NAME},
    {VIDEO_ENCODE_TYPE_H265,SDP_H265_ENC_FORMAT_NAME}
};
static T_AudioEncTypeToSdpEncName g_atAudioEncTypeToSdpEncName[]={
	{AUDIO_ENCODE_TYPE_AAC,SDP_AAC_ENC_FORMAT_NAME},
    {AUDIO_ENCODE_TYPE_G711U,SDP_G711U_ENC_FORMAT_NAME},
    {AUDIO_ENCODE_TYPE_G711A,SDP_G711A_ENC_FORMAT_NAME}
};

string RtspServer::m_astrCmd[]={"OPTIONS", "DESCRIBE", "SETUP", "TEARDOWN", "PLAY", "PAUSE"};

/*****************************************************************************
-Fuction		: RtspServer
-Description	: �����ĳɴ����д�ص���������ʽ
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtspServer::RtspServer()
{
    m_SessionList.clear();
    m_strURL.assign("");
    m_strIP.assign("");
    m_wPort = 0;
    m_dwBandwidth =500;//Ĭ��500kbps
    m_iSessionCount =0;
    m_HandleCmdMap.insert(make_pair("OPTIONS", &RtspServer::HandleCmdOPTIONS)); //WirelessNetManage::ָ��������,����ͷ�ļ��ﶨ������ʱҲҪָ��
    m_HandleCmdMap.insert(make_pair("DESCRIBE", &RtspServer::HandleCmdDESCRIBE)); //��ĳ�Ա��������ָ�룬����Ҫ�� &
    m_HandleCmdMap.insert(make_pair("SETUP", &RtspServer::HandleCmdSETUP));
    m_HandleCmdMap.insert(make_pair("PLAY", &RtspServer::HandleCmdPLAY));
    m_HandleCmdMap.insert(make_pair("PAUSE", &RtspServer::HandleCmdPAUSE));
    m_HandleCmdMap.insert(make_pair("TEARDOWN", &RtspServer::HandleCmdTEARDOWN));
    pthread_mutex_init(&m_tSessionMutex, NULL);
    
    //�����߳�
    pthread_t tSessionHandleID;
	if( pthread_create(&tSessionHandleID,NULL,RtspServer::SessionHandleThread, (void *)this) != 0 )
	{
		perror("SessionHandleThread pthread_create err\r\n");
	}
    pthread_t tRtspStreamHandle;
	if( pthread_create(&tRtspStreamHandle,NULL,RtspServer::RtspStreamHandleThread, (void *)this) != 0 )
	{
		perror("tRtspStreamHandle pthread_create err\r\n");
	}
    m_pRtpPacket = new RtpPacket();
}

/*****************************************************************************
-Fuction		: ~RtspServer
-Description	: ~RtspServer
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtspServer::~RtspServer()
{
    if(NULL != m_pRtpPacket)
    {
        delete m_pRtpPacket;
        m_pRtpPacket = NULL;
    }
}
/*****************************************************************************
-Fuction		: GetIpAndPort
-Description	: GetIpAndPort
-Input			: " rtsp://192.168.1.88:554/1"
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::GetIpAndPort(string * i_pstrURL,string *o_Ip,unsigned short *o_wPort)
{
	int iRet=FALSE;
	string IP("");
	unsigned int iIpStartPos;
	unsigned int iIpEndPos;
	unsigned int iPortEndPos;
	iIpStartPos=i_pstrURL->find("//",0);
	if(iIpStartPos!=5)
	{	
		cout<<"GetIp err:"<<iIpStartPos<<endl;
	}
	else
	{
		iIpEndPos=i_pstrURL->find(":",iIpStartPos+2);
		if(iIpEndPos==string::npos)
		{
			cout<<"GetIp1 err:"<<iIpEndPos<<endl;
		}
		else
		{
			iPortEndPos=i_pstrURL->find("/",iIpEndPos+1);
			if(iPortEndPos==string::npos)
			{
				cout<<"GetPort err:"<<iPortEndPos<<endl;
			}
			else
			{
				//IP.assign(i_URL,iIpStartPos+2,iIpEndPos-iIpStartPos-2);//����֮��Ŀ�����ǳ��������������ͷ�ͬһ���ڴ棬
				//memcpy(o_Ip,&IP,sizeof(string));//�������double free or corruption (top)�Ĵ���
				o_Ip->assign(*i_pstrURL,iIpStartPos+2,iIpEndPos-iIpStartPos-2);				
				*o_wPort=atoi(i_pstrURL->substr(iIpEndPos+1,iPortEndPos-iIpEndPos-1).c_str());
				cout<<"IP:"<<o_Ip->c_str()<<" Port:"<<i_pstrURL->substr(iIpEndPos+1,iPortEndPos-iIpEndPos-1).c_str()<<endl;
				iRet=TRUE;
			}
		}
	}
	return iRet;
}

/*****************************************************************************
-Fuction		: ConnectHandle
-Description	: һ���׽������Ӱ���������ip���������˿ں�
�ͻ���ip���ͻ��˶˿ںš�����ĸ�Ԫ��ֻҪ��һ����ͬ��
�Ǿ�����ȫ��ͬ���������ӡ�
�ͻ�����Ҫ֪�������ĸ���������������Ҫ�����������ip�Ͷ˿ں� �Ϳ���
�������ӡ�
���������ǿͻ��˷���ģ����������Դ����ӹ�������Ϣ�л�ȡ��
�ͻ��˵�ip�Ͷ˿ں�(�������ڣ� tcp/ipЭ�����һ�����ݰ�
����Դ��ַ����Ϣ(ip and port)��Ŀ�ĵ�ַ����Ϣ(ip and port),�����tcp/ip�涨��)��
����(�����������ȴ�����)ֻ�贫���������
ip(���ʹ�ñ���������IP��ipҲ���Բ�Ҫ)�Ͷ˿ںż��ɣ�
����ʹ��sockettool��������֤������ۡ�
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::Init(char *i_strURL,char * i_strFilePath)
{
    int iRet=FALSE;
    m_strURL.assign(i_strURL);
    GetIpAndPort(&m_strURL,&m_strIP,&m_wPort);
	if(FALSE==TcpServer::Init(m_strIP,m_wPort))//server socket handle bind listen
	{
	}
	else
	{
        iRet=m_MediaHandle.Init(i_strFilePath);
	}
    return iRet;
	
}

/*****************************************************************************
-Fuction		: ConnectHandle
-Description	: һ���׽������Ӱ���������ip���������˿ں�
�ͻ���ip���ͻ��˶˿ںš�����ĸ�Ԫ��ֻҪ��һ����ͬ��
�Ǿ�����ȫ��ͬ���������ӡ�
�ͻ�����Ҫ֪�������ĸ���������������Ҫ�����������ip�Ͷ˿ں� �Ϳ���
�������ӡ�
���������ǿͻ��˷���ģ����������Դ����ӹ�������Ϣ�л�ȡ��
�ͻ��˵�ip�Ͷ˿ں�(�������ڣ� tcp/ipЭ�����һ�����ݰ�
����Դ��ַ����Ϣ(ip and port)��Ŀ�ĵ�ַ����Ϣ(ip and port),�����tcp/ip�涨��)��
����(�����������ȴ�����)ֻ�贫���������
ip(���ʹ�ñ���������IP��ipҲ���Բ�Ҫ)�Ͷ˿ںż��ɣ�
����ʹ��sockettool��������֤������ۡ�
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::WaitConnectHandle()
{
    int iRet=FALSE;
    int iClientSocketFd=-1;
    T_Session tSession;
    
    iClientSocketFd = TcpServer::Accept();
    if(iClientSocketFd<0)
    {
        cout<<"TcpServer::Accept err:"<<iClientSocketFd<<endl;
    }
    else
    {//���سɹ����ʾ��һ�����Ӳ�������������Ӽ��뵽����
        memset(&tSession,0,sizeof(T_Session));
        tSession.iTrackNumber=m_iSessionCount;//��Ƶ��Ӧһ��rtp�Ự����Ƶ��Ӧһ��rtp�Ự,�����Ự�໥������һ��rtp�Ự��Ӧһ��TrackId
        m_iSessionCount++;//һ��rtsp����Ự���԰�������rtp�Ự��
        
        gettimeofday(&tSession.tCreateTime, NULL);
        tSession.iClientSocketFd = iClientSocketFd;//�Ự�������ݵ�ʱ��ʹ�����socket
        tSession.eRtspState=INIT;
        tSession.pVideoRtpSession=NULL;
        tSession.pAudioRtpSession=NULL;
        tSession.dwSessionId=0;
        pthread_mutex_lock(&m_tSessionMutex);
        m_SessionList.push_back(tSession);//�����̴߳����������
        pthread_mutex_unlock(&m_tSessionMutex);
        iRet=TRUE;
    }
    return iRet;
	
}

/*****************************************************************************
-Fuction		: SessionHandle
-Description	: SessionHandle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::SessionHandle()
{
	int iRet=FALSE;	
    //T_Session tSession={0};
    list<T_Session> ::iterator Iter;
    timeval tTimeVal={0};
    string strMsg("");
    string strSendMsg("");
    char acRecvBuf[3*1024]={0};
    int iRecvLen=0;
    
    while(1)
    {
        pthread_mutex_lock(&m_tSessionMutex);
        if(true == m_SessionList.empty())
        {
            pthread_mutex_unlock(&m_tSessionMutex);
            usleep(50*1000);
            continue;
        }

        //memset(tSession,0,sizeof(tSession));
        //cout<<"RtspCmdHandle m_SessionList size "<<m_SessionList.size()<<endl;
        for(Iter=m_SessionList.begin();Iter!=m_SessionList.end();Iter++)
        {
            strSendMsg.clear();
            strMsg.clear();
            memset(&tTimeVal,0,sizeof(tTimeVal));
            tTimeVal.tv_sec      = 0;//��ʱʱ�䣬��ʱ���ش���
            tTimeVal.tv_usec     = 10*1000;
            memset(acRecvBuf,0,sizeof(acRecvBuf));
            iRecvLen=0;
            iRet = TcpServer::Recv(acRecvBuf,&iRecvLen,sizeof(acRecvBuf),Iter->iClientSocketFd,&tTimeVal);
            if(FALSE == iRet)
            {
                cout<<"TcpServer Recv err:"<<Iter->eRtspState<<",session:"<<Iter->dwSessionId<<" will be erased"<<endl;
                if(NULL != Iter->pVideoRtpSession)
                {
                    delete Iter->pVideoRtpSession;
                    Iter->pVideoRtpSession = NULL;
                }
                if(NULL != Iter->pAudioRtpSession)
                {
                    delete Iter->pAudioRtpSession;
                    Iter->pAudioRtpSession = NULL;
                }
                m_SessionList.erase(Iter);
                break;
            }
            if(iRecvLen > 0)
            {
                strMsg.assign(acRecvBuf);
                //RtspCmdHandle(&Iter->first);
                cout<<"RtspCmdHandle State:"<<Iter->eRtspState<<" SocketFd:"<<Iter->iClientSocketFd<<endl;
                if(FALSE==RtspCmdHandle(&*Iter,&strMsg,&strSendMsg))//��iter���н�����,��ȡ��ַ������ֱ��ʹ��Iter
                {
                    cout<<"RtspCmdHandle err:"<<Iter->eRtspState<<",session:"<<Iter->dwSessionId<<" will be erased"<<endl;
                    if(NULL != Iter->pVideoRtpSession)
                        delete Iter->pVideoRtpSession;
                    if(NULL != Iter->pAudioRtpSession)
                        delete Iter->pAudioRtpSession;
                    m_SessionList.erase(Iter);
                    break;
                }
                else
                {
                    TcpServer::Send((char *)strSendMsg.c_str(),strSendMsg.length(),Iter->iClientSocketFd);
                }

            }
        }
        
        pthread_mutex_unlock(&m_tSessionMutex);
        usleep(100*1000);
    }

	return iRet;
}

/*****************************************************************************
-Fuction		: RtspCmdHandle
-Description	: RtspCmdHandle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::RtspCmdHandle(T_Session *i_ptSession,string *i_pstrMsg,string *o_pstrMsg)
{
    int iRet=FALSE;
    int iCSeq;
    unsigned int dwCmdPos;
    unsigned int dwCSeqPos;
    unsigned int dwCSeqEndPos;
    string strCmd("");    
	map<string, HandleCmd> ::iterator HandleCmdMapIter;

    if(i_pstrMsg->length() <= 0)
    {
        cout<<"RtspCmdHandle i_pstrMsg err"<<endl;
        return iRet;
    }

    dwCmdPos=i_pstrMsg->find("rtsp:");
    if(dwCmdPos==string::npos)
    {
        cout<<"RtspCmdHandle can not find rtsp:"<<dwCmdPos<<endl;
    }
    else
    {
        strCmd.assign(i_pstrMsg->substr(0,dwCmdPos-1));
        
        dwCSeqPos=i_pstrMsg->find("CSeq:");
        if(dwCSeqPos==string::npos)
        {
            cout<<"RtspCmdHandle can not find CSeq:"<<dwCSeqPos<<endl;
        }
        else
        {
            dwCSeqEndPos=i_pstrMsg->find("\r\n",dwCSeqPos);
            if(dwCSeqEndPos==string::npos)
            {
                cout<<"RtspCmdHandle can not find CSeq:"<<dwCSeqEndPos<<endl;
            }
            else
            {
            
                iCSeq = atoi(i_pstrMsg->substr(dwCSeqPos+strlen("CSeq:"),dwCSeqEndPos-(dwCSeqPos+strlen("CSeq:"))).c_str());
            
                HandleCmdMapIter=m_HandleCmdMap.find(strCmd.c_str());
                if(HandleCmdMapIter == m_HandleCmdMap.end())
                {
                    cout<<"Can not find:"<<strCmd<<endl;
                }
                else
                {
                    iRet=(this->*(HandleCmdMapIter->second))(i_ptSession,i_pstrMsg,iCSeq,o_pstrMsg);
                    if(FALSE==iRet)
                    {
                    }
                    else
                    {
                    }
                }
            }    
        }    
    }    
    
    return iRet;
}

/*****************************************************************************
-Fuction		: HandleCmdOPTIONS
-Description	: HandleCmdOPTIONS
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::HandleCmdOPTIONS(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg)
{
    int iRet=FALSE;
	stringstream Msg("");
	
	Msg<<RTSP_VERSION<<RTSP_RESPONSE_OK<<"\r\n";
	Msg << "CSeq: " << i_iCSeq<< "\r\n";
    Msg <<GetDateHeader();
	Msg << "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE"<< "\r\n";//�����Ż�Ϊ��ʹ��m_astrCmd�ַ�������
	Msg << "\r\n";

    iRet = TRUE;
	o_pstrMsg->assign(Msg.str());

    return iRet;
}

/*****************************************************************************
-Fuction		: HandleCmdDESCRIBE
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::HandleCmdDESCRIBE(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg)
{
    int iRet=FALSE;
	stringstream Msg("");
    string strSDP("");
    
    if(FALSE == GenerateSDP(i_ptSession,&strSDP))
    {
        Msg<<RTSP_VERSION<<RTSP_RESPONSE_ERROR_404<<"\r\n";
        Msg << "CSeq: " << i_iCSeq<< "\r\n";
        Msg <<GetDateHeader();
        Msg << "\r\n";
    }
    else
    {
        Msg<<RTSP_VERSION<<RTSP_RESPONSE_OK<<"\r\n";
        Msg << "CSeq: " << i_iCSeq<< "\r\n";
        Msg <<GetDateHeader();
        Msg << "Content-Base: "<<m_strURL<< "/\r\n";
        Msg <<"Content-Type: application/sdp\r\n";
        Msg << "Content-Length: "<<strSDP.length()<< "\r\n";
        Msg << "\r\n";

        Msg <<strSDP;
    }

    iRet = TRUE;
	o_pstrMsg->assign(Msg.str());

    return iRet;
}

/*****************************************************************************
-Fuction		: HandleCmdSETUP
-Description	: ����ȷ��ת����ƣ�����RTSP�Ự��
�ͻ����ܹ�����һ��SETUP����Ϊ���ڲ��ŵ�ý�����ı䴫�����������������ͬ����Щ�����ĸı䡣
���ǲ�ͬ�⣬��������Ӧ����"455 Method Not Valid In This State"��
Request�е�Transportͷ�ֶ�ָ���˿ͻ��˿ɽ��ܵ����ݴ��������Response�е�Transport ͷ�ֶΰ������ɷ�����ѡ���Ĵ������

������ý��Ự����֪RTSP�����׼����Դ���Դ�������һ������
RTSP/<RTSP VERSION><BLANK><STATE ID><BLANK><STATE DESCRIBE>\r\nCSeq:<BLANK><COMMAND SEQUENCE>\r\n<OTHER>\r\n<SESSION ID>\r\n\r\n
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::HandleCmdSETUP(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg)
{
    int iRet=FALSE;
	stringstream Msg("");
	const char *strPatten=NULL;
	regmatch_t atMatch[MAX_MATCH_NUM];
	string strStreamTransportProtocol("");
    string strCastMethod("");
    string strRtpPort("");
	string strRtcpPort("");
	string strTrackID("");
    T_MediaInfo tMediaInfo;
	
    if(i_ptSession->eRtspState!=INIT && i_ptSession->eRtspState!=SETUP_PLAY_READY)
    {
        Msg<<RTSP_VERSION<<RTSP_RESPONSE_METHOD_NOT_VALID_455<<"\r\n";
        Msg << "CSeq: " << i_iCSeq<< "\r\n";
        Msg <<GetDateHeader();
        Msg << "\r\n";
    }
    else
    {
        strPatten=".*Transport: ([A-Z/]+);([a-z]+);client_port=([0-9]+)-([0-9]+).*";
        //strPatten=".*Transport: "RTSP_TRANSPORT_BASE"[A-Z]+;[a-z]+;client_port=[0-9]+-[0-9]+.*";//Transport: RTP/AVP/UDP;unicast;client_port=10330-10331
        memset(atMatch,0,sizeof(atMatch));//ʹ��RTP���䣨RTP/AVP/UDP,RTPoverUDP,һ��ΪRTP+UDP�������䷽ʽΪ������unicast����RTP��RTCP�Ķ˿ںŷֱ�Ϊ10330��10331��client_port=10330-10331��
        if(REG_NOERROR!=Tools::Instance()->Regex(strPatten,(char *)i_pstrMsg->c_str(),atMatch))
        {
            Msg<<RTSP_VERSION<<RTSP_RESPONSE_UNSUPPORTED_TRANSPORT_461<<"\r\n";
            Msg << "CSeq: " << i_iCSeq<< "\r\n";
            Msg <<GetDateHeader();
            Msg << "\r\n";
        }
        else
        {
            strStreamTransportProtocol.assign(*i_pstrMsg,atMatch[1].rm_so,atMatch[1].rm_eo-atMatch[1].rm_so);//*i_pstrMsg�ᱨ��
            strCastMethod.assign(*i_pstrMsg,atMatch[2].rm_so,atMatch[2].rm_eo-atMatch[2].rm_so);
            strRtpPort.assign(*i_pstrMsg,atMatch[3].rm_so,atMatch[3].rm_eo-atMatch[3].rm_so);
            strRtcpPort.assign(*i_pstrMsg,atMatch[4].rm_so,atMatch[4].rm_eo-atMatch[4].rm_so);
            cout<<"strStreamTransportProtocol "<<strStreamTransportProtocol<<endl;
            cout<<"strCastMethod "<<strCastMethod<<endl;
            cout<<"strRtpPort "<<strRtpPort<<endl;
            cout<<"strRtcpPort "<<strRtcpPort<<endl;
            
            strPatten=".*/([A-Za-z0-9]+) "RTSP_VERSION".*";//SETUP rtsp://127.0.0.1/ansersion/track1 RTSP/1.0��һ��setup����ֻ��һ��trackid
            memset(atMatch,0,sizeof(atMatch));//ʹ��RTP���䣨RTP/AVP/UDP,RTPoverUDP,һ��ΪRTP+UDP�������䷽ʽΪ������unicast����RTP��RTCP�Ķ˿ںŷֱ�Ϊ10330��10331��client_port=10330-10331��
            if(REG_NOERROR!=Tools::Instance()->Regex(strPatten,(char *)i_pstrMsg->c_str(),atMatch))
            {
                Msg<<RTSP_VERSION<<RTSP_RESPONSE_UNSUPPORTED_TRANSPORT_461<<"\r\n";
                Msg << "CSeq: " << i_iCSeq<< "\r\n";
                Msg <<GetDateHeader();
                Msg << "\r\n";
            }
            else
            {
                strTrackID.assign(*i_pstrMsg,atMatch[1].rm_so,atMatch[1].rm_eo-atMatch[1].rm_so);
                cout<<"strTrackID "<<strTrackID<<endl;
                char strCurrentTrackID[50]={0};                
                //int iTrackIdType=-1;//-1 err,0 video,1 audio
                bool blTrackIdIsVideo=false;
                bool blTrackIdIsAudio=false;
                snprintf(strCurrentTrackID,sizeof(strCurrentTrackID),"track%d",2*i_ptSession->iTrackNumber+1);//video trackid
                if(string::npos==strTrackID.find(strCurrentTrackID))
                {
                    cout<<"Video TrackID err:"<<strTrackID<<" TrackId:"<<2*i_ptSession->iTrackNumber+1<<endl;
                }
                else
                {
                    blTrackIdIsVideo=true;
                }
                memset(strCurrentTrackID,0,sizeof(strCurrentTrackID));
                snprintf(strCurrentTrackID,sizeof(strCurrentTrackID),"track%d",2*i_ptSession->iTrackNumber+2);//audio trackid
                if(string::npos==strTrackID.find(strCurrentTrackID))
                {
                    cout<<"Audio TrackID err:"<<strTrackID<<" TrackId:"<<2*i_ptSession->iTrackNumber+2<<endl;
                    //Msg<<RTSP_VERSION<<RTSP_RESPONSE_UNSUPPORTED_TRANSPORT_461<<"\r\n";
                    //Msg << "CSeq: " << i_iCSeq<< "\r\n";//���Բ�֧����Ƶ����˲����ش���
                    //Msg <<GetDateHeader();//����Ͳ��ܼ���Щ��Ϣ������ֻ������Ƶʱ��
                    //Msg << "\r\n";//����ᵼ���޷�����
                }
                else
                {
                    blTrackIdIsAudio=true;
                }
                string strClientIP(Tools::Instance()->UseSocketGetIP(i_ptSession->iClientSocketFd));
                int iRtpSocket=0,iRtcpSocket=0;
                if(false==blTrackIdIsAudio && false == blTrackIdIsVideo)
                {
                    iRet=FALSE;
                }
                else if(true == blTrackIdIsVideo)
                {
                    if(NULL != i_ptSession->pVideoRtpSession)
                        delete i_ptSession->pVideoRtpSession;
                        
                    memset(&tMediaInfo,0,sizeof(T_MediaInfo));
                    m_MediaHandle.GetMediaInfo(&tMediaInfo);
                    switch(tMediaInfo.eVideoEncType)
                    {
                        case VIDEO_ENCODE_TYPE_H264:
                        case VIDEO_ENCODE_TYPE_H265:
                            i_ptSession->pVideoRtpSession=new RtpSession(RTP_PAYLOAD_VIDEO,tMediaInfo.dwVideoSampleRate);//���ݱ����ʽ����
                        break;
                        default :
                        break;
                    }
                    if(NULL != i_ptSession->pVideoRtpSession)
                    {
                        if(strStreamTransportProtocol.find(RTSP_TRANSPORT_RTP_OVER_TCP)!=string::npos)    
                            iRet=i_ptSession->pVideoRtpSession->Init(true,m_strIP,strClientIP,(unsigned short)atoi(strRtpPort.c_str()),(unsigned short)atoi(strRtcpPort.c_str()));
                        else
                            iRet=i_ptSession->pVideoRtpSession->Init(false,m_strIP,strClientIP,(unsigned short)atoi(strRtpPort.c_str()),(unsigned short)atoi(strRtcpPort.c_str()));
                        iRtpSocket = i_ptSession->pVideoRtpSession->GetRtpSocket();
                        iRtcpSocket =i_ptSession->pVideoRtpSession->GetRtcpSocket();
                    }
                }
                else
                {
                    if(NULL != i_ptSession->pAudioRtpSession)
                        delete i_ptSession->pAudioRtpSession;
                        
                    memset(&tMediaInfo,0,sizeof(T_MediaInfo));
                    m_MediaHandle.GetMediaInfo(&tMediaInfo);
                    switch(tMediaInfo.eAudioEncType)
                    {
                        case AUDIO_ENCODE_TYPE_AAC:
                        case AUDIO_ENCODE_TYPE_G711U:
                        case AUDIO_ENCODE_TYPE_G711A:
                            i_ptSession->pAudioRtpSession=new RtpSession(RTP_PAYLOAD_AUDIO,tMediaInfo.dwAudioSampleRate);//���ݱ����ʽ����
                        break;
                        default :
                        break;
                    }
                    if(NULL != i_ptSession->pAudioRtpSession)
                    {
                        if(strStreamTransportProtocol.find(RTSP_TRANSPORT_RTP_OVER_TCP)!=string::npos)    
                            iRet=i_ptSession->pAudioRtpSession->Init(true,m_strIP,strClientIP,(unsigned short)atoi(strRtpPort.c_str()),(unsigned short)atoi(strRtcpPort.c_str()));
                        else
                            iRet=i_ptSession->pAudioRtpSession->Init(false,m_strIP,strClientIP,(unsigned short)atoi(strRtpPort.c_str()),(unsigned short)atoi(strRtcpPort.c_str()));
                        iRtpSocket = i_ptSession->pAudioRtpSession->GetRtpSocket();
                        iRtcpSocket =i_ptSession->pAudioRtpSession->GetRtcpSocket();
                    }
                }
                if(FALSE==iRet)
                {
                    cout<<"i_ptSession->pRtpSession->Init err"<<endl;
                    Msg<<RTSP_VERSION<<RTSP_RESPONSE_BAD_400<<"\r\n";
                    Msg << "CSeq: " << i_iCSeq<< "\r\n";
                    Msg <<GetDateHeader();
                    Msg << "\r\n";
                }
                else
                {
                    char strSession[50]={0};
                    const char * strSessionFmt="Session: %08x;timeout=20\r\n";//client��Ҫ�ֺ�
                    i_ptSession->eRtspState=SETUP_PLAY_READY;
                    //RTSP/<RTSP VERSION><BLANK><STATE ID><BLANK><STATE DESCRIBE>\r\nCSeq:<BLANK><COMMAND SEQUENCE>\r\n<OTHER>\r\n<SESSION ID>\r\n\r\n
                    Msg<<RTSP_VERSION<<RTSP_RESPONSE_OK<<"\r\n";
                    Msg << "CSeq: " << i_iCSeq<< "\r\n";
                    Msg <<GetDateHeader();
                    //Transport: RTP/AVP;unicast;destination=127.0.0.1;source=127.0.0.1;client_port=10330-10331;server_port=6970-6971
                    if(strStreamTransportProtocol.find(RTSP_TRANSPORT_RTP_OVER_TCP)!=string::npos)//multicast streams can't be sent via TCP ����������������жϲ����ش�������ͻ���    
                    {
                        Msg <<"Transport: RTP/AVP/TCP;unicast;destination="<<strClientIP<<";source="<<m_strIP<<";client_port="<<strRtpPort<<"-"<<
                        strRtcpPort<<";server_port="<<Tools::Instance()->UseSocketGetPort(iRtpSocket)<<"-"<<Tools::Instance()->UseSocketGetPort(iRtcpSocket)<< "\r\n";
                    }
                    else
                    {
                        Msg <<"Transport: RTP/AVP;unicast;destination="<<strClientIP<<";source="<<m_strIP<<";client_port="<<strRtpPort<<"-"<<
                        strRtcpPort<<";server_port="<<Tools::Instance()->UseSocketGetPort(iRtpSocket)<<"-"<<Tools::Instance()->UseSocketGetPort(iRtcpSocket)<< "\r\n";
                    }
                    i_ptSession->dwSessionId=Tools::Instance()->GetRandom();//<SESSION ID>������˽�������Դ��ͨ���ñ�ʶ������ý������Դ��
                    snprintf(strSession,sizeof(strSession),strSessionFmt,i_ptSession->dwSessionId);//"Session: SESSION ID"��PLAY�����Դ�Ϊ����
                    Msg <<strSession;
                    Msg << "\r\n";
                }
            }
        }
    }

    iRet = TRUE;
	o_pstrMsg->assign(Msg.str());

    return iRet;
}

/*****************************************************************************
-Fuction		: HandleCmdPLAY
-Description	: 

PLAY������֪������ͨ��SETUP��ָ���Ļ��ƿ�ʼ�������� ��
����δ�յ�SETUP����ĳɹ�Ӧ��֮ǰ���ͻ��˲����Է���PLAY����
PLAY������������ʱ�䣨normal play time����λ��ָ����Χ����ʼ����
���Ҵ���������ֱ�����ŷ�Χ������
PLAY������ܱ��ܵ�����pipelined��������������У�queued����
���������뽫PLAY����ŵ�����������ִ�С�Ҳ����˵��
��һ��PLAY������Ҫ�ȴ�ǰһ��PLAY������ɲ��ܵõ�ִ��

-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::HandleCmdPLAY(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg)
{
    int iRet=FALSE;
	stringstream Msg("");
	
	if (i_ptSession->eRtspState!= SETUP_PLAY_READY && i_ptSession->eRtspState != PLAYING) 
	{//��ü��ϱȶ�session id
        Msg<<RTSP_VERSION<<RTSP_RESPONSE_METHOD_NOT_VALID_455<<"\r\n";
        Msg << "CSeq: " << i_iCSeq<< "\r\n";
        Msg <<GetDateHeader();
        Msg << "\r\n";
        cout<<"HandleCmdPLAY state err:"<<i_ptSession->eRtspState<<endl;
	}
    else
    {
        char strSession[50]={0};
        const char * strSessionFmt="Session: %08x";
        snprintf(strSession,sizeof(strSession),strSessionFmt,i_ptSession->dwSessionId);
        
        Msg<<RTSP_VERSION<<RTSP_RESPONSE_OK<<"\r\n";//Range: npt= ����Ϣ��������
        Msg << "CSeq: " << i_iCSeq<< "\r\n";//Rangeͷ���ܰ���һ��ʱ��������ò�����UTC��ʽָ���˲��ſ�ʼ��ʱ��,
        Msg <<strSession<< "\r\n";//ʱ�����������������ͬ���Ӳ�ͬ����Դ��ȡ����������
        Msg <<GetDateHeader();//��������ָ��ʱ����յ���Ϣ����ô����������ʼ,
        Msg << "\r\n";//����Rangeͷ��PLAY����Ҳ�ǺϷ��ġ�����ý������ͷ��ʼ���ţ�ֱ��ý��������ͣ��
        //���ý����ͨ��PAUSE��ͣ��ý�������佫����ͣ�㣨the pause point�����¿�ʼ
        //���ý�������ڲ��ţ���ô����һ��PLAY���󽫲����������ã�ֻ�ǿͻ��˿����ô������Է������Ƿ���
        if (i_ptSession->eRtspState != PLAYING)
            i_ptSession->eRtspState = PLAYING;
    }


    iRet = TRUE;
	o_pstrMsg->assign(Msg.str());
    return iRet;
}

/*****************************************************************************
-Fuction		: HandleCmdPAUSE
-Description	: 
PAUSE��������ý�����������ʱ�жϡ�
�������URL��ָ���˾����ý������
��ôֻ�и�ý�����Ĳ��źͼ�¼����ͣ��halt��
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::HandleCmdPAUSE(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg)
{
    int iRet=FALSE;
	stringstream Msg("");
	
	if (i_ptSession->eRtspState != SETUP_PLAY_READY && i_ptSession->eRtspState != PLAYING) 
	{
        Msg<<RTSP_VERSION<<RTSP_RESPONSE_METHOD_NOT_VALID_455<<"\r\n";
        Msg << "CSeq: " << i_iCSeq<< "\r\n";
        Msg <<GetDateHeader();
        Msg << "\r\n";
        cout<<"HandleCmdPAUSE state err:"<<i_ptSession->eRtspState<<endl;
	}
    else
    {
        Msg<<RTSP_VERSION<<RTSP_RESPONSE_OK<<"\r\n";//Rangeͷ�ݲ�����
        Msg << "CSeq: " << i_iCSeq<< "\r\n";//PAUSE�����п��ܰ���һ��Rangeͷ����ָ����ʱý������ͣ��
        Msg <<GetDateHeader();//���ǳ����ʱ��Ϊ��ͣ�㣨pause point������ͷ�������һ����ȷ��ֵ��������һ��ʱ�䷶Χ��ý��������������ʱ�����ó���ͣ��
        Msg << "\r\n";//���Rangeͷָ����һ��ʱ�䳬�����κ�һ����ǰ�����PLAY���󣬽����ش���"457 Invalid Range" ��
        //���Rangeͷȱʧ����ô���յ���ͣ��Ϣ��ý�������������жϣ�������ͣ�����óɵ�ǰ��������ʱ�䡣

        if (i_ptSession->eRtspState != SETUP_PLAY_READY)
            i_ptSession->eRtspState = SETUP_PLAY_READY;
    }

    iRet = TRUE;
	o_pstrMsg->assign(Msg.str());

    return iRet;
}

/*****************************************************************************
-Fuction		: HandleCmdTEARDOWN
-Description	: HandleCmdTEARDOWN
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::HandleCmdTEARDOWN(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg)
{
    int iRet=FALSE;
	stringstream Msg("");

    if(NULL !=i_ptSession->pVideoRtpSession)
        i_ptSession->pVideoRtpSession->Close();
    if(NULL !=i_ptSession->pAudioRtpSession)
        i_ptSession->pAudioRtpSession->Close();
    
    if(NULL !=i_ptSession->pVideoRtpSession)
    {
        delete i_ptSession->pVideoRtpSession;
        i_ptSession->pVideoRtpSession = NULL;
    }
    if(NULL != i_ptSession->pAudioRtpSession)
    {
        delete i_ptSession->pAudioRtpSession;
        i_ptSession->pAudioRtpSession = NULL;
    }
        
    i_ptSession->eRtspState=INIT;

	
	Msg<<RTSP_VERSION<<RTSP_RESPONSE_OK<<"\r\n";
	Msg << "CSeq: " << i_iCSeq<< "\r\n";
    Msg <<GetDateHeader();
	Msg << "\r\n";

    cout<<"Recv CmdTEARDOWN"<<endl;
    iRet = FALSE;//TEARDOWN ��ɾ����rtsp�Ự
	o_pstrMsg->assign(Msg.str());

    return iRet;
}





















/*****************************************************************************
-Fuction		: GenerateSDP
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::GenerateSDP(T_Session *i_ptSession,string *o_pstrSDP)
{
    int iRet=FALSE;
    char *pcSdpBuf=new char[2000];
    string strMediaSDP("");
    string strDescriptionSDP(RTSP_SERVER_NAME);
    
    memset(pcSdpBuf,0,2000);
    strDescriptionSDP.append(RTSP_SERVER_VERSION);

    if(NULL == o_pstrSDP)
    {
        cout<<"GenerateSDP err o_pstrSDP NULL"<<endl;
    }
    else
    {
        //Generate the media SDP
        iRet=GenerateMediaSDP(i_ptSession,&strMediaSDP);
        if(FALSE== iRet)
        {
            cout<<"GenerateMediaSDP err"<<endl;
        }
        else
        {
            //Generate the Session SDP
            const char * strSdpPrefixFmt =
                 "v=0\r\n"                   //V=0     ;Version ������SDPЭ��İ汾
                 "o=- %ld%06ld %d IN IP4 %s\r\n"//o=<username><session id> <version> <network type> <address type><address> Origin ,�����˻Ự�ķ�������Ϣ
                 "s=%s\r\n"                 //s=<sessionname> ;������Session Name
                 "i=%s\r\n"                 //i=<sessiondescription> ; Information ����Session��һЩ��Ϣ
                 "t=0 0\r\n"                //t=<start time><stop time> ;Time
                 "a=tool:%s%s\r\n"          //a=<attribute>     ; Attribute
                 "a=type:broadcast\r\n"     //a=<attribute>:<value>
                 "a=control:*\r\n"          //"a=control:%s\r\n", uri ? uri : "*"
                 "a=range:npt=0-\r\n"       // a=range: line duration
                 "a=x-qt-text-nam:%s\r\n"
                 "a=x-qt-text-inf:%s\r\n"
                 "%s";                      // Then, add the (media-Sdp)
            snprintf(pcSdpBuf,2000,strSdpPrefixFmt,
                  i_ptSession->tCreateTime.tv_sec, i_ptSession->tCreateTime.tv_usec, // o= <session id>
                  1,                                                        // o= <version> // (needs to change if params are modified)
                  m_strIP.c_str(),//Tools::Instance()->GetLocalIP(),                                             // o= <address>
                  strDescriptionSDP.c_str(),                                // s= <description>
                  strDescriptionSDP.c_str(),                                // i= <info>
                  RTSP_SERVER_NAME, RTSP_SERVER_VERSION,                    // a=tool:
                  strDescriptionSDP.c_str(),                                // a=x-qt-text-nam: line
                  strDescriptionSDP.c_str(),                                // a=x-qt-text-inf: line
                  strMediaSDP.c_str());
                  
            if(strlen(pcSdpBuf)!=0)
            {
                o_pstrSDP->assign(pcSdpBuf);
                iRet=TRUE;
            }
        }
        
    }
    delete [] pcSdpBuf;
    return iRet;
}


/*****************************************************************************
-Fuction		: GenerateMediaSDP
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::GenerateMediaSDP(T_Session *i_ptSession,string *o_pstrMediaSDP)
{
    int iRet=FALSE;
    unsigned short wPortNumForSDP=0;
    unsigned char ucRtpPayloadType=0;
    char *pstrRtpPayloadFormatName=NULL;
    unsigned int dwRtpTimestampFrequency=0;
    
    char *pcAuxSdpBuf=new char[300];
    char *pcMediaSdpBuf=new char[1000];
    char *pcMediaSdq=NULL;
    T_MediaInfo tMediaInfo;
    int i =0;
    
    memset(pcAuxSdpBuf,0,300);
    memset(pcMediaSdpBuf,0,1000);
    if(NULL == o_pstrMediaSDP)
    {
        cout<<"GenerateMediaSDP err o_pstrMediaSDP NULL"<<endl;
        return iRet;
    }
    
    //Generate the media SDP
    const char * strMediaSdpFmt =
         "m=%s %u RTP/AVP %d\r\n"  // m= <media><port>
         "c=IN IP4 0.0.0.0\r\n"
         "b=AS:%u\r\n"             // b=AS:<bandwidth>
         "a=rtpmap:%d %s/%d%s\r\n"  // a=rtpmap:... (if present) rtpmapLine
         "a=range:npt=0-\r\n"      // a=range:... (if present)
         "%s"                      //���������������� "a=fmtp" ��
         "a=control:track%d\r\n";  //"a=control:track1"ָ���˷��ʸ���ý��ķ�ʽ���Ǻ���SETUP�������Ҫ����

    memset(&tMediaInfo,0,sizeof(T_MediaInfo));
    m_MediaHandle.GetMediaInfo(&tMediaInfo);
    if(STREAM_TYPE_VIDEO_STREAM == tMediaInfo.eStreamType ||
      STREAM_TYPE_MUX_STREAM == tMediaInfo.eStreamType)
    {
        //�����ϱ�����ȷָ��PTֵ�ĸ������ͣ�����Щ�����������ڵ����Ľ���û�о����PTֵ��
        ucRtpPayloadType =RTP_PAYLOAD_VIDEO;//ֻ��ʹ�ö�̬��dynamic��PTֵ����96��127�������Ϊʲô����ձ�ָ��H264��PTֵΪ96��
        for(i = 0;i<(sizeof(g_atVideoEncTypeToSdpEncName)/sizeof(T_VideoEncTypeToSdpEncName));i++)
        {
            if(tMediaInfo.eVideoEncType == g_atVideoEncTypeToSdpEncName[i].eVideoEncodeType)
            {
                break;
            }
        }
        if(i>=(sizeof(g_atVideoEncTypeToSdpEncName)/sizeof(T_VideoEncTypeToSdpEncName)))
        {
            cout<<"err tMediaInfo.eVideoEncType:"<<tMediaInfo.eVideoEncType<<endl;
        }
        else
        {
            pstrRtpPayloadFormatName =(char *)g_atVideoEncTypeToSdpEncName[i].srtSdpEncName;
        }
        dwRtpTimestampFrequency =tMediaInfo.dwVideoSampleRate;
        
        // begin Generate a new "a=fmtp:" line each time, using our SPS and PPS (if we have them),
        T_VideoEncodeParam tVideoEncodeParamWEB;// abSPS_WEB "WEB" means "Without Emulation Bytes"
        memset(&tVideoEncodeParamWEB,0,sizeof(T_VideoEncodeParam));
        m_MediaHandle.GetVideoEncParam(&tVideoEncodeParamWEB);
        if (tVideoEncodeParamWEB.iSizeOfSPS< 4) 
        { // Bad SPS size => assume our source isn't ready
            cout<<"Bad SPS size:"<<tVideoEncodeParamWEB.iSizeOfSPS<<endl;
        }
        else
        {
            unsigned int dwProfileLevelId = (tVideoEncodeParamWEB.abSPS[1]<<16) | (tVideoEncodeParamWEB.abSPS[2]<<8) | tVideoEncodeParamWEB.abSPS[3];
            char * strSPS_Base64 = base64Encode((char*)tVideoEncodeParamWEB.abSPS, tVideoEncodeParamWEB.iSizeOfSPS);
            char * strPPS_Base64 = base64Encode((char*)tVideoEncodeParamWEB.abPPS, tVideoEncodeParamWEB.iSizeOfPPS);
            const char * strAuxSdpFmt =
                 "a=fmtp:%d packetization-mode=1"//��ʾ֧�ֵķ��ģʽ.�� packetization-mode ��ֵΪ 1 ʱ����ʹ�÷ǽ���(non-interleaved)���ģʽ.
                 ";profile-level-id=%06X"   //�����������ָʾ H.264 ���� profile ���ͺͼ���. �� Base16(ʮ������) ��ʾ�� 3 ���ֽ�. ��һ���ֽڱ�ʾ H.264 �� Profile ����, �������ֽڱ�ʾ H.264 �� Profile ����:
                 ";sprop-parameter-sets=%s,%s\r\n";//����H264��SPS��PPS��Base64����         
            snprintf(pcAuxSdpBuf,300,strAuxSdpFmt,
                  ucRtpPayloadType,        //sps_pps��Ҫ��h264�ļ�(Դ)�л�ȡ,����������������һ������(gop)
                  dwProfileLevelId,        //��Ӧ����һ����������ж�Ӧ���ж��,Ҳ����˵������Ƶ���Ĺ������������ǲ��ϸ��µģ����б任�˾Ͳ�ͬ��
                  strSPS_Base64,           //������Ҫ�и�һֱ��ִ�еĺ���ȥ����������������,���Ǵ���ֻ��Ҫ�õ���һ��sps��pps
                  strPPS_Base64);          //����H264��SPS��PPS��Base64����         
            delete[] strSPS_Base64;
            delete[] strPPS_Base64;
            // end Generate a new "a=fmtp:" 
            
            snprintf(pcMediaSdpBuf,1000,strMediaSdpFmt,
                  "video",              // m= <media>
                  wPortNumForSDP,       // m= <port>
                  ucRtpPayloadType,     // m= <fmt list>
                  m_dwBandwidth*10,        // b=AS:<bandwidth>// If bandwidth is specified, use it and add 5% for RTCP overhead. Otherwise make a guess at 500 kbps.
                  ucRtpPayloadType,     // a=rtpmap:... (if present):
                  pstrRtpPayloadFormatName,// rtpPayloadType�������� rtpPayloadFormatName�������� TimestampFrequencyʱ��Ƶ��encodingParamsPart
                  dwRtpTimestampFrequency,
                  "",                   //encodingParamsPart
                  pcAuxSdpBuf,          // optional extra SDP line
                  2*i_ptSession->iTrackNumber+1); // a=control:<track-id> //track-idΨһ���ɣ�����Ҫ����������ʹ��2n+1�ļ��㷽ʽ
                  //��Ƶ��Ӧһ��rtp�Ự����Ƶ��Ӧһ��rtp�Ự,�����Ự�໥������һ��rtp�Ự��Ӧһ��TrackId
                  //һ��rtsp����Ự���԰�������rtp�Ự��
        }
    }
    if(STREAM_TYPE_AUDIO_STREAM == tMediaInfo.eStreamType ||
      STREAM_TYPE_MUX_STREAM == tMediaInfo.eStreamType)
    {
        pcMediaSdq = pcMediaSdpBuf+strlen(pcMediaSdpBuf);
        
        ucRtpPayloadType =RTP_PAYLOAD_AUDIO;//https://tools.ietf.org/html/rfc3551#page-32
        for(i = 0;i<(sizeof(g_atAudioEncTypeToSdpEncName)/sizeof(T_AudioEncTypeToSdpEncName));i++)
        {
            if(tMediaInfo.eAudioEncType == g_atAudioEncTypeToSdpEncName[i].eAudioEncodeType)
            {
                break;
            }
        }
        if(i>=(sizeof(g_atAudioEncTypeToSdpEncName)/sizeof(T_AudioEncTypeToSdpEncName)))
        {
            cout<<"err tMediaInfo.eVideoEncType:"<<tMediaInfo.eAudioEncType<<endl;
        }
        else
        {
            pstrRtpPayloadFormatName =(char *)g_atAudioEncTypeToSdpEncName[i].srtSdpEncName;
        }
        dwRtpTimestampFrequency = tMediaInfo.dwAudioSampleRate;
        snprintf(pcMediaSdq,1000-strlen(pcMediaSdpBuf),strMediaSdpFmt,
              "audio",              // m= <media>
              wPortNumForSDP,       // m= <port>
              ucRtpPayloadType,     // m= <fmt list>
              m_dwBandwidth,        // b=AS:<bandwidth>// If bandwidth is specified, use it and add 5% for RTCP overhead. Otherwise make a guess at 500 kbps.
              ucRtpPayloadType,     // a=rtpmap:... (if present):
              pstrRtpPayloadFormatName,// rtpPayloadType�������� rtpPayloadFormatName�������� TimestampFrequencyʱ��Ƶ��encodingParamsPart
              dwRtpTimestampFrequency,
              SDP_AUDIO_CHANNEL_NUM,         //encodingParamsPart
              "",                   // optional extra SDP line
              2*i_ptSession->iTrackNumber+2); // a=control:<track-id> //track-idΨһ���ɣ�����Ҫ����������ʹ��2n+2�ļ��㷽ʽ
              //��Ƶ��Ӧһ��rtp�Ự����Ƶ��Ӧһ��rtp�Ự,�����Ự�໥������һ��rtp�Ự��Ӧһ��TrackId
              //һ��rtsp����Ự���԰�������rtp�Ự��
    }
    
    if(strlen(pcMediaSdpBuf)!=0)
    {
        o_pstrMediaSDP->assign(pcMediaSdpBuf);
        iRet=TRUE;
    }
    else
    {
        cout<<"GenerateMediaSDP err len=0"<<tMediaInfo.eStreamType<<endl;
    }

    delete [] pcAuxSdpBuf;
    delete [] pcMediaSdpBuf;
    return iRet;
}

/*****************************************************************************
-Fuction		: GetDateHeader
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
const char * RtspServer::GetDateHeader()
{
    static char DateBuf[200];
    time_t tTime = time(NULL);

    memset(DateBuf,0,sizeof(DateBuf));
    strftime(DateBuf, sizeof(DateBuf), "Date: %a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&tTime));
    

    return DateBuf;
}

/*****************************************************************************
-Fuction		: SessionHandleThread
-Description	: SessionHandleThread
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void * RtspServer::SessionHandleThread(void *pArg)
{
	if( NULL != pArg )
	{
		RtspServer *pRtspServer = ( RtspServer * )pArg;
		pRtspServer->SessionHandle();
	}

	return NULL;
}


/*****************************************************************************
-Fuction		: RtspStreamHandle
-Description	: rtsp��ý�����ݴ�������
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspServer::RtspStreamHandle()
{
    int iRet=FALSE;
    list<T_Session> ::iterator Iter;
    T_MediaFrameParam tMediaFrameParam;
    int iPacketNum=0;
    unsigned char *ppbPacketBuf[RTP_MAX_PACKET_NUM]={0};
    int aiEveryPacketLen[RTP_MAX_PACKET_NUM]={0};
    int i,j;
    unsigned char *pbNaluStartPos = NULL;
    unsigned int dwNaluOffset = 0;
    T_RtpPacketParam tRtpPacketParam;
    unsigned int dwDiffTimestamp = 0;
    struct timespec tTimeSpec;
    int iDelayTimeUs = 0;
    T_MediaInfo tMediaInfo;

    memset(&tMediaFrameParam,0,sizeof(T_MediaFrameParam));
    tMediaFrameParam.pbFrameBuf = new unsigned char[FRAME_BUFFER_MAX_SIZE];
    if(NULL == tMediaFrameParam.pbFrameBuf)
    {
        cout<<"pbVideoBuf malloc NULL"<<endl;
        return iRet;
    }
    iRet=m_pRtpPacket->Init(ppbPacketBuf, RTP_MAX_PACKET_NUM);
    if(FALSE == iRet)
    {
        cout<<"m_pRtpPacket->Init NULL"<<endl;
        delete [] tMediaFrameParam.pbFrameBuf;
        return iRet;
    }
    memset(&tMediaInfo,0,sizeof(T_MediaInfo));
    m_MediaHandle.GetMediaInfo(&tMediaInfo);
    while(1)
    {
        pthread_mutex_lock(&m_tSessionMutex);
        if(true == m_SessionList.empty())
        {
            pthread_mutex_unlock(&m_tSessionMutex);
            usleep(50*1000);
            continue;
        }
        if(tMediaFrameParam.eFrameType == FRAME_TYPE_UNKNOW)
        {
            memset(tMediaFrameParam.pbFrameBuf,0,FRAME_BUFFER_MAX_SIZE);
            tMediaFrameParam.iFrameBufMaxLen = FRAME_BUFFER_MAX_SIZE;
            iRet=m_MediaHandle.GetNextFrame(&tMediaFrameParam);
            if(FALSE == iRet)
            {
                pthread_mutex_unlock(&m_tSessionMutex);
                sleep(1);
                cout<<"m_MediaHandle.GetNextFrame err"<<endl;
                continue;
            }
        }
        cout<<"m_MediaHandle.GetNextFrame"<<tMediaFrameParam.dwNaluCount<<endl;
        iRet = FALSE;
        for(Iter=m_SessionList.begin();Iter!=m_SessionList.end();Iter++)
        {
            if (Iter->eRtspState == PLAYING)
            {
                iRet = TRUE;
            }
        }
        if(FALSE == iRet && tMediaFrameParam.eFrameType != FRAME_TYPE_UNKNOW)
        {
            pthread_mutex_unlock(&m_tSessionMutex);
            usleep(50*1000);
            continue;
        }
        for(Iter=m_SessionList.begin();Iter!=m_SessionList.end();Iter++)
        {
            if (Iter->eRtspState!= PLAYING)
            { //��Ƶ������ʵʱ�ģ�״̬�������������ݴ���˾ʹ����
                //sleep(10);//m_SessionList��������̣߳���session�ڴ����߳�
                continue;
            }
        
            switch(tMediaFrameParam.eFrameType)
            {
                case FRAME_TYPE_VIDEO_I_FRAME:
                case FRAME_TYPE_VIDEO_P_FRAME:
                case FRAME_TYPE_VIDEO_B_FRAME:
                {
                    if (NULL==Iter->pVideoRtpSession)
                    {
                        break;
                    }
                    
                    memset(&tRtpPacketParam,0,sizeof(T_RtpPacketParam));
                    Iter->pVideoRtpSession->GetRtpPacketParam(&tRtpPacketParam);
                    if (0 == Iter->dwLastTimestamp)
                    {
                        dwDiffTimestamp = 0;
                    }
                    else
                    {
                        dwDiffTimestamp = tMediaFrameParam.dwTimeStamp - Iter->dwLastTimestamp;
                    }
                    tRtpPacketParam.dwTimestamp += dwDiffTimestamp;
                    memset(&tTimeSpec,0,sizeof(struct timespec));
                    clock_gettime(CLOCK_MONOTONIC,&tTimeSpec);
                    if (0 != Iter->dwLastTimestamp)//����Ƶͬʱ��Դ������ֻ��ֻ��Ƶ������
                    {
                        iDelayTimeUs = (tTimeSpec.tv_sec-Iter->tLastTimeSpec.tv_sec)*1000*1000+(tTimeSpec.tv_nsec-Iter->tLastTimeSpec.tv_nsec)/1000-iDelayTimeUs;
                        iDelayTimeUs = dwDiffTimestamp/(tMediaInfo.dwVideoSampleRate/1000)*1000-iDelayTimeUs;//��ȥ����ʱ��
                    }
                    memcpy(&Iter->tLastTimeSpec,&tTimeSpec,sizeof(struct timespec));
                    if(iDelayTimeUs > 0)
                    {
                        usleep(iDelayTimeUs);//ʧЧ������
                    }
                    Iter->dwLastTimestamp = tMediaFrameParam.dwTimeStamp;
                
                    pbNaluStartPos = tMediaFrameParam.pbFrameStartPos;
                    dwNaluOffset = 0;
                    for(i=0;i<tMediaFrameParam.dwNaluCount;i++)
                    {
                        iPacketNum=m_pRtpPacket->Packet(&tRtpPacketParam,pbNaluStartPos,tMediaFrameParam.a_dwNaluEndOffset[i]-dwNaluOffset,ppbPacketBuf,aiEveryPacketLen);
                        for(j=0;j<iPacketNum;j++)
                        {
                            iRet=Iter->pVideoRtpSession->SendRtpData((char *)ppbPacketBuf[j], aiEveryPacketLen[j]);
                            if(FALSE==iRet)
                                break;
                        }
                        Iter->pVideoRtpSession->SetRtpPacketParam(&tRtpPacketParam);
                        cout<<"PacketBuf:"<<tMediaFrameParam.a_dwNaluEndOffset[i]<<" PacketNum:"<<iPacketNum<<endl;
                        pbNaluStartPos = tMediaFrameParam.pbFrameStartPos +tMediaFrameParam.a_dwNaluEndOffset[i];
                        dwNaluOffset =tMediaFrameParam.a_dwNaluEndOffset[i];
                    }

                    break;
                }
                case FRAME_TYPE_AUDIO_FRAME:
                {
                    if (NULL==Iter->pAudioRtpSession)
                    {
                        break;
                    }
                    memset(&tRtpPacketParam,0,sizeof(T_RtpPacketParam));
                    Iter->pAudioRtpSession->GetRtpPacketParam(&tRtpPacketParam);
                    iPacketNum=m_pRtpPacket->Packet(&tRtpPacketParam,tMediaFrameParam.pbFrameStartPos,tMediaFrameParam.iFrameLen,ppbPacketBuf,aiEveryPacketLen);
                    for(j=0;j<iPacketNum;j++)
                    {
                        iRet=Iter->pAudioRtpSession->SendRtpData((char *)ppbPacketBuf[i], aiEveryPacketLen[i]);
                        if(FALSE==iRet)
                            break;
                    }
                    Iter->pAudioRtpSession->SetRtpPacketParam(&tRtpPacketParam);
                    
                    break;
                }
                default:
                {
                
                    break;
                }
            }
        }
        tMediaFrameParam.eFrameType = FRAME_TYPE_UNKNOW;
        pthread_mutex_unlock(&m_tSessionMutex);
        usleep(10*1000);
    }
    if(NULL != tMediaFrameParam.pbFrameBuf)
    {
        delete tMediaFrameParam.pbFrameBuf;
    }
    iRet=m_pRtpPacket->DeInit(ppbPacketBuf, RTP_MAX_PACKET_NUM);
    return iRet;
}


/*****************************************************************************
-Fuction		: SessionHandleThread
-Description	: SessionHandleThread
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void * RtspServer::RtspStreamHandleThread(void *pArg)
{
	if( NULL != pArg )
	{
		RtspServer *pRtspServer = ( RtspServer * )pArg;
		pRtspServer->RtspStreamHandle();
	}

	return NULL;
}


















