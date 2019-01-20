#include "StdAfx.h"
#include "filter.h"


filter::filter(void)
	: nSize(0)
{
	for(int i=0;i<nSize;i++)
	{
		for(int j=0;j<nSize;j++)
	    mFactor[i][j]=float(1+rand()%100)/2000.0;
	}
}


filter::~filter(void)
{
}
