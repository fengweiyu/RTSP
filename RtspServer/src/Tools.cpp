/*****************************************************************************
* Copyright (C) 2017-2020 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	Tools.cpp
* Description		: 	Tools operation center:内部全是一些工具函数
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "Tools.h"

#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <err.h>

#include "Definition.h"
#include <iostream>

using std::cout;//需要<iostream>
using std::endl;

Tools * Tools::m_pInstance = new Tools();//一般使用饿汉模式,懒汉模式线程不安全
/*****************************************************************************
-Fuction		: Tools
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
Tools::Tools()
{

}

/*****************************************************************************
-Fuction		: ~Tools
-Description	: ~Tools
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
Tools::~Tools()
{

}

/*****************************************************************************
-Fuction		: Instance
-Description	: Instance
-Input			:
-Output 		:
-Return 		:
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
Tools * Tools::Instance()
{
	return m_pInstance;
}

/*****************************************************************************
-Fuction		: Regex
-Description	: 正则表达式
.点				匹配除“\r\n”之外的任何单个字符
*				匹配前面的子表达式任意次。例如，zo*能匹配“z”，也能匹配“zo”以及“zoo”。*等价于o{0,}
				其中.*的匹配结果不会存储到结果数组里
(pattern)		匹配模式串pattern并获取这一匹配。所获取的匹配可以从产生的Matches集合得到
[xyz]			字符集合。匹配所包含的任意一个字符。例如，“[abc]”可以匹配“plain”中的“a”。
+				匹配前面的子表达式一次或多次(大于等于1次）。例如，“zo+”能匹配“zo”以及“zoo”，但不能匹配“z”。+等价于{1,}。
				//如下例子中不用+，默认是一次，即只能匹配到一个数字6
				
[A-Za-z0-9] 	26个大写字母、26个小写字母和0至9数字
[A-Za-z0-9+/=]	26个大写字母、26个小写字母0至9数字以及+/= 三个字符


-Input			: i_strPattern 模式串,i_strBuf待匹配字符串,
-Output 		: o_ptMatch 存储匹配串位置的数组,用于存储匹配结果在待匹配串中的下标范围
//数组0单元存放主正则表达式匹配结果的位置,即所有正则组合起来的匹配结果，后边的单元依次存放子正则表达式匹配结果的位置
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/11/01	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int Tools::Regex(const char *i_strPattern,char *i_strBuf,regmatch_t *o_ptMatch)
{
    char acErrBuf[256];
    int iRet=-1;
    regex_t tReg;    //定义一个正则实例
    //const size_t dwMatch = 6;    //定义匹配结果最大允许数       //表示允许几个匹配


    //REG_ICASE 匹配字母时忽略大小写。
    iRet =regcomp(&tReg, i_strPattern, REG_EXTENDED);    //编译正则模式串
    if(iRet != 0) 
    {
        regerror(iRet, &tReg, acErrBuf, sizeof(acErrBuf));
        cout<<"Regex Error:"<<acErrBuf<<endl;
    }
    else
    {
        iRet = regexec(&tReg, i_strBuf, MAX_MATCH_NUM, o_ptMatch, 0); //匹配他
        if (iRet == REG_NOMATCH)
        { //如果没匹配上
            cout<<"No Match!"<<endl;
        }
        else if (iRet == REG_NOERROR)
        { //如果匹配上了
            /*cout<<"Match"<<endl;
            int i=0,j=0;
			for(j=0;j<MAX_MATCH_NUM;j++)
			{
				for (i= o_ptMatch[j].rm_so; i < o_ptMatch[j].rm_eo; i++)
				{ //遍历输出匹配范围的字符串
					printf("%c", i_strBuf[i]);
				}
				printf("\n");
			}*/
        }
        else
        {
            cout<<"Unknow err:"<<iRet<<endl;
        }
        regfree(&tReg);  //释放正则表达式
    }
    
    return iRet;
}

/*****************************************************************************
-Fuction		: GetLocalIP
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
const char * Tools::GetLocalIP()
{
    static char strIpBuf[128];
    struct hostent *he;  
    char strHostName[40]={0} ;  
  
  
    gethostname(strHostName,sizeof(strHostName));  
    he = gethostbyname(strHostName);
    cout<<he<<"-handle-gethostbyname hostname:"<<strHostName<<endl;
    
    char **phe = NULL;  
    for( phe=he->h_addr_list ; NULL != *phe ; ++phe)  
    {  
        inet_ntop(he->h_addrtype,*phe,strIpBuf,sizeof(strIpBuf));
        cout<<"GetLocalIP addr:"<<strIpBuf<<endl;
    }  

    return strIpBuf;
}

/*****************************************************************************
-Fuction		: GetRemoteIP
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
const char * Tools::UseSocketGetIP(int i_iClientSocketFd)
{
	struct sockaddr_in inaddr;
	socklen_t addrlen = sizeof(inaddr);
	int ret = getpeername(i_iClientSocketFd, (struct sockaddr*)&inaddr, &addrlen);
	if (ret < 0) {
		err(1,"getpeername failed: %s\n", strerror(errno));
		return NULL;
	}
	return inet_ntoa(inaddr.sin_addr);
}

/*****************************************************************************
-Fuction		: GetLocalPort
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
unsigned short Tools::UseSocketGetPort(int i_iSocketFd)
{
	struct sockaddr_in inaddr;
	socklen_t addrlen = sizeof(inaddr);
	int ret = getsockname(i_iSocketFd, (struct sockaddr*)&inaddr, &addrlen);
	if (ret < 0) {
		err(1,"getsockname failed: %s\n", strerror(errno));
		cout<<"UseSocketGetPort err"<<endl;
		return 0;
	}
	return ntohs(inaddr.sin_port);
}

/*****************************************************************************
-Fuction		: GetRandom
-Description	: 
   Return a 32-bit random number.
   Because "our_random()" returns a 31-bit random number, we call it a second
   time, to generate the high bit.
   (Actually, to increase the likelhood of randomness, we take the middle 16 bits of two successive calls to "our_random()")

-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
unsigned int Tools::GetRandom()
{
    long random_1 = random();
    unsigned int random16_1 = (unsigned int)(random_1&0x00FFFF00);
    
    long random_2 = random();
    unsigned int random16_2 = (unsigned int)(random_2&0x00FFFF00);
    
    return (random16_1<<8) | (random16_2>>8);
}

/*****************************************************************************
-Fuction		: GetLocalPort
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 返回微妙us
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
unsigned long long Tools:: GetSysTime (void)
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);//clk_id为CLOCK_MONOTONIC，则返回系统启动后秒数和纳秒数。
	return (tp.tv_sec * 1000000llu + tp.tv_nsec / 1000llu);//转换为us
}









