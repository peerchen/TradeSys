// TradeSys.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MarketPrice.h"
#include "ConfigFile.h"

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	MarketPrice MP_hd;
	config_class config("test.conf");
	string addr;
	string broker;
	string user;
	string passwd;
	vector<string> IDs;

	config.get_string("Front_Address", addr);
	config.get_string("Broker_ID", broker);
	config.get_string("User_name", user);
	config.get_string("PassWd", passwd);
	config.get_vector("Instrument_ID", IDs);


	MP_hd.setconfig(addr.c_str(), broker.c_str(), user.c_str(), passwd.c_str(), IDs);

	MP_hd.StartToGetData();
	
	return 0;
}

