#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "DataType.h"
#include "Data_db.h"
#include <vector>

class DataInterface
{
public:
	DataInterface();
	~DataInterface();
	int GetData(char * Instrument, int InstrumentNum, unsigned long long  start_tm , unsigned long long  end_tm, std::vector<struct data_type > & data);
	int GetData_range(char * Instrument, int InstrumentNum, unsigned long long  start_tm , unsigned long long  end_tm, struct rang_data_type * data);
	int PutData(char * Instrument, int InstrumentNum, unsigned long long  tm, const struct data_type &  data);

//	int Get_current_Data_start(char * Instrument , int InstrumentNum,  unsigned long long  start_tm ,  std::vector<struct data_type> & data_out);
//	int Get_rid_Data(char * Instrument , int InstrumentNum , int rid, unsigned long long  start_tm , unsigned long long  end_tm, std::vector<struct data_type> & data);

protected:
	class DataDB * db;
};

class SingleInsData
{
public:
	SingleInsData(DataInterface * in, char * Instrument);
	~SingleInsData();
	int GetLastData(std::vector<struct data_type > &data);
	//int GetData_range(char * Instrument, int InstrumentNum, unsigned long long  start_tm , unsigned long long  end_tm, struct rang_data_type * data);
	//int PutData(char * Instrument, int InstrumentNum, unsigned long long  tm, const struct data_type &  data);

	//	int Get_current_Data_start(char * Instrument , int InstrumentNum,  unsigned long long  start_tm ,  std::vector<struct data_type> & data_out);
	//	int Get_rid_Data(char * Instrument , int InstrumentNum , int rid, unsigned long long  start_tm , unsigned long long  end_tm, std::vector<struct data_type> & data);

protected:
	char Ins[32];
	DataInterface * Data_hd;
	unsigned long long  last_tm;
};

#endif