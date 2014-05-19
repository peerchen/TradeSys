#include "Indicate.h"
#include "DataType.h"
#include <string.h>

float cal_EMA(float pre, float current , int day)
{
	return (pre*(day-1) + current*2)/(day+1);
}

float cal_MACD_DIF(float EMA_fast, float EMA_slow)
{
	return EMA_fast - EMA_slow;
}

Instrument_base_Indicate::Instrument_base_Indicate(char * id)
{
	strcpy(ID, id);
	pre_MACD_d1 = 0;
	pre_MACD_d2 = 0;
	pre_MACD_d3 = 0;


	pre_K = 50;
	pre_D = 50;
}

void Instrument_base_Indicate::Set_MACD_para(int D1, int D2, int D3) //example 12 26 9
{
	MACD_D1 = D1;
	MACD_D2 = D2;
	MACD_D3 = D3;
	return;
}


void Instrument_base_Indicate::cal_MACD(float current,  float * dif, float * dem, float * osc)
{
	if(pre_MACD_d1 == 0)
	{
		pre_MACD_d1 = current;
		pre_MACD_d2 = current;
		pre_MACD_d3 = 0;
	}
	else
	{
		pre_MACD_d1 = cal_EMA(pre_MACD_d1, current, MACD_D1);
		pre_MACD_d2 = cal_EMA(pre_MACD_d2, current, MACD_D2);
		
	}

	*dif = pre_MACD_d1 - pre_MACD_d2;
	pre_MACD_d3 = cal_EMA(pre_MACD_d3, *dif, MACD_D3);
	*dem = pre_MACD_d3;
	*osc = (*dif) - (*dem);

	return; 
}

void Instrument_base_Indicate::Set_KDJ_para(int  in)
{
	max_n = in;

	return ;
}




int Instrument_base_Indicate::cal_KDJ(struct rang_data_type data, float * RSV,  float * K , float * D, float * J)
{
	int i;
	float close = data.close_price;
	float Hn = 0.0;
	float Ln = 999999999.0;
	//int RSV=£¨Cn£­Ln£©¡Â£¨Hn£­Ln£©¡Á100;
	if(store_data(data) < 0)
	{
		return -1;
	}

	for(i = 0 ; i < n_day_data.size(); i++)
	{
		if(n_day_data[i].high > Hn)
		{
			Hn = n_day_data[i].high;
		}

		if(n_day_data[i].low < Ln)
		{
			Ln = n_day_data[i].low;
		}
	}

	*RSV = 100*(close-Ln)/(Hn-Ln);

	*K = (pre_K*2+*RSV)/3;
	pre_K = *K;
	*D = (pre_D*2 + *K)/3;
	pre_D = 0;

	*J = *K*3-*D*2;

	return 1;
}

void Instrument_base_Indicate::Set_MA_para(int in)
{
	max_n = in;
	return ;
}

int Instrument_base_Indicate::cal_MA(struct rang_data_type data, float * ma)
{
	int i;
	float total = 0;
	if(store_data(data) < 0)
	{
		return -1;
	}

	for(i = 0 ; i < n_day_data.size(); i++)
	{
		total +=  n_day_data[i].close_price;
	}

	*ma = total/n_day_data.size();

	return 1;
}


int Instrument_base_Indicate::store_data(struct rang_data_type data)
{
	n_day_data.push_back(data);

	if(n_day_data.size() > max_n)
	{
		n_day_data.erase(n_day_data.begin());
	}

	if(n_day_data.size()  == max_n)
		return 1;
	return 0;
}

/*
void cal_KDJ(float current)
{

}
*/