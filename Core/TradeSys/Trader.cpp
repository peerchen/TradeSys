#include "Trader.h"


#include <windows.h>



//TThostFtdcBrokerIDType	BROKER_ID = "7060";				// ���͹�˾����
//TThostFtdcInvestorIDType INVESTOR_ID = "1052095";			// Ͷ���ߴ���
//TThostFtdcPasswordType  PASSWORD = "801900";			// �û�����



void ShowTraderCommand(CtpTraderSpi* p, bool print)
{
	if(print){
		cerr<<"-----------------------------------------------"<<endl;
		cerr<<" [1] ReqUserLogin              -- ��¼"<<endl;
		cerr<<" [2] ReqSettlementInfoConfirm  -- ���㵥ȷ��"<<endl;
		cerr<<" [3] ReqQryInstrument          -- ��ѯ��Լ"<<endl;
		cerr<<" [4] ReqQryTradingAccount      -- ��ѯ�ʽ�"<<endl;
		cerr<<" [5] ReqQryInvestorPosition    -- ��ѯ�ֲ�"<<endl;
		cerr<<" [6] ReqOrderInsert            -- ����"<<endl;
		cerr<<" [7] ReqOrderAction            -- ����"<<endl;
		cerr<<" [8] PrintOrders               -- ��ʾ����"<<endl;
		cerr<<" [9] PrintOrders               -- ��ʾ�ɽ�"<<endl;
		cerr<<" [0] Exit                      -- �˳�"<<endl;
		cerr<<"----------------------------------------------"<<endl;
	}   
	TThostFtdcDirectionType dir;
	TThostFtdcCombOffsetFlagType kpp;
	TThostFtdcPriceType price;
	TThostFtdcPriceType loss;
	TThostFtdcVolumeType vol;
	TThostFtdcSequenceNoType orderSeq;

	int cmd;  cin>>cmd;
	int requestID =0;
	switch(cmd){
	case 1: {
		CThostFtdcReqUserLoginField userLogin;
		strcpy(userLogin.BrokerID,p->BROKER_ID);
		strcpy(userLogin.UserID,p->INVESTOR_ID);
		strcpy(userLogin.Password,p->PASSWORD);
		p->pUserApi->ReqUserLogin(&userLogin,requestID++); break;
			}
	case 2: p->ReqSettlementInfoConfirm(); break;
	case 3: {
		cerr<<" ��Լ > "; //cin>>instId; 
		CThostFtdcQryInstrumentField req;
		memset(&req, 0, sizeof(req)); 
		//int iResult = p->ReqQryInstrument("FG1405"); 
		p->ReqQryInstrument("rb1405"); break;
//CThostFtdcQryInvestorPositionDetailField ReqPosition;
		//strcpy(ReqPosition.BrokerID,BROKER_ID);
		//strcpy(ReqPosition.InvestorID,INVESTOR_ID);
		//strcpy(ReqPosition.InstrumentID, "m1405");
	    //p->ReqQryInvestorPositionDetail(&ReqPosition, ++requestID);
			}
	case 4: p->ReqQryTradingAccount(); break;
	case 5: {
		cerr<<" ��Լ > "; //cin>>instId; 
		p->ReqQryInvestorPosition(""); break;
		///p->ReqQryInvestorPositionDetail();
			}
	case 6: {
		cerr<<" ��Լ > \n"; //instId = "c1405"; //cin>>instId; 
		cerr<<" ���� (0:buy, 1:sell)\n> "; cin>>dir;//dir = THOST_FTDC_D_Buy; //cin>>dir; 
		cerr<<" ��ƽ(0:��,  1:ƽ ) \n> "; cin>>kpp; //strcpy(kpp,"0");; //
		cerr<<" �۸� > \n"; cin>>price; //price = 2356 ; //
		cerr<<" ���� > \n"; vol = 1; // cin>>vol;   
		cerr<<" ֹ�� > \n"; loss = -2; //cin>>loss;
		p->ReqOrderInsert("c1405",dir,kpp,price,vol,loss); break;
			}
	case 7: { 
		cerr<<" ��� > "; cin>>orderSeq;
		p->ReqOrderAction(orderSeq);break;
			}
	case 8: p->PrintOrders();break;
	case 9: p->PrintTrades();break;
	case 0: exit(0);
	}  
	WaitForSingleObject(p->g_hEvent,INFINITE);
	ResetEvent(p->g_hEvent);
	ShowTraderCommand(p);
}





CtpTraderSpi::CtpTraderSpi()
{ 
	requestId = 0;
	memset(BROKER_ID,0,sizeof(FRONT_ADDR));
	memset(INVESTOR_ID,0,sizeof(FRONT_ADDR));
	memset(PASSWORD,0,sizeof(FRONT_ADDR));
	memset(ppInstrumentID,0,sizeof(ppInstrumentID));
	//pDb = NULL;
	
}

CtpTraderSpi::~CtpTraderSpi()
{
	pUserApi->Release();
}

void CtpTraderSpi::StartToTrade()
{
	//��ʼ��UserApi
	CThostFtdcTraderApi* _pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();

	_pUserApi->RegisterSpi(this);									// ע���¼���
	_pUserApi->SubscribePublicTopic(THOST_TERT_RESUME);				// ע�ṫ����
	_pUserApi->SubscribePrivateTopic(THOST_TERT_RESUME);			 // ע��˽����
	_pUserApi->RegisterFront(FRONT_ADDR);							// ע�ύ��ǰ�õ�ַ

	_pUserApi->Init();

	this->pUserApi =_pUserApi;


	UserLogin();
	//pUserApi->Join();
}

void CtpTraderSpi::UserLogin()
{
	int i;
	Sleep(1000);
	ReqUserLogin(BROKER_ID,INVESTOR_ID,PASSWORD);
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);

	ReqSettlementInfoConfirm();
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);

}

void CtpTraderSpi::SetConfig(const char * addr,const char * Broker,const char * user,const char * passwd)
{
	strcpy( FRONT_ADDR ,addr );
	strcpy(BROKER_ID,Broker);
	strcpy(INVESTOR_ID, user);
	strcpy(PASSWORD , passwd);
}

//strategy model will do the price check and call the Trader

//void CtpTraderSpi::SetDataInterface(DataInterface * db)
//{
//	pDb = db;
//
//	return;
//}

void CtpTraderSpi::CancelOrder(int orderSeq)
{
	ReqOrderAction(orderSeq);
}

int CtpTraderSpi::SubmitOrder(char* instId, char dir, char* kpp, double price, unsigned int vol, int loss )
{
	ReqOrderInsert(instId,dir,kpp,price,vol,loss);
	return BrokerOrderSeq;
}



void CtpTraderSpi::OnFrontConnected()
{
	cerr<<" ���ӽ���ǰ��...�ɹ�"<<endl;
	SetEvent(g_hEvent);
}

void CtpTraderSpi::OnFrontDisconnected(int nReason)
{
	//��������������API���Զ��������ӣ��ͻ��˿ɲ�������
	cerr<<" ��Ӧ | �����ж�..." 
		<< " reason=" << nReason << endl;
}

void CtpTraderSpi::ReqUserLogin(TThostFtdcBrokerIDType	vAppId,
								TThostFtdcUserIDType	vUserId,	TThostFtdcPasswordType	vPasswd)
{

	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, vAppId); 
	strcpy(req.UserID, vUserId);  
	strcpy(req.Password, vPasswd);
	int ret = pUserApi->ReqUserLogin(&req, ++requestId);
	cerr<<" ���� | ���͵�¼..."<<((ret == 0) ? "�ɹ�" :"ʧ��") << endl;	
}


void CtpTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
								  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ( pRspUserLogin ) {  
		// ����Ự����	
		RspUserInfo.frontId = pRspUserLogin->FrontID;
		RspUserInfo.sessionId = pRspUserLogin->SessionID;
		int nextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		sprintf(RspUserInfo.orderRef, "%d", ++nextOrderRef);
		cerr<<" ��Ӧ | ��¼�ɹ�...��ǰ������:"
			<<pRspUserLogin->TradingDay<<endl;     
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	int ret = pUserApi->ReqSettlementInfoConfirm(&req, ++requestId);
	cerr<<" ���� | ���ͽ��㵥ȷ��..."<<((ret == 0)?"�ɹ�":"ʧ��")<<endl;
}


void CtpTraderSpi::OnRspSettlementInfoConfirm(
	CThostFtdcSettlementInfoConfirmField  *pSettlementInfoConfirm, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if( pSettlementInfoConfirm){
		/*cerr<<" ��Ӧ | ���㵥..."<<pSettlementInfoConfirm->InvestorID
		<<"...<"<<pSettlementInfoConfirm->ConfirmDate
		<<" "<<pSettlementInfoConfirm->ConfirmTime<<">...ȷ��"<<endl;*/
	}
	if(bIsLast) 
		SetEvent(g_hEvent);
}



void CtpTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, "7060");
	strcpy(req.InvestorID, "1052095");
	int ret = pUserApi->ReqQryTradingAccount(&req, ++requestId);
	cerr<<" ���� | �����ʽ��ѯ..."<<((ret == 0)?"�ɹ�":"ʧ��")<<endl;

}

void CtpTraderSpi::OnRspQryTradingAccount(
	CThostFtdcTradingAccountField *pTradingAccount, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if (pTradingAccount){
		cerr<<" ��Ӧ | Ȩ��:"<<pTradingAccount->Balance
			<<" ����:"<<pTradingAccount->Available   
			<<" ��֤��:"<<pTradingAccount->CurrMargin
			<<" ƽ��ӯ��:"<<pTradingAccount->CloseProfit
			<<" �ֲ�ӯ��"<<pTradingAccount->PositionProfit
			<<" ������:"<<pTradingAccount->Commission
			<<" ���ᱣ֤��:"<<pTradingAccount->FrozenMargin
			//<<" ����������:"<<pTradingAccount->FrozenCommission 
			<< endl;    
	}
	if(bIsLast) SetEvent(g_hEvent);
}


void CtpTraderSpi::ReqQryInstrument(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, instId);
	int ret = pUserApi->ReqQryInstrument(&req, ++requestId);
	cerr<<" ���� | ���ͺ�Լ��ѯ..."<<((ret == 0)?"�ɹ�":"ʧ��")<<endl;
}

void CtpTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, 
									  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 	
	if (pInstrument){
		cerr<<" ��Ӧ | ��Լ:"<<pInstrument->InstrumentID
			<<" ������:"<<pInstrument->DeliveryMonth
			<<" ��ͷ��֤����:"<<pInstrument->LongMarginRatio
			<<" ��ͷ��֤����:"<<pInstrument->ShortMarginRatio<<endl;    
	}
	if(bIsLast) SetEvent(g_hEvent);
}



void CtpTraderSpi::ReqQryInvestorPosition(TThostFtdcInstrumentIDType instId)
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, "7060");
	strcpy(req.InvestorID, "1052095");
	strcpy(req.InstrumentID, instId);	
	int ret = pUserApi->ReqQryInvestorPosition(&req, ++requestId);
	cerr<<" ���� | ���ͳֲֲ�ѯ..."<<((ret == 0)?"�ɹ�":"ʧ��")<<endl;
}

char MapDirection(char src, bool toOrig=true){
	if(toOrig){
		if('b'==src||'B'==src){src='0';}else if('s'==src||'S'==src){src='1';}
	}else{
		if('0'==src){src='B';}else if('1'==src){src='S';}
	}
	return src;
}

char MapOffset(char src, bool toOrig=true){
	if(toOrig){
		if('o'==src||'O'==src){src='0';}
		else if('c'==src||'C'==src){src='1';}
		else if('j'==src||'J'==src){src='3';}
	}else{
		if('0'==src){src='O';}
		else if('1'==src){src='C';}
		else if('3'==src){src='J';}
	}
	return src;
}



void CtpTraderSpi::OnRspQryInvestorPosition(
	CThostFtdcInvestorPositionField *pInvestorPosition, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if(pInvestorPosition ){
		cerr<<" ��Ӧ | ��Լ:"<<pInvestorPosition->InstrumentID
			<<" ����:"<<MapDirection(pInvestorPosition->PosiDirection)
			<<" �ֲܳ�:"<<pInvestorPosition->Position
			<<" ���:"<<pInvestorPosition->YdPosition 
			<<" ���:"<<pInvestorPosition->TodayPosition
			<<" �ɱ�:"<<pInvestorPosition->OpenCost
			<<" �ֲ�ӯ��:"<<pInvestorPosition->PositionProfit
			<<" ��֤��:"<<pInvestorPosition->UseMargin<<endl;
	}
	if(bIsLast) SetEvent(g_hEvent);	
}


void CtpTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

bool CtpTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (ret){
		cerr<<" ��Ӧ | "<<pRspInfo->ErrorMsg<<endl;
	}
	return ret;
}

//

void CtpTraderSpi::ReqOrderInsert(TThostFtdcInstrumentIDType instId,
								  TThostFtdcDirectionType dir, TThostFtdcCombOffsetFlagType kpp,
								  TThostFtdcPriceType price,   TThostFtdcVolumeType vol, TThostFtdcPriceType deta)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));	
	strcpy(req.BrokerID, "7060");  //Ӧ�õ�Ԫ����	
	strcpy(req.InvestorID, "1052095"); //Ͷ���ߴ���	
	strcpy(req.InstrumentID, instId); //��Լ����	
	strcpy(req.OrderRef, RspUserInfo.orderRef);  //��������
	int nextOrderRef = atoi(RspUserInfo.orderRef);
	sprintf(RspUserInfo.orderRef, "%d", ++nextOrderRef);

	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;//�۸�����=�޼�	
	req.Direction = MapDirection(dir,true);  //��������	
	req.CombOffsetFlag[0] = MapOffset(kpp[0],true); //THOST_FTDC_OF_Open; //��Ͽ�ƽ��־:����
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	  //���Ͷ���ױ���־
	req.LimitPrice = price;	//�۸�
	req.VolumeTotalOriginal = vol;	///����	
	req.TimeCondition = THOST_FTDC_TC_GFD;  //��Ч������:������Ч
	req.VolumeCondition = THOST_FTDC_VC_AV; //�ɽ�������:�κ�����
	req.MinVolume = 1;	//��С�ɽ���:1	
	req.ContingentCondition = THOST_FTDC_CC_Immediately;  //��������:����

	TThostFtdcPriceType	StopPrice =price + deta ;  //ֹ���
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;	//ǿƽԭ��:��ǿƽ	
	req.IsAutoSuspend = 0;  //�Զ������־:��	
	req.UserForceClose = 0;   //�û�ǿ����־:��

	int ret = pUserApi->ReqOrderInsert(&req, ++requestId);
	cerr<<" ���� | ���ͱ���..."<<((ret == 0)?"�ɹ�":"ʧ��")<< endl;
}

void CtpTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
									CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInputOrder ){
		cerr<<"��Ӧ | �����ύ�ɹ�...��������:"<<pInputOrder->OrderRef<<endl;  
	}
	if(bIsLast) SetEvent(g_hEvent);	
}




void CtpTraderSpi::ReqOrderAction(TThostFtdcSequenceNoType orderSeq)
{
	bool found=false; unsigned int i=0;
	for(i=0;i<orderList.size();i++){
		if(orderList[i]->BrokerOrderSeq == orderSeq){ found = true; break;}
	}
	if(!found){cerr<<" ���� | ����������."<<endl; return;} 

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, "7060");   //���͹�˾����	
	strcpy(req.InvestorID, "1052095"); //Ͷ���ߴ���
	//strcpy(req.OrderRef, pOrderRef); //��������	
	//req.FrontID = frontId;           //ǰ�ñ��	
	//req.SessionID = sessionId;       //�Ự���
	strcpy(req.ExchangeID, orderList[i]->ExchangeID);
	strcpy(req.OrderSysID, orderList[i]->OrderSysID);
	req.ActionFlag = THOST_FTDC_AF_Delete;  //������־ 

	int ret = pUserApi->ReqOrderAction(&req, ++requestId);
	cerr<< " ���� | ���ͳ���..." <<((ret == 0)?"�ɹ�":"ʧ��") << endl;
}

void CtpTraderSpi::OnRspOrderAction(
									CThostFtdcInputOrderActionField *pInputOrderAction, 
									CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if (!IsErrorRspInfo(pRspInfo) && pInputOrderAction){
		cerr<< " ��Ӧ | �����ɹ�..."
			<< "������:"<<pInputOrderAction->ExchangeID
			<<" �������:"<<pInputOrderAction->OrderSysID<<endl;
	}
	if(bIsLast) SetEvent(g_hEvent);	
}


void CtpTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr<<" ��Ӧ | ������ʱ����..." 
		<< " TimerLapse = " << nTimeLapse << endl;
}



///�ɽ�֪ͨ
void CtpTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	CThostFtdcTradeField* trade = new CThostFtdcTradeField();
	memcpy(trade,  pTrade, sizeof(CThostFtdcTradeField));
	bool founded=false;     unsigned int i=0;
	for(i=0; i<tradeList.size(); i++){
		if(tradeList[i]->TradeID == trade->TradeID) {
			founded=true;   break;
		}
	}
	if(founded) tradeList[i] = trade;   
	else  tradeList.push_back(trade);
	cerr<<" �ر� | �����ѳɽ�...�ɽ����:"<<trade->TradeID<<endl;
	SetEvent(g_hEvent);
}


void CtpTraderSpi::PrintOrders(){
	CThostFtdcOrderField* pOrder; 
	for(unsigned int i=0; i<orderList.size(); i++){
		pOrder = orderList[i];
		cerr<<" ���� | ��Լ:"<<pOrder->InstrumentID
			<<" ����:"<<MapDirection(pOrder->Direction,false)
			<<" ��ƽ:"<<MapOffset(pOrder->CombOffsetFlag[0],false)
			<<" �۸�:"<<pOrder->LimitPrice
			<<" ����:"<<pOrder->VolumeTotalOriginal
			<<" ���:"<<pOrder->BrokerOrderSeq 
			<<" �������:"<<pOrder->OrderSysID
			<<" ״̬:"<<pOrder->StatusMsg<<endl;
	}
	SetEvent(g_hEvent);
}


void CtpTraderSpi::PrintTrades(){
	CThostFtdcTradeField* pTrade;
	for(unsigned int i=0; i<tradeList.size(); i++){
		pTrade = tradeList[i];
		cerr<<" �ɽ� | ��Լ:"<< pTrade->InstrumentID 
			<<" ����:"<<MapDirection(pTrade->Direction,false)
			<<" ��ƽ:"<<MapOffset(pTrade->OffsetFlag,false) 
			<<" �۸�:"<<pTrade->Price
			<<" ����:"<<pTrade->Volume
			<<" �������:"<<pTrade->OrderSysID
			<<" �ɽ����:"<<pTrade->TradeID<<endl;
	}
	SetEvent(g_hEvent);
}



///�����ر�
void CtpTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{	
	CThostFtdcOrderField* order = new CThostFtdcOrderField();
	memcpy(order,  pOrder, sizeof(CThostFtdcOrderField));
	bool founded=false;    unsigned int i=0;
	for(i=0; i<orderList.size(); i++){
		if(orderList[i]->BrokerOrderSeq == order->BrokerOrderSeq) {
			founded=true;    break;
		}
	}
	if(founded) orderList[i]= order;   
	else  orderList.push_back(order);
	cerr<<" �ر� | �������ύ...���:"<<order->BrokerOrderSeq<<endl;
	BrokerOrderSeq = order->BrokerOrderSeq;
	SetEvent(g_hEvent);	
}
