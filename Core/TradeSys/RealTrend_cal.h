#ifndef REALTREND_CAL_H
#define REALTREND_CAL_H

#include <vector>
class RealTrend_class
{
public:
	int cycle;
	int Set_Cycle(int cycle);
	int cal_trend(float next_n);
	int cal_Max(float next_n);

private:
	std::vector<float> datas;
	int data_size;
};


#endif
