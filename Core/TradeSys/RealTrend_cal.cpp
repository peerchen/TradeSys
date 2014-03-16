#include "RealTrend_cal.h"


int RealTrend_class::Set_Cycle(int cycle_in)
{
	cycle = cycle_in;
	data_size = cycle*2+1;

	return 1;
}

int RealTrend_class::cal_trend(float next_n)
{
	int i;
	static float prev_data;
	datas.push_back(next_n);


	if(datas.size() < data_size)
	{
		return 0;
	}

	if(datas.size() > data_size)
	{
		datas.erase(datas.begin());
	}

	float total = 0;
	for(i = 0 ; i< datas.size(); i++)
	{
		total += datas[i];
	}
	
	float current = total/data_size;
	int ret ;
	
	if(current > prev_data)
	{
		ret =1;
	}
	else if(current < prev_data)
	{
		ret = -1;
	}
	else
		ret = 0;

	prev_data = current;

	return ret;
}


int RealTrend_class::cal_Max(float next_n)
{
	int i;
	static float prev_data;
	datas.push_back(next_n);


	if(datas.size() < data_size)
	{
		return 0;
	}

	if(datas.size() > data_size)
	{
		datas.erase(datas.begin());
	}

	int mid = cycle-1;
	int min_ = 0;
	int max_ = 0;
	for(int i = 1 ; i < datas.size(); i++ )
	{
		if(datas[min_] > datas[i])
		{
			min_ = i;
		}

		if(datas[max_] < datas[i])
		{
			max_ = i;
		}
	}
	
	if(min_ == mid)
		return -1;
	if(max_ == mid)
		return 1;

	return 0;
}

