#ifndef __TRADER__
#define __TRADER__

#include <iostream>
#include <vector>

#include "api/ThostFtdcTraderApi.h"

#include "DataInterface.h"
using namespace std;

typedef void *	HANDLE;

/* 
	�û���Ϊ:
		1)����  OnFrontConnected
		2)��¼	ReqUserLogin
		3)�ʵ�ȷ��
		4)��ѯ��Լ
		5)
*/



class CtpTraderSpi : public CThostFtdcTraderSpi
{
	
public:
	CtpTraderSpi();
	~CtpTraderSpi();

	/***************************All request and response interface************************/

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(int nReason);

	///��¼����
	void ReqUserLogin(TThostFtdcBrokerIDType	appId,  TThostFtdcUserIDType	userId,	TThostFtdcPasswordType	passwd);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	///Ͷ���߽�����ȷ��
	void ReqSettlementInfoConfirm();
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,\
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ�ʽ��˻�
	void ReqQryTradingAccount();
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, \
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	///�����ѯ��Լ
	void ReqQryInstrument(TThostFtdcInstrumentIDType instId);
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, \
		int nRequestID, bool bIsLast);

	///�����ѯͶ���ֲ߳�
	void ReqQryInvestorPosition(TThostFtdcInstrumentIDType instId);
		virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, \
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	///����¼������
	void ReqOrderInsert(TThostFtdcInstrumentIDType instId, \
		TThostFtdcDirectionType dir, TThostFtdcCombOffsetFlagType kpp, \
		TThostFtdcPriceType price,   TThostFtdcVolumeType vol, TThostFtdcPriceType deta);
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,\
		int nRequestID, bool bIsLast);


	///������������
	void ReqOrderAction(TThostFtdcSequenceNoType orderSeq);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, \
		int nRequestID, bool bIsLast);


	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse);


	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///�ɽ�֪ͨ
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

	// �Ƿ��յ��ɹ�����Ӧ
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	///����Ӧ��
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