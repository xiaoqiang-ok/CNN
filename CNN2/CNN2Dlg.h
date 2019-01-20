/*
作者：黄志强 
单位：内蒙古大学计算机学院
email：huangzhiqiang89@qq.com
date： 2014-12-16
*/
// CNN2Dlg.h : 头文件
//

#pragma once


// CCNN2Dlg 对话框
class CCNN2Dlg : public CDialogEx
{
// 构造
public:
	CCNN2Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CNN2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void  initial_net(void);
	void  getPic(CString imgPath);
	void  cnnForward();
	void  cnnBackward(int targetNum);
	void  applyGradient();
	void  train(int orderNum);
	int   recogfun();
    float my_verify();
	void  saveto(CString mySavePath);
	afx_msg void ObcWatchConvol();
	afx_msg void ObcTrainNet();
	afx_msg void ObcSaveNet();
	afx_msg void ObcReadNet();
	afx_msg void ObcRecogTest();
//	CEdit m_epoch;
	CString m_epoch;
};
