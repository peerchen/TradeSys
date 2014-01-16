
#include "DataInterface.h"
#include "Data_db.h"
#include <leveldb.h>
#include <vector>
#include <string>
#include <iostream>
#include <time.h>
#include <leveldb.h>
#include "Trade_Util.h"


using namespace std;

DataInterface::DataInterface()
{
	db = new DataDB("data_db");
}

DataInterface::~DataInterface()
{
	delete db;
}



int DataInterface::GetData(char * Instrument, int  InstrumentNum, unsigned long long start_tm , unsigned long long end_tm, vector<struct data_type > & data_out)
{

#if 1
	leveldb::Iterator* it = db->db->NewIterator(leveldb::ReadOptions());
	if(!it)
		return 1;

	char start[256];
	char end[256];

	sprintf(start, "%s_%04d_%016lld", Instrument, InstrumentNum, start_tm);
	sprintf(end, "%s_%04d_%016lld", Instrument, InstrumentNum, end_tm);
	leveldb::Slice s_start((char*)start, sizeof(start));
	leveldb::Slice s_end((char*)end, sizeof(end));

	for (it->Seek(s_start); (it->Valid()) && (it->key().ToString() < end); it->Next()) {
		leveldb::Slice slice = it->value(); 
		struct data_type * data = (struct data_type * )slice.data();

		data_out.push_back(*data);

	}
	if(!it->status().ok())
	{
		std::cout<<it->status().ToString();
	}
		assert(it->status().ok());  // Check for any errors found during the scan
	delete it;
#endif
	return 1;
}



int DataInterface::GetData_range(char * Instrument, int InstrumentNum, unsigned long long  start_tm , unsigned long long  end_tm, struct rang_data_type * data)
{
	std::vector<struct data_type> data_in;
	GetData(Instrument, InstrumentNum, start_tm, end_tm, data_in);

	if(data_in.size() > 0)
	{
		int i;
		data->open_price = data_in[0].last_Price;
		data->close_price = data_in[data_in.size() -1].last_Price;

		data->high = data_in[0].last_Price;
		data->low = data_in[0].last_Price;
		data->volume = 0;

		for(i = 0 ; i < data_in.size(); i++)
		{
			data->volume += data_in[i].volume;

			if(data_in[i].last_Price > data->high)
			{
				data->high = data_in[i].last_Price;
			}

			if(data_in[i].last_Price < data->low)
			{
				data->low = data_in[i].LowestPrice;
			}
		}

		return 1;
	}

	data->high = 0;

	return -1;
}

int DataInterface::PutData(char * Instrument, int InstrumentNum, unsigned long long tm, const struct data_type & data)
{
	char key[256];

	sprintf(key, "%s_%04d_%016lld", Instrument, InstrumentNum, tm);

	db->insert_data(key, 256, &data, sizeof(data));

	return 1;
}




SingleInsData::SingleInsData(DataInterface * in, char * Instrument)
{
	//struct timeval tv;
	//gettimeofday( &tv, NULL);

	time_t tm = time(NULL);
	
	last_tm = (tm )*1000;
	Data_hd = in;
	strcpy(Ins, Instrument);

}

SingleInsData::~SingleInsData()
{

}

int SingleInsData::GetLastData(std::vector<struct data_type > &data)
{
	int ret = Data_hd->GetData(Ins, 0, last_tm , 999999999999999ULL , data);
	if(data.size() > 0)
	{
		last_tm = data[data.size()-1].tm+1;
		return 1;
	}

	return 0;
}






#if 0
int DataInterface::Get_current_Data_start(char * Instrument , int InstrumentNum,  unsigned long long  start_tm ,  vector<struct data_type> & data_out)
{

	leveldb::Iterator* it = db->db->NewIterator(leveldb::ReadOptions());
	if(!it)
		return 1;

	char start[256];
	sprintf(start, "%s_%d_%lld", Instrument, InstrumentNum, start_tm);
	leveldb::Slice s_start((char*)start, sizeof(start));

	for (it->Seek(s_start); it->Valid(); it->Next()) {
		leveldb::Slice slice = it->value(); 
		struct data_type * data = (struct data_type * )slice.data();
		//int len = slice.size();
		
		data_out.push_back(*data);

	}
	assert(it->status().ok());  // Check for any errors found during the scan
	delete it;

	return 1;
}
#endif