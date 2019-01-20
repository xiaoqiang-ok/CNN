
// CNN2Dlg.cpp : 实现文件
#include "stdafx.h"
#include "CNN2.h"
#include "CNN2Dlg.h"
#include "afxdialogex.h"

#include "filter.h"
#include "featureMap.h"

#include <Windows.h>
#include "cmath"
#include <iostream>
#include <fstream>
using namespace std;

#define F1K 6
#define F2K 12
#define N0N (16*F2K)
#define N1N 10
struct neuroNode
{
	float I;
	float O;
	float Err;
	float bias;
};
float pic[28*28]={0};
//设置卷积核
filter filter1[F1K];
filter filter2[F1K][F2K];
//设置卷积层与降采样层
featureMap fm1[F1K];
featureMap Sfm1[F1K];
featureMap fm2[F2K];
featureMap Sfm2[F2K];
//全连接网络
neuroNode node0[N0N];
neuroNode node1[N1N];
float w01[N0N][N1N]={0};
float l=1;

CDC *pDC;
int a=0;
int b=600;

float trainImage[60000][784];
int   trainLable[60000];
float testImage[10000][784];
int   testLable[10000];
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCNN2Dlg::CCNN2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCNN2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_epoch = _T("");
}
void CCNN2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT_EPOTCH, m_epoch);
	DDX_Text(pDX, IDC_EDIT_EPOTCH, m_epoch);
}
BEGIN_MESSAGE_MAP(CCNN2Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_watchConvol, &CCNN2Dlg::ObcWatchConvol)
	ON_BN_CLICKED(IDC_trainNet, &CCNN2Dlg::ObcTrainNet)
	ON_BN_CLICKED(IDC_saveNet, &CCNN2Dlg::ObcSaveNet)
	ON_BN_CLICKED(IDC_readNet, &CCNN2Dlg::ObcReadNet)
	ON_BN_CLICKED(IDC_recogTest, &CCNN2Dlg::ObcRecogTest)
END_MESSAGE_MAP()
// CCNN2Dlg 消息处理程序
BOOL CCNN2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	initial_net();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CCNN2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
//当用户拖动最小化窗口时系统调用此函数取得光标//显示。
HCURSOR CCNN2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI ConvolFun1(LPVOID lpPara)   
{   
	int i,j,ii,jj,k;
	k = (int)lpPara;
	for(i=0;i<=28-filter1[0].nSize;i++)
	{
		for(j=0;j<=28-filter1[0].nSize;j++)
		{
			float tempSum=0;
			for(ii=0;ii<filter1[0].nSize;ii++)
			{
				for(jj=0;jj<filter1[0].nSize;jj++)
				{
					tempSum+=filter1[k].mFactor[ii][jj]*pic[(i+ii)*28+(j+jj)];
				}
			}
			float tempU=tempSum+fm1[k].bias;
			fm1[k].picMatrix[i*fm1[k].nSize+j]=1.0/(1.0+exp(-tempU));//tanh(tempU);	
		}
	}
	return NULL;
}
DWORD WINAPI ConvolFun2(LPVOID lpPara)
{   
	int i,j,k,ii,jj,kk;
	k = (int)lpPara;
	for(i=0;i<fm2[k].nSize;i++)
	{
		for(j=0;j<fm2[k].nSize;j++)
		{
			float tempSum=0;//tempSum是其它featureMap的卷积加权和
			for(kk=0;kk<fm2[k].numComb;kk++)
			{
				int pick = fm2[k].arrComb[kk];
				for(ii=0;ii<filter2[0][0].nSize;ii++)
				{
					for(jj=0;jj<filter2[0][0].nSize;jj++)
					{
						tempSum += filter2[pick][k].mFactor[ii][jj]*Sfm1[pick].picMatrix[(i+ii)*Sfm1[pick].nSize+(j+jj)];
					}
				}
			}
			//tempSum/=float(fm2[k].numComb);
			float tempU = tempSum + fm2[k].bias;
			fm2[k].picMatrix[i*fm2[k].nSize+j] = 1.0/(1.0+exp(-tempU));//tanh(tempU);
		}
	}
	return NULL;
}
DWORD WINAPI ConvolErrSubFM1(LPVOID lpPara)//得到1层降采样层的误差
{   //计算
	int i,j,k,ii,jj,kk,i2,j2;
	k = (int)lpPara;
    //1层降采样误差初始化为0
	for(i=0;i<Sfm1[k].nSize;i++)
	{
		for(j=0;j<Sfm1[k].nSize;j++)
		{
			Sfm1[k].errMatrix[ i*Sfm1[k].nSize+j ] = 0;
		}
	}
	//从后层err收集err
	for(kk=0;kk<Sfm1[k].numComb;kk++)//与Sfm1[k]所有相连的fm2[errK]，收集错误
	{
		int   errK  = Sfm1[k].arrComb[kk];
		for(i=0;i<fm2[errK].nSize;i++)
		{
			for(j=0;j<fm2[errK].nSize;j++)//枚举fm2[errK]的每一个点
			{
				for(ii=0;ii<filter2[k][errK].nSize;ii++)
				{
					for(jj=0;jj<filter2[k][errK].nSize;jj++)//枚举filter2 kernel 的每个因子
					{
						//误差播撒到Sfm1的[i2,j2]位置
						i2 = i+ii;
						j2 = j+jj;
						float tempW = filter2[k][errK].mFactor[ii][jj];
						Sfm1[k].errMatrix[ i2*Sfm1[k].nSize+j2 ] += tempW * fm2[errK].errMatrix[ i*fm2[errK].nSize+j ];
					}
				}
			}
		}
	}
	return NULL;
}

int readFeatureMapComb2(CString path,int n)
{
	n=10;
	freopen("FeatureMapComb2.txt","r",stdin);
	int i,j,k;
	for(k=0; k<F1K; k++)
	{
		Sfm1[k].numComb=0;
	}
	scanf("%d",&n);
	int numComb,pick;
	for(k=0; k<F2K; k++)
	{
		scanf("%d ",&numComb);
		fm2[k].numComb = numComb;
		//scanf("%d ",&fm2[k].numComb);
		for(i=0; i<fm2[k].numComb; i++)
		{
			scanf("%d ",&pick);//&
			//assert(pick>F1K || pick<0);
			fm2[k].arrComb[i] = pick;
			Sfm1[pick].arrComb[Sfm1[pick].numComb++]=k; 
		}
	}
	fclose(stdin);
	return n;
}
//初始化网络，读入数据
void CCNN2Dlg::initial_net(void)
{
	pDC=GetDC();
	int i,j,k,ii,jj;
	//读入训练数据、测试数据
	CString barnPath = "data\\mnist\\";
	CString strTrainPicPath = barnPath+"train-images.idx3-ubyte";
	CString strTrainLabPath = barnPath+"train-labels.idx1-ubyte";
	CString strTestPicPath  = barnPath+"t10k-images.idx3-ubyte";
	CString strTestLabPath  = barnPath+"t10k-labels.idx1-ubyte";
	FILE *pFP  = fopen(strTrainPicPath, "rb");
	FILE *pFT  = fopen(strTrainLabPath, "rb");
	FILE *pFP2  = fopen(strTestPicPath, "rb");
	FILE *pFT2  = fopen(strTestLabPath, "rb");

	int nOffsetP = 16;
	int nOffsetT = 8;
	BYTE *pTrainP   = NULL;
	BYTE *pTrainL   = NULL;
	BYTE *pTestP   = NULL;
	BYTE *pTestL   = NULL;
	fseek(pFP,nOffsetP,SEEK_SET);//绕过开头的表示本文件有多少的32bit的数据
	fseek(pFT,nOffsetT,SEEK_SET);//绕过开头的表示本文件有多少的32bit的数据
	fseek(pFP2,nOffsetP,SEEK_SET);
	fseek(pFT2,nOffsetT,SEEK_SET);
	int nLen;
	pTrainP = new BYTE[28*28*60000];//新建要读出的像素字节的存放地址（整个image集）
	pTrainL = new BYTE[1    *60000];//新建要读出的像素字节的存放地址（整个lable集）
	pTestP  = new BYTE[28*28*10000];
	pTestL  = new BYTE[1    *10000];
	nLen = fread(pTrainP,784,60000,pFP); //将训练集image的所有字节像素读入到事先建立好的地址中
	nLen = fread(pTrainL,1,  60000,pFT);//将训练集lable的所有字节像素读入到事先建立好的地址中
	nLen = fread(pTestP, 784,10000,pFP2);
	nLen = fread(pTestL, 1,  10000,pFT2);
	for(k=0;k<60000;k++)//处理训练集的image和lable
	{
		for(i=0;i<784;i++)
		{
			if((pTrainP[k*784+i])>100)
				trainImage[k][i]=1.175;///为啥要设置成1.175 均衡正负
			else
				trainImage[k][i]=-0.1;
		}
		trainLable[k] = pTrainL[k];
	}
	for(k=0;k<10000;k++)//处理测试集的image和lable
	{
		for(i=0;i<784;i++)
		{
			if((pTestP[k*784+i])>100)
				testImage[k][i]=1.175;
			else
				testImage[k][i]=-0.1;
		}
		testLable[k] = pTestL[k];
	}

	//nn bias
	for(i=0;i<N1N;i++)
		node1[i].bias=0;
	//nn weight
	for(i=0;i<N0N;i++)
		for(j=0;j<N1N;j++)
			w01[i][j]=sqrt(6.0/(N0N*N1N))*float(rand()%1000-500)*0.002;
    //初始化卷积核
	//filter1 
	for(k=0;k<F1K;k++)
	 {
	    filter1[k].nSize=5; 
	 }
	 for(k=0;k<F1K;k++)
	 {
		for(i=0;i<filter1[k].nSize;i++)
		{
			for(j=0;j<filter1[k].nSize;j++)
			{
				filter1[k].mFactor[i][j]=sqrt(6.0/(7*25))*float(rand()%1000-500)*0.002;
				//float(rand()%200-100)/2500.0;//0.04;
			}
		}
	 }
	//filter2[f1k][f2k]
	for(i=0;i<F1K;i++)
	{
		for(j=0;j<F2K;j++)
		{
			filter2[i][j].nSize=5;
		}
	}
	for(i=0;i<F1K;i++)
	{
		for(j=0;j<F2K;j++)
		{
			for(ii=0;ii<filter2[0][0].nSize;ii++)
			{
				for(jj=0;jj<filter2[0][0].nSize;jj++)
				{
					filter2[i][j].mFactor[ii][jj]=sqrt(6.0/(13*6*25))*float(rand()%1000-500)*0.002;
						//float(1+rand()%200-100)/2500.0;//0.04;
				}
			}
		}
	}
	//构造featrueMap，与subFeatureMap,featureMapComb;
	for(k=0;k<F1K;k++)
	{
	    fm1[k] =featureMap(28-filter1[k].nSize+1);
	    Sfm1[k]=featureMap(fm1[k].nSize/2);
	}
	for(k=0;k<F2K;k++)
	{
		fm2[k]=featureMap(12-filter2[0][0].nSize+1);
		Sfm2[k]=featureMap(fm2[k].nSize/2);	
	}
	readFeatureMapComb2("FeatureMapComb2.txt",F2K);
}

void CCNN2Dlg::cnnForward()
{
	int i,j,k,ii,jj,kk;
	//卷积层1
	HANDLE hThread[F1K];//设置F1K个句柄
	for(k=0;k<F1K;k++)
	{
		hThread[k] = CreateThread(NULL, 0, ConvolFun1, LPVOID(k), 0, NULL);  //开辟F1K个线程，线程的执行函数为ConvolFun1，参数为k
	}
	for(k=0;k<F1K;k++)
	{
		WaitForSingleObject(hThread[k],INFINITE);//等待各线程执行完返回
		CloseHandle(hThread[k]);                 //线程结束后一定要关闭句柄，不然程序会越跑越慢，句柄也是一种资源，用完要关闭释放
	}
	//降采样1
	for(k=0;k<F1K;k++)
	{
		for(i=0;i<Sfm1[k].nSize;i++)
		{
			for(j=0;j<Sfm1[k].nSize;j++)
			{
				float tempSum=0;
				tempSum += fm1[k].picMatrix[ 2*i    *fm1[k].nSize + 2*j   ];
				tempSum += fm1[k].picMatrix[ 2*i    *fm1[k].nSize + 2*j+1 ];
				tempSum += fm1[k].picMatrix[ (2*i+1)*fm1[k].nSize + 2*j   ];
				tempSum += fm1[k].picMatrix[ (2*i+1)*fm1[k].nSize + 2*j+1 ];
				Sfm1[k].picMatrix[ (i)*Sfm1[k].nSize+j ]=tempSum/4.0;
			}
		}
	}
	//卷积层2
	HANDLE hThread2[F2K];
	for(k=0;k<F2K;k++)
	{
		hThread2[k] = CreateThread(NULL, 0, ConvolFun2, LPVOID(k), 0, NULL);    
	}
	for(k=0;k<F2K;k++)
	{
		WaitForSingleObject(hThread2[k],INFINITE);
		CloseHandle(hThread2[k]);
	}
	//降采样2
	for(k=0;k<F2K;k++)
	{
		for(i=0;i<Sfm2[k].nSize;i++)
		{
			for(j=0;j<Sfm2[k].nSize;j++)
			{
				int fm2N = fm2[k].nSize;
				float tempSum = fm2[k].picMatrix[2*i*fm2[k].nSize + 2*j];
				tempSum += fm2[k].picMatrix[  2*i*fm2[k].nSize    + 2*j+1];
				tempSum += fm2[k].picMatrix[ (2*i+1)*fm2[k].nSize + 2*j];
				tempSum += fm2[k].picMatrix[ (2*i+1)*fm2[k].nSize + 2*j+1];
				Sfm2[k].picMatrix[(i)*Sfm2[k].nSize+j]=tempSum/4.0;
			}
		}
	}
	//得到全连接BP网络的输入层
	for(k=0;k<F2K;k++)
	{
		for(i=0;i<Sfm2[k].nSize;i++)
		{
			for(j=0;j<Sfm2[k].nSize;j++)
			{
				float tempfloat = Sfm2[k].picMatrix[i*Sfm2[k].nSize+j];
				node0[k*16+i*Sfm2[k].nSize+j].O = tempfloat;
			}
		}
	}
	float softSum=0;
	for(j=0;j<N1N;j++)//1st层神经网络训练，every neuro unit
	{
		node1[j].I=node1[j].bias;
		for(i=0;i<N0N;i++)//every colum of a pic，1层第j神经元 与 0层第i输入元的关系
		{
			node1[j].I+=w01[i][j]*node0[i].O;//w01[j][i] means 0层第j输入元的关系 与 1层第i神经元 
		}
		node1[j].O=exp(node1[j].I);
		softSum+=node1[j].O;
	}
	for(j=0;j<N1N;j++)//1st层神经网络训练，every neuro unit
	{
		node1[j].O/=softSum;
	}
}
void CCNN2Dlg::cnnBackward(int targetNum)
{
	int i,j,k,ii,jj,kk,i1,j1,i2,j2;
	//计算误差并反向传播
	for(i=0;i<N1N;i++)//1nd层神经元错误计算
	{
		if(i==targetNum)
			node1[i].Err=(1-node1[i].O)/float(N1N);//node1[i].O*(1-node1[i].O)*
		else
			node1[i].Err=(0-node1[i].O)/float(N1N);//node1[i].O*(1-node1[i].O)*
	}
	for(i=0;i<N0N;i++)//0层神经元错误计算,对于0层第i个神经元作计算
	{
		node0[i].Err=0;
		for(j=0;j<N1N;j++)
		{
			node0[i].Err += w01[i][j]*node1[j].Err;
		}
	}
	//CS诸层的反向过程
	//得到2层子采样层 Sfm2 的误差
	for(k=0;k<F2K;k++)
	{
		for(i=0;i<Sfm2[k].nSize;i++)
		{
			for(j=0;j<Sfm2[k].nSize;j++)
			{
				Sfm2[k].errMatrix[i*Sfm2[k].nSize+j] = node0[k*16+i*Sfm2[k].nSize+j].Err;
			}
		}
	}
	//得到2层卷积层 fm2 的误差
	for(k=0;k<F2K;k++)
	{
		for(i=0;i<Sfm2[k].nSize;i++)
		{
			for(j=0;j<Sfm2[k].nSize;j++)
			{
				float errTemp = Sfm2[k].errMatrix[(i)*Sfm2[k].nSize+j]/4.0;
				int indexT;
				indexT = 2*i    *fm2[k].nSize+2*j  ;fm2[k].errMatrix[indexT] = errTemp*(fm2[k].picMatrix[indexT])*(1-fm2[k].picMatrix[indexT]);
				indexT = 2*i    *fm2[k].nSize+2*j+1;fm2[k].errMatrix[indexT] = errTemp*(fm2[k].picMatrix[indexT])*(1-fm2[k].picMatrix[indexT]);
				indexT = (2*i+1)*fm2[k].nSize+2*j  ;fm2[k].errMatrix[indexT] = errTemp*(fm2[k].picMatrix[indexT])*(1-fm2[k].picMatrix[indexT]);
				indexT = (2*i+1)*fm2[k].nSize+2*j+1;fm2[k].errMatrix[indexT] = errTemp*(fm2[k].picMatrix[indexT])*(1-fm2[k].picMatrix[indexT]);
			}
		}
	}
	//得到1层子采样层 Sfm1 的误差
	HANDLE hThread[F1K];//设置F1K个句柄
	for(k=0;k<F1K;k++)
	{
		hThread[k] = CreateThread(NULL, 0, ConvolErrSubFM1, LPVOID(k), 0, NULL);  //开辟F1K个线程，线程的执行函数为ConvolFun1，参数为k
	}
	for(k=0;k<F1K;k++)
	{
		WaitForSingleObject(hThread[k],INFINITE);//等待各线程执行完返回
		CloseHandle(hThread[k]);                 //线程结束后一定要关闭句柄，不然程序会越跑越慢，句柄也是一种资源，用完要关闭释放
	}
	//得到1层卷积层 fm1 的误差
	for(k=0;k<F1K;k++)
	{
		for(i=0;i<Sfm1[k].nSize;i++)
		{
			for(j=0;j<Sfm1[k].nSize;j++)
			{
				//误差可以考虑按照比例分配给上层
				float errTemp=Sfm1[k].errMatrix[i*Sfm1[k].nSize+j]/4.0;
				int indexT;
				indexT = 2*i*fm1[k].nSize+2*j;
				fm1[k].errMatrix[indexT] = errTemp*fm1[k].picMatrix[indexT]*(1-fm1[k].picMatrix[indexT]);
				indexT = 2*i*fm1[k].nSize+2*j+1;
				fm1[k].errMatrix[indexT] = errTemp*fm1[k].picMatrix[indexT]*(1-fm1[k].picMatrix[indexT]);
				indexT = (2*i+1)*fm1[k].nSize+2*j;
				fm1[k].errMatrix[indexT] = errTemp*fm1[k].picMatrix[indexT]*(1-fm1[k].picMatrix[indexT]);
				indexT = (2*i+1)*fm1[k].nSize+2*j+1;
				fm1[k].errMatrix[indexT] = errTemp*fm1[k].picMatrix[indexT]*(1-fm1[k].picMatrix[indexT]);
			}
		}
	}
	
}
void CCNN2Dlg::applyGradient()
{
	int i,j,k, ii,jj,kk, i1,j1,i2,j2;
	//修订权重
	//1nd层神经元bias更新
	for(i=0;i<N1N;i++)
	{
		node1[i].bias+=l*node1[i].Err;
	}
	//w01更新 
	float wch;
	for(i=0;i<N0N;i++)
	{
		for(j=0;j<N1N;j++)
		{
			wch=l*node0[i].O*node1[j].Err;
			w01[i][j]+=wch;
		}
	}
	//更新fm2的bias
	float sumErr2[F2K];
	for(k=0;k<F2K;k++)
	{
		sumErr2[k]=0;
		for(i=0;i<fm2[k].nSize;i++)
		{
			for(j=0;j<fm2[k].nSize;j++)
				sumErr2[k] += fm2[k].errMatrix[ i*fm2[k].nSize+j ];
		}
		fm2[k].bias += l*sumErr2[k];
	}
	//更新fm1的bias
	float sumErr1[F1K];
	for(k=0;k<F1K;k++)
	{
		sumErr1[k]=0;
		for(i=0;i<fm1[k].nSize;i++)
		{
			for(j=0;j<fm1[k].nSize;j++)
				sumErr1[k] += fm1[k].errMatrix[ i*fm1[k].nSize+j ];
		}
		fm1[k].bias += l*sumErr1[k];// /float(fm1[k].nSize*fm1[k].nSize);
	}
	//filter2[k]的权重
	 for(k=0;k<F2K;k++)//every fm2
	 {
		 for(kk=0; kk<fm2[k].numComb; kk++)
		 {
			 int pick = fm2[k].arrComb[kk];//枚举到每个filter，filter2[pick][k]
			 for(ii=0; ii<5; ii++)
			 {
				 for(jj=0; jj<5; jj++)//每个filter的卷积核
				 {
					 float wch = 0;
					 for(i2=0; i2<8; i2++)
					 {
					    for(j2=0; j2<8; j2++)
						{
							i1 = i2+ii;
							j1 = j2+jj;
							wch += l * Sfm1[pick].picMatrix[i1*12+j1] * fm2[k].errMatrix[i2*8+j2];
						}
					 }
					 filter2[pick][k].mFactor[ii][jj] += wch;
				 }
			 }
			 
		 }
	 }
	 //filter1[k]的权重
	 for(k=0;k<F1K;k++)
	 {
		 for(ii=0;ii<5;ii++)
		 {
			 for(jj=0;jj<5;jj++)
			 {
	             float wch = 0;
				 for(i2=0;i2<24;i2++)
				 {
					 for(j2=0;j2<24;j2++)
					 {
						 i1 = i2+ii;
						 j1 = j2+jj;
						 wch += pic[i1*28+j1]*fm1[k].errMatrix[i2*24+j2];
					 }
				 }
				 filter1[k].mFactor[ii][jj] += l*wch;
			 }
		 }
	 }
}
//单图训练
void CCNN2Dlg::train(int orderNum)
{
	//读入原图
	int targetNum;
	memcpy(pic,trainImage[orderNum],784*sizeof(float));
	targetNum=trainLable[orderNum];
	//CNN前向过程
	cnnForward();
	//CNN反向过程：
	cnnBackward(targetNum);
	//权重更新
	applyGradient();
}
//单图识别
int CCNN2Dlg::recogfun()
{
	//CNN前向过程
	cnnForward();
	float  bigO=node1[0].O;
	int    bigN=0;
	for(int i=0;i<N1N;i++)
	{
		if(bigO<node1[i].O)
		{
			bigO=node1[i].O;
			bigN=i;
		}
	}
	return bigN;
}
//验证测试集的准确率
float CCNN2Dlg::my_verify()
{
	int i,j,k;
	int right=0;
	for(i=0;i<60000;i++)
	{
		memcpy(pic,trainImage[i],784*sizeof(float));
		int temp=recogfun();
		if(temp==trainLable[i])
			right++;
	}
	float accuracy=float(right)/float(60000);
	return accuracy;
}
//训练保存
void CCNN2Dlg::saveto(CString mySavePath)
{
	int i,ii,j,jj,k,kk;
	char tempStr[500];
	for(i=0;i<=mySavePath.GetLength();i++)
	    tempStr[i]=mySavePath[i];
	freopen(tempStr,"w",stdout);
   //filter1
   for(k=0;k<F1K;k++)
   {
	   printf("filter1[%d]\n",k);
	   for(i=0;i<filter1[k].nSize;i++)
	   {
		   for(j=0;j<filter1[k].nSize;j++)
		   {
			   printf("%f ",filter1[k].mFactor[i][j]);
		   }
		   printf("\n");
	   }
   }
   //filter2
   for(ii=0;ii<F1K;ii++)
   {
	   for(jj=0;jj<F2K;jj++)
	   {
		   printf("filter2[%d][%d]\n",ii,jj);
		   for(i=0;i<filter2[ii][jj].nSize;i++)
		   {
			   for(j=0;j<filter2[ii][jj].nSize;j++)
			   {
				   printf("%f ",filter2[ii][jj].mFactor[i][j]);
			   }
			   printf("\n");
		   }
	   }
   }
   //保存卷积层fm的bias
   printf("biasFm1\n");
   for(k=0;k<F1K;k++)
	   printf("%f ",fm1[k].bias);
   printf("\n");
   printf("biasFm2\n");
   for(k=0;k<F2K;k++)
	   printf("%f ",fm2[k].bias);
   printf("\n");
   //保存MLP的偏量
   printf("biasnode1\n");
   for(i=0;i<N1N;i++)
	   printf("%f ",node1[i].bias);
   printf("\n");
   //保存权重
   printf("w01[N0N][N1N]\n");
   for(i=0;i<N0N;i++)
   {
	   for(j=0;j<N1N;j++)
	   {
		   printf("%f ",w01[i][j]);
	   }
	   printf("\n");
   }
   fclose(stdout);
}
//5个按钮功能
void CCNN2Dlg::ObcWatchConvol()
{
	int i,j,k,ii,jj;
	//打开文件
	int orderNum=a++;
	int targetNum;
	memcpy(pic,testImage[orderNum],784*sizeof(float));
	targetNum=testLable[orderNum];
	cnnForward();
	//绘图
	CRect rc;                       //用于保存当前窗口的大小
	GetClientRect(&rc);
	CDC dcMem,*mypDC;//用于设置内存映像和获得当前设备上下文指针
	mypDC=GetDC();
	dcMem.CreateCompatibleDC(mypDC);//mypDC->dcMem; dcMem依附窗口DC创建兼容内存DC
	CBitmap bmp,*pOldBit;           //内存中承载临时图像的位图，及获得位图指针
	bmp.CreateCompatibleBitmap(mypDC,rc.Width(),rc.Height());//创建于当前窗口对应的位图
	pOldBit=dcMem.SelectObject(&bmp);//bmp->dcMem; dcMem选择位图
	dcMem.FillSolidRect(rc,RGB(255,255,255));
	//用memDC绘图
	//显示原图
	for(i=0;i<28;i++)
	{
		for(j=0;j<28;j++)
		{
			float myPv = 255*pic[j*28+i];
			if(myPv>0){
				dcMem.SetPixel(i,j,RGB(255,255,255));
			}
			else{
				dcMem.SetPixel(i,j,RGB(0,0,0));
			}
		}
	}
	//显示处理效果 卷积层1
	for(k=0;k<F1K;k++)
	{
		for(i=0;i<=28-filter1[0].nSize;i++)
		{
			for(j=0;j<=28-filter1[0].nSize;j++)
			{
				float myPv = 255*fm1[k].picMatrix[j*fm1[k].nSize+i];
				dcMem.SetPixel(30+i,30*k+j,RGB(myPv,myPv,myPv));
			}
		}
	}
	//显示 降采样1
	for(k=0;k<F1K;k++)
	{
		for(i=0;i<Sfm1[k].nSize;i++)
		{
			for(j=0;j<Sfm1[k].nSize;j++)
			{
				float myPv = 255*Sfm1[k].picMatrix[j*Sfm1[k].nSize+i];
				COLORREF myColor=RGB(myPv,myPv,myPv);
				dcMem.SetPixel(60+2*i,  30*k+2*j,  myColor);
				dcMem.SetPixel(60+2*i,  30*k+2*j+1,myColor);
				dcMem.SetPixel(60+2*i+1,30*k+2*j,  myColor);
				dcMem.SetPixel(60+2*i+1,30*k+2*j+1,myColor);
			}
		}
	}
	//卷积层2
	for(k=0;k<F2K;k++)
	{	
		for(i=0;i< fm2[k].nSize;i++)
		{
			for(j=0;j< fm2[k].nSize;j++)
			{
				float myPv = 255*fm2[k].picMatrix[j*fm2[k].nSize+i];
				COLORREF myColor=RGB(myPv,myPv,myPv);
				int tSize=2;
				for(int ii=0;ii<tSize;ii++)
					for(int jj=0;jj<tSize;jj++)
						dcMem.SetPixel(90+tSize*i+ii,  30*k+tSize*j+jj,  myColor);
			}
		}
	}
	//降采样2
	for(k=0;k<F2K;k++)
	{
		for(i=0;i<Sfm2[k].nSize;i++)
		{
			for(j=0;j<Sfm2[k].nSize;j++)
			{
				float myPv = 255*Sfm2[k].picMatrix[i*Sfm2[k].nSize+j];
				COLORREF myColor=RGB(myPv,myPv,myPv);
				for(ii=0;ii<4;ii++)
				{
					for(jj=0;jj<4;jj++)
					{
						dcMem.SetPixel(120+4*i+ii,  30*k+4*j+jj,  myColor);
					}
				}
			}
		}
	}
	for(k=0; k<N0N; k++)//显示MLP输入层
	{
		COLORREF myColor=RGB(255*node0[k].O,255*node0[k].O,255*node0[k].O);
		int tSize=2;
		for(int ii=0;ii<10;ii++)
		{
			for(int jj=0;jj<tSize;jj++)
			{
				dcMem.SetPixel(150+ii,  k*tSize+jj,  myColor);
			}
		}
	}
	for(k=0;k<N1N;k++)//显示MLP输出层
	{
		COLORREF myColor=RGB(255*node1[k].O,255*node1[k].O,255*node1[k].O);
		int tSize=15;
		for(int ii=0;ii<tSize;ii++)
		{
			for(int jj=0;jj<tSize;jj++)
			{
				dcMem.SetPixel(170+ii,  k*tSize+jj,  myColor);
			}
		}
		CString strK;
		strK.Format("%d",k);
		if(k==targetNum)
		{
			dcMem.SetTextColor(RGB(255,0,0));    //设置字体颜色
			dcMem.SetBkColor(RGB(200,200,0));
		}
		else
		{
			dcMem.SetTextColor(RGB(0,0,0));
			dcMem.SetBkColor(RGB(255,255,255));
		}
		dcMem.TextOut(170+20,  k*tSize,strK);
	}
	//memDC->mypDC,绘制内存映像
	mypDC->BitBlt(0,0,rc.Width()/2,rc.Height(),&dcMem,0,0,SRCCOPY);
	//善后，删除内存
	dcMem.DeleteDC();
	bmp.DeleteObject();
	ReleaseDC(mypDC);
	
}
void CCNN2Dlg::ObcSaveNet()
{
   int i,j,k,ii,jj;
   freopen("net.txt","w",stdout);
   //filter1
   for(k=0;k<F1K;k++)
   {
	   printf("filter1[%d]\n",k);
	   for(i=0;i<filter1[k].nSize;i++)
	   {
		   for(j=0;j<filter1[k].nSize;j++)
		   {
			   printf("%.2f ",filter1[k].mFactor[i][j]);
		   }
		   printf("\n");
	   }
   }
   //filter2
   for(ii=0;ii<F1K;ii++)
   {
	   for(jj=0;jj<F2K;jj++)
	   {
		   printf("filter2[%d][%d]\n",ii,jj);
		   for(i=0;i<filter2[ii][jj].nSize;i++)
		   {
			   for(j=0;j<filter2[ii][jj].nSize;j++)
			   {
				   printf("%.2f ",filter2[ii][jj].mFactor[i][j]);
			   }
			   printf("\n");
		   }
	   }
   }
   //保存卷积层fm的bias
   printf("biasFm1\n");
   for(k=0;k<F1K;k++)
	   printf("%.2f ",fm1[k].bias);
   printf("\n");
   printf("biasFm2\n");
   for(k=0;k<F2K;k++)
	   printf("%.2f ",fm2[k].bias);
   printf("\n");
   //保存MLP的偏量
   printf("biasnode1\n");
   for(i=0;i<N1N;i++)
	   printf("%.2f ",node1[i].bias);
   printf("\n");
   //保存权重
   printf("w01[N0N][N1N]\n");
   for(i=0;i<N0N;i++)
   {
	   for(j=0;j<N1N;j++)
	   {
		   printf("%.2f ",w01[i][j]);
	   }
	   printf("\n");
   }
   fclose(stdout);
}
void CCNN2Dlg::ObcReadNet()
{
	CString m_path;
	CFileDialog openhclDlg(TRUE,_T("txt"),NULL,0,_T("net文件|*.txt||"),this);
	if(openhclDlg.DoModal()==IDOK)
	{
		m_path=openhclDlg.GetPathName(); //得到文件目录
	}
	else
		return;
	if(!freopen(m_path,"r",stdin))
		return;
	int i,j,k,ii,jj;
	char cc[1024];
	string str;
	//卷积层参数的读入
	for(k=0;k<F1K;k++)
	{
		scanf("%s ",cc);//cin.getline(cc,1024);//
		for(i=0;i<filter1[k].nSize;i++)
		{
			for(j=0;j<filter1[k].nSize;j++)
			{
				scanf("%f ",&filter1[k].mFactor[i][j]);
			}
		}
	}
	//filter2
	for(ii=0;ii<F1K;ii++)
	{
		for(jj=0;jj<F2K;jj++)
		{
			scanf("%s ",cc);//cin.getline(cc,1024);
			for(i=0;i<filter2[ii][jj].nSize;i++)
			{
				for(j=0;j<filter2[ii][jj].nSize;j++)
				{
					scanf("%f ",&filter2[ii][jj].mFactor[i][j]);
				}
			}
		}
	}
	//卷积层fm的bias
	cin.getline(cc,1024);
	for(k=0;k<F1K;k++)
	{
		scanf("%f ",&fm1[k].bias);
	}
	cin.getline(cc,1024);
	for(k=0;k<F2K;k++)
		scanf("%f ",&fm2[k].bias);
	//node1bias
	cin.getline(cc,1024);
	for(i=0;i<N1N;i++)
		scanf("%f ",&node1[i].bias);
	//w01[N0N][N1N]
	cin.getline(cc,1024);
	for(i=0;i<N0N;i++)
		for(j=0;j<N1N;j++)
			scanf("%f ",&w01[i][j]);
	fclose(stdin);
	//fclose(stdout);
	MessageBox("read done!");
}
void CCNN2Dlg::ObcTrainNet()
{
	CString m_path="C:\\Users\\Administrator\\Desktop\\nn\\t10k-images-bmp\\t10k-images\\";
	freopen("result.txt","w",stdout);
	int i,j,k;
	int t_start,t_end;
	t_start=t_end=clock();
	float iterationT = 10;
	for(k=1;k<=iterationT;k++)
	{
		l=1*(iterationT-k+1)/iterationT;// + float(rand()%100)/1000.0; //learning rate
		for(i=0;i<60000;i++)
		{
			train(i);//train a sample
			if(i%100==0)//show net via a pic
			{
				MSG msg;
				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
				m_epoch.Format("epoch=%d i=%d",k,i);
				RedrawWindow();
				ObcWatchConvol();
				UpdateData(FALSE);
			}
		}
		/*for(i=0;i<60000;i++)
		{
			train(i);
			if(i%6000==0)
			{

				MSG msg;
				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
				m_epoch.Format("epoch=%d i=%d",k,i);
				RedrawWindow();
				ObcWatchConvol();
				UpdateData(FALSE);
			}
		}*/
		if(k%1==0)//每迭代100次保存网络
		{
			int t_preEnd=t_end;
			t_end=clock();
			float accuracy=my_verify();
			CString tempStr;
			tempStr.Format("k=%d-time=%ds-%.5f-net.txt",k,(t_end-t_preEnd)/1000,accuracy);
			saveto(tempStr);
		}
	}
}
void CCNN2Dlg::ObcRecogTest()
{
	int i,j,k;
	//打开文件
	CString m_path="C:\\Users\\Administrator\\Desktop\\nn\\nn\\t10k-images-bmp\\t10k-images\\";
	/*CFileDialog openhclDlg(TRUE,_T("bmp"),NULL,0,_T("bmp位图文件|*.*||"),this);
	if(openhclDlg.DoModal()==IDOK)
		m_path=openhclDlg.GetPathName();
	else return;
	m_path=m_path.Left(m_path.GetLength()-1);*/
	freopen("result.txt","w",stdout);
	int t_start,t_end;
	t_start=clock();
	int right=0;
	for(i=0;i<10000;i++)
	{
		memcpy(pic,testImage[i],784*sizeof(float));
		int temp=recogfun();
		//printf("%d.bmp :%d\n",testLable[i],temp);
		if(temp == testLable[i])
			right++;
	}
	t_end=clock();
	printf("共10000张图片，正确%d张，准确率%f\n,耗费时间为：%dms",right,float(right)/float(10000),(t_end-t_start));
	fclose(stdout);
	system("result.txt");
	//system("net.txt");
}