/*
作者：黄志强 
单位：内蒙古大学计算机学院
email：huangzhiqiang89@qq.com
date： 2014-12-16
*/
#pragma once
class featureMap
{
public:
	int nSize;//特征影响像素的单维长度
	float *picMatrix;
	float *errMatrix;
	int *numMatrix;
	int numComb;
	int arrComb[20];//一个（sub）featuremap最多与20个（sub）featuremap相连
	float bias;//偏量
	featureMap(void);
	featureMap(int);
	~featureMap(void);
};

