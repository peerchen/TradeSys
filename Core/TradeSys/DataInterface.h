
#include "DataType.h"

class DataInterface
{
	DataInterface();
	int GetData(char * Instrument, int InstrumentNum, int start_tm , int end_tm, vector<struct data_type > & data);

	int Get_rid_Data(char * Instrument , int InstrumentNum , int rid, int start_tm , int end_tm, vector<struct data_type> & data);
};
