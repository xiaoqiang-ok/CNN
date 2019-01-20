/*
作者：黄志强 
单位：内蒙古大学计算机学院
email：huangzhiqiang89@qq.com
date： 2014-12-16
*/
// CNN2.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CCNN2App:
// 有关此类的实现，请参阅 CNN2.cpp
//

class CCNN2App : public CWinApp
{
public:
	CCNN2App();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CCNN2App theApp;