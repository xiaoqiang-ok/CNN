/*
作者：黄志强 
单位：内蒙古大学计算机学院
email：huangzhiqiang89@qq.com
date： 2014-12-16
*/
#pragma once
class filter
{
public:
	filter(void);
	~filter(void);
	// 虑波器的大小
	int nSize;
	float mFactor[5][5];
};

