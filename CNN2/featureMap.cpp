#include "StdAfx.h"
#include "featureMap.h"


featureMap::featureMap(void)
{
}
featureMap::featureMap(int n)
{
    nSize=n;
	bias = 0;
	picMatrix=new float[n*n];
	errMatrix=new float[n*n];
	numMatrix=new int[n*n];
	/* numMatrix
	1  2  3  4  5  5  5  5  4  3  2  1  
	2  4  6  8  10 10 10 10 8  6  4  2  
	3  6  9  12 15 15 15 15 12 9  6  3  
	4  8  12 16 20 20 20 20 16 12 8  4  
	5  10 15 20 25 25 25 25 20 15 10 5  
	5  10 15 20 25 25 25 25 20 15 10 5  
	5  10 15 20 25 25 25 25 20 15 10 5  
	5  10 15 20 25 25 25 25 20 15 10 5  
	4  8  12 16 20 20 20 20 16 12 8  4  
	3  6  9  12 15 15 15 15 12 9  6  3  
	2  4  6  8  10 10 10 10 8  6  4  2  
	1  2  3  4  5  5  5  5  4  3  2  1  
		*/
}
featureMap::~featureMap(void)
{

}
