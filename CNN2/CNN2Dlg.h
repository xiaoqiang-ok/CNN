/*
���ߣ���־ǿ 
��λ�����ɹŴ�ѧ�����ѧԺ
email��huangzhiqiang89@qq.com
date�� 2014-12-16
*/
// CNN2Dlg.h : ͷ�ļ�
//

#pragma once


// CCNN2Dlg �Ի���
class CCNN2Dlg : public CDialogEx
{
// ����
public:
	CCNN2Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CNN2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
