#ifndef __TRADER__
#define __TRADER__

#include <iostream>
#include <vector>

#include "api/ThostFtdcTraderApi.h"

#include "DataInterface.h"
using namespace std;

typedef void *	HANDLE;

/* 
	用户行为:
		1)连接  OnFrontConnected
		2)登录	ReqUserLogin
		3)帐单确认
		4)查询合约
		5)
*/



class CtpTraderSpi : public CThostFtdcTraderSpi
{
	
public:
	CtpTraderSpi();
	~CtpTraderSpi();

	/***************************All request and response interface************************/

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected(int nReason);

	///登录请求
	void ReqUserLogin(TThostFtdcBrokerIDType	appId,  TThostFtdcUserIDType	userId,	TThostFtdcPasswordType	passwd);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	///投资者结算结果确认
	void ReqSettlementInfoConfirm();
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,\
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询资金账户
	void ReqQryTradingAccount();
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, \
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	///请求查询合约
	void ReqQryInstrument(TThostFtdcInstrumentIDType instId);
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, \
		int nRequestID, bool bIsLast);

	///请求查询投资者持仓
	void ReqQryInvestorPosition(TThostFtdcInstrumentIDType instId);
		virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, \
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	///报单录入请求
	void ReqOrderInsert(TThostFtdcInstrumentIDType instId, \
		TThostFtdcDirectionType dir, TThostFtdcCombOffsetFlagType kpp, \
		TThostFtdcPriceType price,   TThostFtdcVolumeType vol, TThostFtdcPriceType deta);
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,\
		int nRequestID, bool bIsLast);


	///报单操作请求
	void ReqOrderAction(TThostFtdcSequenceNoType orderSeq);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, \
		int nRequestID, bool bIsLast);


	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	virtual void OnHeartBeatWarning(int nTimeLapse);


	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);




	void PrintOrders();
	void PrintTrades();


	void StartToTrade();
	void SetConfig(const char * addr,const char * Broker,const char * user,const char * passwd/*, std::vector<std::string> IDs*/);
	void UserLogin();
	//void SetDataInterface(DataInterface * p);

	//action
	int SubmitOrder(char* instId, char dir, char* kpp, double price, unsigned int vol, int loss );
	void CancelOrder(int orderSeq);

CThostFtdcTraderApi* pUserApi;

	char FRONT_ADDR[512];		
	TThostFtdcBrokerIDType	BROKER_ID;
	TThostFtdcInvestorIDType INVESTOR_ID;
	TThostFtdcPasswordType	PASSWORD;
	char ppInstrumentID [256];

	HANDLE g_hEvent;

	int BrokerOrderSeq;


private:

	// 是否收到成功的响应
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);



	

	int requestId;

	struct{
		int frontId;
		int sessionId;
		char orderRef[13];
	}RspUserInfo;


	vector<CThostFtdcOrderField*> orderList;
	vector<CThostFtdcTradeField*> tradeList;


	//DataInterface * pDb;

};


extern "C" void ShowTraderCommand(CtpTraderSpi* p, bool print=false); 






#endif