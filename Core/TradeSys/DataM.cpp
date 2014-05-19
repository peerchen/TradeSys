// TradeSys.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "MarketPrice.h"
#include "ConfigFile.h"
//#include "Data_db.h"
#include "Data_loader.h"
#include "DataInterface.h"
#include "time.h"
#include "Trader.h"
#include "data_analys_1.h"
#include<iostream>
#include<windows.h>

#include"Strategy.h"


#define FRONT_ADDR		"tcp://180.166.0.226:28213"
#define TRADE_ADDR		"tcp://180.166.0.226:28205"
#define BROKER_ID		"0034"
#define USER_NAME		"1052095"
#define PASSWD			"801900"
#define INSTRUMENT_ID	"ta1409"

static unsigned long long time_convert(int year, int month, int day, int hour, int min, int sec, int micro_sec)
{
	struct tm timein;
	timein.tm_year = year;
	timein.tm_mon = month-1;
	timein.tm_mday = day;
	timein.tm_hour = hour;
	timein.tm_min = min;
	timein.tm_sec = sec;
	time_t tm = mktime(&timein);

	//time_t tm = time(NULL);

	return (unsigned long long)tm*1000+micro_sec;
}



using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	
	string addr;  
	string trade_addr;
	string broker;
	string user;
	string passwd;
	vector<string> IDs;
	

	DataInterface data_interface;

#if 0
	LoadData(&data_interface);

	//Data_analys_1(&data_interface);

	return 1;
#endif
#if 1
	MarketPrice MP_hd;
	config_class config("test.conf");
	config.get_string("Front_Address", addr);
	config.get_string("Trading_Address",trade_addr);
	config.get_string("Broker_ID", broker);
	config.get_string("User_name", user);
	config.get_string("PassWd", passwd);
	config.get_vector("Instrument_ID", IDs);

	MP_hd.setDataInterface(&data_interface);
	MP_hd.setconfig(addr.c_str(), broker.c_str(), user.c_str(), passwd.c_str(), IDs);

	MP_hd.StartToGetData();


	MP_hd.setDataInterface(&data_interface);
	SingleInsData Data(&data_interface, "c1409");
	
	CtpTraderSpi trader(TRADE_ADDR, BROKER_ID, USER_NAME, PASSWD);

	

	/*TThostFtdcBrokerIDType	BROKER_ID_;
	TThostFtdcInvestorIDType INVESTOR_ID;
	TThostFtdcPasswordType	PASSWORD;
	strcpy(BROKER_ID_,broker.c_str());
	strcpy(INVESTOR_ID, user.c_str());
	strcpy(PASSWORD , passwd.c_str());*/


	//CtpTraderSpi trader(TRADE_ADDR, BROKER_ID_, INVESTOR_ID, PASSWORD);
	//CtpTraderSpi trader(trade_addr.c_str(), broker.c_str(), user.c_str(), passwd.c_str());
	trader.StartToTrade();
	//while(1)
		ShowTraderCommand(&trader);


	Strategy strategy(&Data,&trader);
	strategy.Strate_MA(20);

	int i ; cin>>i;


	//MP_hd.WaitForJoin();


#endif
	
	return 0;
}


//test program
#if 0
FILE *fp = fopen("data.log", "r");
for(int i = 0 ; i < 100; i++)
{
	CThostFtdcDepthMarketDataField data1;
	fread(&data1, sizeof(data1), 1, fp );

	CThostFtdcDepthMarketDataField *data = &data1;

	int allday = atoi(data->TradingDay);
	int hour ;
	int min;
	int sec;

	struct data_type mydata;

	sscanf(data->UpdateTime, "%d:%d:%d", &hour, &min, &sec);

	unsigned long long tm =time_convert(allday/10000 - 1900, (allday%10000)/100, allday%100, hour, min, sec, data->UpdateMillisec);

	mydata.version = 0x1;
	mydata.ask_price[0] = data->AskPrice1;
	mydata.ask_volume[0] = data->AskVolume1;
	mydata.bid_price[0] = data->BidPrice1;
	mydata.bid_volume[0] = data->BidVolume1;

	mydata.last_Price = data->LastPrice;
	mydata.SettlementPrice = data->SettlementPrice;
	mydata.turnover = data->Turnover;
	mydata.volume = data->Volume;

	mydata.average_price = data->AveragePrice;
	mydata.ClosePrice = data->ClosePrice;
	mydata.CurrDelta = data->CurrDelta;
	mydata.HighestPrice = data->HighestPrice;
	mydata.LowestPrice = data->LastPrice;
	strcpy(mydata.IDs, data->InstrumentID);
	mydata.PreDelta = data->PreDelta;
	mydata.tm = tm;

	mydata.OpenPrice = data->OpenPrice;

	data_interface.PutData(data->InstrumentID, 0, tm , mydata );






	//int cc = 0;
	//data_interface.Get_current_Data_start(data.InstrumentID, 0 , tm -1,  dataout);

	//data_interface.Get_rid_Data();
}

vector<struct data_type> dataout;
data_interface.GetData("au1312", 0, 0 , 999999999999999ll,  dataout);



#endif