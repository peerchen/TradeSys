
#include "data_analys_1.h"
#include "Trade_Util.h"



int Data_analys_1(DataInterface * interface)
{

	std::vector<struct data_type> data;
	struct rang_data_type data_a;
	unsigned long long tm_s = time_convert(113, 11, 10, 18 , 1, 1, 0);
	unsigned long long tm_e = time_convert(113, 11, 12, 9 , 1, 1, 1);

	interface->GetData("ag12", 0, tm_s, tm_e, data);

	interface->GetData_range("ag12", 0, tm_s, tm_e, &data_a);

	return 1;
}