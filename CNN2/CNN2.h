/*
���ߣ���־ǿ 
��λ�����ɹŴ�ѧ�����ѧԺ
email��huangzhiqiang89@qq.com
date�� 2014-12-16
*/
// CNN2.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCNN2App:
// �йش����ʵ�֣������ CNN2.cpp
//

class CCNN2App : public CWinApp
{
public:
	CCNN2App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCNN2App theApp;