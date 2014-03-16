
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

void  create_tick_key(char * key, char * Instrument, int  InstrumentNum, unsigned long long start_tm)
{
	memset(key, 0x00, KEY_SIZE);
	sprintf(key, "%s_%04d_%016lld", Instrument, InstrumentNum, start_tm);

	return;
}

void  create_1min_key(char * key, char * Instrument, int  InstrumentNum, unsigned long long start_tm)
{
	memset(key, 0x00, KEY_SIZE);
	sprintf(key, "min1_%s_%04d_%016lld", Instrument, InstrumentNum, start_tm);

	return;
}

void  create_day_key(char * key, char * Instrument, int  InstrumentNum, unsigned long long start_tm)
{
	memset(key, 0x00, KEY_SIZE);
	sprintf(key, "day_%s_%04d_%016lld", Instrument, InstrumentNum, start_tm);

	return;
}


void DataInterface::SetKeyType(Enum_Key_type key_type)
{
	Key_type = key_type;
	return;
}

void DataInterface::GenKey(char * key, char * Instrument, int  InstrumentNum, unsigned long long tm)
{

	switch(Key_type)
	{
		case TICK_Key:
			create_tick_key(key, Instrument, InstrumentNum, tm);
			break;
		case MIN_1_Key:
			create_1min_key(key, Instrument, InstrumentNum, tm);
			break;
		case DAY_Key:
			create_day_key(key, Instrument, InstrumentNum, tm);
			break;
		default:
			break;
	}		

	return ;
}

int DataInterface::GetData(char * Instrument, int  InstrumentNum, unsigned long long start_tm , unsigned long long end_tm, vector<struct tick_data_type > & data_out)
{
	leveldb::Iterator* it = db->db->NewIterator(leveldb::ReadOptions());
	if(!it)
		return 1;

	char start[KEY_SIZE];
	char end[KEY_SIZE];
	
	create_tick_key(start,  Instrument, InstrumentNum, start_tm);
	create_tick_key(end,  Instrument, InstrumentNum, end_tm);
	
	leveldb::Slice s_start((char*)start, sizeof(start));
	leveldb::Slice s_end((char*)end, sizeof(end));

	for (it->Seek(s_start); (it->Valid()) && (it->key().ToString() < end); it->Next()) {
		leveldb::Slice slice = it->value(); 
		struct tick_data_type * data = (struct tick_data_type * )slice.data();

		data_out.push_back(*data);
	}

	if(!it->status().ok())
	{
		std::cout<<it->status().ToString();
	}
	assert(it->status().ok());  // Check for any errors found during the scan
	delete it;

	return 1;
}


int DataInterface::GetData_range(char * Instrument, int InstrumentNum, unsigned long long  start_tm , unsigned long long  end_tm, struct rang_data_type * data)
{
	std::vector<struct tick_data_type> data_in;
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


int DataInterface::GetOneData(char * Instrument, int  InstrumentNum, unsigned long long tm ,void *data, int size)
{
	char key[256];
	string d;

	GenKey(key, Instrument, InstrumentNum,tm);

	if(db->get_data(key, 256, d)> 0)
	{
		memcpy(data, d.c_str(), size);
		return 1;
	}

	return -1;
}



int DataInterface::PutData(char * Instrument, int InstrumentNum, unsigned long long tm, const struct tick_data_type & data)
{
	char key[256];

	create_tick_key(key,  Instrument, InstrumentNum, tm);

	db->insert_data(key, 256, &data, sizeof(data));

	return 1;
}

int DataInterface::end_batch()
{
	db->put_batch();
	return 1;
}

int DataInterface::start_batch()
{
	db->clear_batch();
	return 1;
}

int DataInterface::Put_Data_batch(char * key, void * data, int data_size)
{
	db->insert_data_batch(key, 256, data, data_size);

	return 1;
}

int	DataInterface::Put_Tick_Data_batch(char * Instrument, int InstrumentNum, unsigned long long  tm, const struct tick_data_type &  data)
{
	char key[256];

	memset(key, 0x00, sizeof(key));
	sprintf(key, "%s_%04d_%016lld", Instrument, InstrumentNum, tm);

	db->insert_data_batch(key, 256, &data, sizeof(data));

	return 1;
}




SingleInsData::SingleInsData(DataInterface * in, char * Instrument)
{
	//struct timeval tv;
	//gettimeofday( &tv, NULL);

	PB_mode = 0;
	time_t tm = time(NULL);
	
	last_tm = (tm )*1000;
	Data_hd = in;
	strcpy(Ins, Instrument);

}

SingleInsData::~SingleInsData()
{

}


void SingleInsData::SetPBMode(unsigned long long start_tm)
{
	last_tm = start_tm;
	PB_mode = 1;
}

int SingleInsData::GetLastTickData(std::vector<struct tick_data_type > &data)
{
	unsigned long long endtm = 999999999999999ULL;
	if(PB_mode == 1)
	{
		endtm = last_tm + 600;
	}

	int ret = Data_hd->GetData(Ins, 0, last_tm , endtm , data);
	if(data.size() > 0)
	{
		last_tm = data[data.size()-1].tm+1;
		return 1;
	}

	if(PB_mode == 1)
	{
		last_tm = endtm;
	}

	return 0;
}

int SingleInsData::GetCurrentMinData(struct k_data_type * data)
{
	unsigned long long tm = (last_tm/(60*1000))*60*1000;
	if(min_tm != tm)
	{
		GetMinData(tm , data);
		min_tm = tm;
		return 1;
	}

	return 0;
}



int SingleInsData::GetMinData(unsigned long long tm, struct k_data_type * data)
{
	Data_hd->SetKeyType(MIN_1_Key);
	return Data_hd->GetOneData(Ins, 0, tm, data, sizeof(struct k_data_type));
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