#include "stdafx.h"
#include <windows.h>   


#include "data_analys_1.h"
#include "Trade_Util.h"
#include <stdio.h>   
#include "Indicate.h"
#include "RealTrend_cal.h"



void IsTradingDay(unsigned long long tm)
{
	
}

void NextDay()
{
	
}

void MA_sys(char * item, unsigned long long start_tm , int periord) 
{
	
}

#if 1
int Data_analys_1(DataInterface * m_interface)
{
	std::vector<struct tick_data_type> data;
	struct rang_data_type data_a;
	/*
	unsigned long long tm_s = time_convert(2014, 1, 27 , 0 , 0, 0, 0);
	unsigned long long tm_e = time_convert(2014, 1, 27, 0 , 10, 0, 0);

	m_interface->GetData("ag1501", 0, tm_s, tm_e, data);
	m_interface->GetData("ag1406", 0, tm_s, tm_e, data);
	//interface->GetData_range("ag08", 0, tm_s, tm_e, &data_a);
	*/
	SingleInsData Data(m_interface, "agMI");

	char fn[32];
	sprintf(fn, "static.txt");
	FILE * static_fp = fopen(fn , "w");

	int year = 2013;
	int counter[2] = {0, 0};
	for(int month = 1; month < 13; month++)
	{
		char filename[32];
		sprintf(filename, "%04d%02d.txt", year, month);
		FILE * fp = fopen(filename , "w");

		//sprintf(filename, "%04d%02d_K.txt", year, month);
		//FILE * fp_K = fopen(filename , "w");
		RealTrend_class Real_trend;
		Real_trend.Set_Cycle(5);

		int motion_range[128];
		int motion_total =0;

		memset(motion_range, 0x00, sizeof(motion_range));

		for(int day = 0 ; day < 32; day++)
		{
			unsigned long long ss = time_convert(year, month, day , 9 , 0, 0, 0);
			unsigned long long ee = time_convert(year, month, day,  15 , 0, 0, 0);
			unsigned long long base = ss;
			struct k_data_type dd;

			Instrument_base_Indicate MACD("ag1406");
			Instrument_base_Indicate V("ag1406_v");

			MACD.Set_MACD_para(10,20,9);
			V.Set_MACD_para(10,20,9);

			
			
			for( ;ss < ee; ss += 60*1000 )
			{
				if(Data.GetMinData(ss , &dd)>0)
				{
					float dif, dem, osc; 
					float v_dif, v_dem, v_osc;
					float v = dd.volume;
					MACD.cal_MACD(dd.close_price, &dif, &dem ,&osc);
					V.cal_MACD(v, &v_dif, &v_dem, &v_osc);

					struct k_data_type dc;
#if 0
					Data.GetMinData(ss + 60*1000*6, &dc);
					//int Rt_ret = Real_trend.cal_trend(dc.close_price);
	
					int d = abs(dd.high-dd.low);
					motion_range[d]++;
					motion_total ++;
#endif
					int Rt_ret = 0;
#if 1
					//struct k_data_type dc;
					Data.GetMinData(ss, &dc);

					if(dc.high - dd.close_price > 5)
					{
						counter[0]++;
						Rt_ret = 1;
					}

					else if(dd.close_price - dc.low > 5)
					{
						counter[1]++;
						Rt_ret = -1;
					}
#else

					int Rt_ret = Real_trend.cal_Max(dc.close_price);
#endif
					if(ss > base+30*60*1000 && ss < ee - 10*60*1000)
					{
						fprintf(fp, "%d ", Rt_ret);
						fprintf(fp, "1:%f 2:%f 3:%f 4:%f  5:%f 6:%f 7:%f 8:%f\n",dd.close_price, dif, dem, osc, v, v_dif, v_dem, v_osc);
					}

#if 0
					if(ss > base+30*60*1000 && ss < ee - 10*60*1000)
					{
						int flag = 0;
						for(int m = 1 ; m <= 5; m++)
						{
							struct k_data_type dc;
							Data.GetMinData(ss + 60*1000*m, &dc);

							if(dc.high - dd.close_price > 5)
							{
								counter[0]++;
								flag = 1;
							}

							else if(dd.close_price - dc.low > 5)
							{
								counter[1]++;
								flag = -1;
							}
						}
						
						if(flag == 1)
							fprintf(fp, "+1 ");
						else if(flag == -1)
							fprintf(fp, "-1 ");
						else
						{
							fprintf(fp, "0 ");
						}

						//if(flag != 0)
							fprintf(fp, "1:%f 2:%f 3:%f 4:%f  5:%f 6:%f 7:%f 8:%f\n",dd.close_price, dif, dem, osc, v, v_dif, v_dem, v_osc);
					}
#endif

					//fprintf(fp_K, "%lld %f %f %f %f\n",dd.tm, dd.open_price,dd.high,dd.low, dd.close_price);
					//	fflush(fp);
				}
			}
		}

		

		for(int mm = 1 ; mm < 64; mm++)
		{
			fprintf(static_fp, "%.4f , ", motion_range[mm]/(float)motion_total);
		}
		fprintf(static_fp, "\n");

		fclose(fp);
		//fclose(fp_K);
	}

#if 0
	while(1)
	{
		int i;
		std::vector<struct tick_data_type> data;
		
		data.clear();
		int ret = Data.GetLastData(data);

		for(i = 0 ; i< data.size(); i++)
		{
			printf("%f \n", data[i].last_Price);
		}

		Sleep(1000);
	}
#endif

	return 1;
}
#endif