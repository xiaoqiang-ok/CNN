/*
���ߣ���־ǿ 
��λ�����ɹŴ�ѧ�����ѧԺ
email��huangzhiqiang89@qq.com
date�� 2014-12-16
*/
#pragma once
class featureMap
{
public:
	int nSize;//����Ӱ�����صĵ�ά����
	float *picMatrix;
	float *errMatrix;
	int *numMatrix;
	int numComb;
	int arrComb[20];//һ����sub��featuremap�����20����sub��featuremap����
	float bias;//ƫ��
	featureMap(void);
	featureMap(int);
	~featureMap(void);
};

