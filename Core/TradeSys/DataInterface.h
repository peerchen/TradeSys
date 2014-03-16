#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "DataType.h"
#include "Data_db.h"
#include <vector>

#define KEY_SIZE 256

void  create_day_key(char * key, char * Instrument, int  InstrumentNum, unsigned long long start_tm);
void  create_1min_key(char * key, char * Instrument, int  InstrumentNum, unsigned long long start_tm);
void  create_day_key(char * key, char * Instrument, int  InstrumentNum, unsigned long long start_tm);

enum Enum_Key_type
{
	TICK_Key,
	MIN_1_Key,
	DAY_Key
};


class DataInterface
{
public:
	DataInterface();
	~DataInterface();
	int GetData(char * Instrument, int InstrumentNum, unsigned long long  start_tm , unsigned long long  end_tm, std::vector<struct tick_data_type > & data);
	int GetData_range(char * Instrument, int InstrumentNum, unsigned long long  start_tm , unsigned long long  end_tm, struct rang_data_type * data);
	
	int GetOneData(char * Instrument, int  InstrumentNum, unsigned long long tm ,void *data, int size);

	int PutData(char * Instrument, int InstrumentNum, unsigned long long  tm, const struct tick_data_type &  data);


	int end_batch();
	int start_batch();
	int Put_Data_batch(char * key, void * data, int data_size);
	int	Put_Tick_Data_batch(char * Instrument, int InstrumentNum, unsigned long long  tm, const struct tick_data_type &  data);



	void SetKeyType(Enum_Key_type key_type);
	void GenKey(char * key, char * Instrument, int  InstrumentNum, unsigned long long tm);

//	int Get_current_Data_start(char * Instrument , int InstrumentNum,  unsigned long long  start_tm ,  std::vector<struct data_type> & data_out);
//	int Get_rid_Data(char * Instrument , int InstrumentNum , int rid, unsigned long long  start_tm , unsigned long long  end_tm, std::vector<struct data_type> & data);

protected:
	class DataDB * db;
	enum Enum_Key_type Key_type;
};

class SingleInsData
{
public:
	SingleInsData(DataInterface * in, char * Instrument);
	~SingleInsData();
	int GetLastTickData(std::vector<struct tick_data_type > &data);
	int GetCurrentMinData(struct k_data_type * data);
	int GetMinData(unsigned long long tm, struct k_data_type * data);
	void SetPBMode(unsigned long long start_tm);



	//int GetData_range(char * Instrument, int InstrumentNum, unsigned long long  start_tm , unsigned long long  end_tm, struct rang_data_type * data);
	//int PutData(char * Instrument, int InstrumentNum, unsigned long long  tm, const struct data_type &  data);

	//	int Get_current_Data_start(char * Instrument , int InstrumentNum,  unsigned long long  start_tm ,  std::vector<struct data_type> & data_out);
	//	int Get_rid_Data(char * Instrument , int InstrumentNum , int rid, unsigned long long  start_tm , unsigned long long  end_tm, std::vector<struct data_type> & data);

protected:
	char Ins[32];
	DataInterface * Data_hd;
	int PB_mode;
	unsigned long long  last_tm;
	unsigned long long	min_tm;
};

#endif