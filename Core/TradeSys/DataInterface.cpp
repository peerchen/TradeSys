
#include "DataInterface.h"
#include "Data_db.h"
#include <leveldb.h>
#include <vector>
#include <string>
#include <iostream>

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

	return 1;
}
int DataInterface::PutData(char * Instrument, int InstrumentNum, unsigned long long tm, const struct data_type & data)
{
	char key[256];

	sprintf(key, "%s_%04d_%016lld", Instrument, InstrumentNum, tm);

	db->insert_data(key, 256, &data, sizeof(data));

	return 1;
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