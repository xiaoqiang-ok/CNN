this is a Visual C++ program developed in visual studio 2012,
this program execute a convolution neural network on MNIST(hand writing digital data). 

how to run this program��
1 download mnist data into path CNN2/data/mnist/,
    mnist file consists of four files:
	train-images.idx3-ubyte, 
	train-labels.idx1-ubyte, 
	t10k-images.idx3-ubyte,  
	t10k-labels.idx1-ubyte, 
	you can download them form  
	http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz 
	http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz
	http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz
	http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz
	and decompress them.
2 open CNN2.sln file with visual studio 2012, and run this program

3 press buttons to test their function








================================================================================
MICROSOFT �������: CNN2 ��Ŀ����
===============================================================================

Ӧ�ó�������Ϊ����������� CNN2 Ӧ�ó��򡣴�Ӧ�ó��򲻽���ʾ Microsoft ������Ļ���ʹ�÷�����������Ϊ����дӦ�ó������㡣

���ļ���Ҫ������� CNN2 Ӧ�ó����ÿ���ļ������ݡ�

CNN2.vcxproj
����ʹ��Ӧ�ó��������ɵ� VC++ ��Ŀ������Ŀ�ļ��� 
���������ɸ��ļ��� Visual C++ �İ汾��Ϣ���Լ��й�ʹ��Ӧ�ó�����ѡ���ƽ̨�����ú���Ŀ���ܵ���Ϣ��

CNN2.vcxproj.filters
    ����ʹ�á�Ӧ�ó����򵼡����ɵ� VC++ ��Ŀɸѡ���ļ���
    �������й���Ŀ�ļ���ɸѡ��֮��Ĺ�����Ϣ���� IDE �У�ͨ�����ֹ��������ض��ڵ����Է�����ʽ��ʾ����������չ�����ļ������磬��.cpp���ļ��롰Դ�ļ���ɸѡ��������

CNN2.h
����Ӧ�ó������Ҫͷ�ļ���������������Ŀ�ض���ͷ�ļ�(���� Resource.h)�������� CCNN2App Ӧ�ó����ࡣ

CNN2.cpp
���ǰ���Ӧ�ó����� CCNN2App ����ҪӦ�ó���Դ�ļ���

CNN2.rc
���ǳ���ʹ�õ����� Microsoft Windows ��Դ���б������� RES ��Ŀ¼�д洢��ͼ�ꡢλͼ�͹�ꡣ���ļ�����ֱ���� Microsoft Visual C++ �н��б༭����Ŀ��Դλ�� 2052 �С�

res\CNN2.ico
��������Ӧ�ó���ͼ���ͼ���ļ�����ͼ���������Ҫ��Դ�ļ� CNN2.rc �С�

res\CNN2.rc2
���ļ��������� Microsoft Visual C++ �н��б༭����Դ����Ӧ�ý���������Դ�༭���༭��������Դ���ڴ��ļ��С�


/////////////////////////////////////////////////////////////////////////////

Ӧ�ó����򵼴���һ���Ի�����:

CNN2Dlg.h��CNN2Dlg.cpp - �Ի���
��Щ�ļ����� CCNN2Dlg �ࡣ���ඨ��Ӧ�ó������Ի������Ϊ���öԻ����ģ��λ�� CNN2.rc �У����ļ������� Microsoft Visual C++ �н��б༭��


/////////////////////////////////////////////////////////////////////////////

������׼�ļ�:

StdAfx.h��StdAfx.cpp
��Щ�ļ�����������Ϊ CNN2.pch ��Ԥ����ͷ (PCH) �ļ�����Ϊ StdAfx.obj ��Ԥ���������ļ���

Resource.h
���Ǳ�׼ͷ�ļ����������µ���Դ ID��
Microsoft Visual C++ ��ȡ�����´��ļ���

/////////////////////////////////////////////////////////////////////////////

����ע��:

Ӧ�ó�����ʹ�á�TODO:��ָʾӦ��ӻ��Զ����Դ���벿�֡�

���Ӧ�ó����ڹ���� DLL ��ʹ�� MFC������Ҫ���·�����Щ MFC DLL�����Ӧ�ó������õ����������ϵͳ�ĵ�ǰ�������ò�ͬ������Ҫ���·�����Ӧ�ı��ػ���Դ MFC100XXX.DLL���й�����������ĸ�����Ϣ����μ� MSDN �ĵ����й� Redistributing Visual C++ applications (���·��� Visual C++ Ӧ�ó���)���½ڡ�

/////////////////////////////////////////////////////////////////////////////
