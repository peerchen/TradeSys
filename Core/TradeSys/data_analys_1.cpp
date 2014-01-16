#include "stdafx.h"
#include <windows.h>   


#include "data_analys_1.h"
//#include "Trade_Util.h"
#include <stdio.h>   



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
	std::vector<struct data_type> data;
	struct rang_data_type data_a;
	//unsigned long long tm_s = time_convert(113, 11, 12 , 0 , 1, 1, 0);
	//unsigned long long tm_e = time_convert(113, 11, 13, 16 , 1, 1, 1);

	//interface->GetData("ag08", 0, tm_s, tm_e, data);

	//interface->GetData_range("ag08", 0, tm_s, tm_e, &data_a);

	SingleInsData Data(m_interface, "ag1406");

	while(1)
	{
		int i;
		std::vector<struct data_type> data;
		
		data.clear();
		int ret = Data.GetLastData(data);

		for(i = 0 ; i< data.size(); i++)
		{
			printf("%f \n", data[i].last_Price);
		}

		Sleep(1000);
	}


	return 1;
}
#endif