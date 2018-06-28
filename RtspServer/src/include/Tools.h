/*****************************************************************************
* Copyright (C) 2017-2020 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	Tools.h
* Description		: 	Tools operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>  
#include <stdlib.h>

#include <regex.h>


#define MAX_MATCH_NUM       8

/*****************************************************************************
-Class			: Tools
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class Tools
{
public:
    ~Tools();
    static Tools * Instance();
    int Regex(const char *i_strPattern,char *i_strBuf,regmatch_t *o_ptMatch);
    const char * GetLocalIP();
    const char * UseSocketGetIP(int i_iClientSocketFd);
    unsigned short UseSocketGetPort(int i_iSocketFd);
    unsigned int GetRandom();
    unsigned long long GetSysTime (void);

    
private:
    Tools();//没有成员变量的都是工具函数的类所以直接用单例模式比较方便
	static Tools		*m_pInstance;
    
};




#endif
