#ifndef DATA_DB
#define DATA_DB

#include <string>
#include <leveldb.h>


class DataDB
{
public:
	DataDB(const char * file);

	int insert_data(const void * key , int key_size, const void * data , int data_size); 

	int del_data(const void * key, int key_size);

	int get_data(const void * key, int key_size, std::string & data);

	int loop_db(int (*pFun)(void *ptr, void * db_data, int length), void * p);

	int  put_batch();

	int  clear_batch();
	int insert_data_batch(const void * key, int key_size, const void * data, int size);

	leveldb::DB* db;
private:

	leveldb::WriteBatch batch;
	
};



#endif