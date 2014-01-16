
#include "Data_db.h"

#include <leveldb.h>



//#pragma comment(lib,"leveldb\\lib\\Release\\leveldb.lib")
#pragma comment(lib,"leveldb\\lib\\Debug\\leveldb_d.lib")
//#pragma comment(lib,"leveldb\\lib\\ReleaseDll\\leveldb.lib")
DataDB::DataDB(const char * name)
{
	leveldb::Options options;
	options.create_if_missing = true;
	options.write_buffer_size = 60 * 1048576;
	options.max_open_files = 64;
	leveldb::Status status = leveldb::DB::Open(options, name, &db);
	assert(status.ok());
}


int DataDB::insert_data(const void * key, int key_size, const void * data, int size)
{
	leveldb::Status status;

	if(NULL == data || size < 0)
		return -1;


	leveldb::Slice s_key((char*)key, key_size);
	leveldb::Slice value((char*)data, size);

	status = db->Put(leveldb::WriteOptions(), s_key, value);


	if(status.ok())
		return 1;
	else
	{
	//	LOG(ERROR)<<status.ToString();
		printf("add error\n");
		return 0;
	}

}

int DataDB::del_data(const void * key_in, int key_size) 
{
	leveldb::Slice key((char*)key_in,  key_size);
	leveldb::Status status;

	printf("%x %d\n", (int)(*(int *)key_in) , key_size);

	db->Delete(leveldb::WriteOptions(), key);

	if(status.ok())
	{
		return 1;
	}
	else
	{
		//LOG(ERROR)<<status.ToString();
		printf("del error\n");
		return 0;
	}

}

int DataDB::get_data(const void * key, int key_size, std::string & data)
{
	leveldb::Status status;

	leveldb::Slice s_key((char*)key, key_size);

	status = db->Get(leveldb::ReadOptions(), s_key, &data);


	if(status.ok())
		return 1;
	else
	{
		//LOG(ERROR)<<status.ToString();
		printf("add error\n");
		return 0;
	}

}




int DataDB::loop_db(int (*pFun)(void *ptr, void * db_data, int length), void * p)
{
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	if(!it)
		return 1;
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		leveldb::Slice slice = it->value(); 
		void * data = (void *)slice.data();
		int len = slice.size();


		pFun(p, data, len);

	}
	assert(it->status().ok());  // Check for any errors found during the scan
	delete it;

	return 1;
}

