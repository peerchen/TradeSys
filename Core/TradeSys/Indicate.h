#ifndef INDICATE_H
#define INDICATE_H

#include <vector>


class Instrument_base_Indicate{
public:
	Instrument_base_Indicate(char * ID_in);

	int MACD_D1, MACD_D2, MACD_D3;
	float pre_MACD_d1; //12 for dif 
	float pre_MACD_d2; //26 for dif
	float pre_MACD_d3; //9 fast one

	void Set_MACD_para(int D1, int D2, int D3);
	void cal_MACD(float current,  float * dif, float * dem, float * osc);

	int RSI_D;
	void Set_RSI_para(int D);
	void cal_RSI(float current);

	

	void Set_KDJ_para(int Day);
	int cal_KDJ(struct rang_data_type data ,float * RSD,  float * K , float * D, float * J);
	float pre_K;
	float pre_D;

	void Cal_OBV(float current);

	void Set_MA_para(int day);
	int cal_MA(struct rang_data_type data, float * ma);

private:
	int store_data(struct rang_data_type data );
	int max_n;
	std::vector<struct rang_data_type> n_day_data;
	char ID[32];

};


#endif