#include "MarketPrice.h"
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)

// USER_API参数




void MarketPrice::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< __FUNCTION__ << endl;
	IsErrorRspInfo(pRspInfo);
}

void MarketPrice::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}
		
void MarketPrice::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void MarketPrice::OnFrontConnected()
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	///用户登录请求
	ReqUserLogin();
}

void MarketPrice::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void MarketPrice::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		// 请求订阅行情
		SubscribeMarketData();	
	}
}

void MarketPrice::SubscribeMarketData()
{
	int size = ppInstrumentID.size();
	char* *buff = new char*[size]; 
	int i = 0 ; 
	//buff[0] = 0;
	for(i = 0 ; i< ppInstrumentID.size();i++)
	{
		buff[i] = new char[ppInstrumentID[i].size()];
		strcpy(buff[i],ppInstrumentID[i].c_str());
	}

	int iResult = pUserApi->SubscribeMarketData(buff , ppInstrumentID.size());
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void MarketPrice::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << __FUNCTION__ << endl;
	cerr << "ResID " << pSpecificInstrument->InstrumentID << endl;
	cerr << "Res " << pRspInfo->ErrorID << endl;
	cerr << nRequestID << endl;
	cerr << bIsLast << endl;

	
}

void MarketPrice::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << __FUNCTION__ << endl;
}

void MarketPrice::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	cerr << __FUNCTION__ << endl;

	static FILE * pflog=fopen("data.log" ,"a+"); 

	fwrite( (void * ) pDepthMarketData, sizeof(CThostFtdcDepthMarketDataField), 1 , pflog); 
	


	cerr << pDepthMarketData->TradingDay << " " << pDepthMarketData->InstrumentID << " "
		<< pDepthMarketData->ExchangeID << " " << pDepthMarketData->ExchangeInstID << " " 
		<< pDepthMarketData->LastPrice << " " << pDepthMarketData->PreSettlementPrice<< " "
		<< pDepthMarketData->PreClosePrice << " " << pDepthMarketData->OpenInterest<< " "
		<< pDepthMarketData->AskVolume1 << " " << pDepthMarketData->BidVolume1<< endl;
}

bool MarketPrice::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}


#if 0
CThostFtdcMdApi* pUserApi;


// 配置参数
#if 0
char FRONT_ADDR[] = "tcp://asp-sim2-md1.financial-trading-platform.com:26213";		// 前置地址
TThostFtdcBrokerIDType	BROKER_ID = "2030";				// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "00092";			// 投资者代码
TThostFtdcPasswordType  PASSWORD = "888888";			// 用户密码
#endif

#if 1
char FRONT_ADDR[] = "tcp://116.228.234.67:41213";		// 前置地址
TThostFtdcBrokerIDType	BROKER_ID = "2011";				// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "020090003221";			// 投资者代码
TThostFtdcPasswordType  PASSWORD = "123321";			// 用户密码
#else


#endif
char *ppInstrumentID[] = {"au1312", "ag1312"};			// 行情订阅列表
//char *ppInstrumentID[] = {"wr1401"};	
int iInstrumentID = 2;									// 行情订阅数量

// 请求编号
int iRequestID = 0;
#endif

void MarketPrice::setconfig(const char * addr,const char * Broker,const char * user,const char * passwd, std::vector<std::string> IDs)
{
	strcpy( FRONT_ADDR ,addr );
	strcpy(BROKER_ID,Broker);
	strcpy(INVESTOR_ID, user);
	strcpy(PASSWORD , passwd);
	ppInstrumentID = IDs;

	iRequestID = 0;

}


void MarketPrice::StartToGetData()
{
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi("", true);			// 创建UserApi -- UDP mode
	//CThostFtdcMdSpi * pUserSpi = new MarketPrice();
	pUserApi->RegisterSpi(this);						// 注册事件类
	pUserApi->RegisterFront(FRONT_ADDR);					// connect
	pUserApi->Init();

	pUserApi->Join();
}
