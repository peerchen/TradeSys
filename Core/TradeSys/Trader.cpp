#include "Trader.h"


#include <windows.h>



//TThostFtdcBrokerIDType	BROKER_ID = "7060";				// 经纪公司代码
//TThostFtdcInvestorIDType INVESTOR_ID = "1052095";			// 投资者代码
//TThostFtdcPasswordType  PASSWORD = "801900";			// 用户密码



void ShowTraderCommand(CtpTraderSpi* p, bool print)
{
	if(print){
		cerr<<"-----------------------------------------------"<<endl;
		cerr<<" [1] ReqUserLogin              -- 登录"<<endl;
		cerr<<" [2] ReqSettlementInfoConfirm  -- 结算单确认"<<endl;
		cerr<<" [3] ReqQryInstrument          -- 查询合约"<<endl;
		cerr<<" [4] ReqQryTradingAccount      -- 查询资金"<<endl;
		cerr<<" [5] ReqQryInvestorPosition    -- 查询持仓"<<endl;
		cerr<<" [6] ReqOrderInsert            -- 报单"<<endl;
		cerr<<" [7] ReqOrderAction            -- 撤单"<<endl;
		cerr<<" [8] PrintOrders               -- 显示报单"<<endl;
		cerr<<" [9] PrintOrders               -- 显示成交"<<endl;
		cerr<<" [0] Exit                      -- 退出"<<endl;
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
		cerr<<" 合约 > "; //cin>>instId; 
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
		cerr<<" 合约 > "; //cin>>instId; 
		p->ReqQryInvestorPosition(""); break;
		///p->ReqQryInvestorPositionDetail();
			}
	case 6: {
		cerr<<" 合约 > \n"; //instId = "c1405"; //cin>>instId; 
		cerr<<" 方向 (0:buy, 1:sell)\n> "; cin>>dir;//dir = THOST_FTDC_D_Buy; //cin>>dir; 
		cerr<<" 开平(0:开,  1:平 ) \n> "; cin>>kpp; //strcpy(kpp,"0");; //
		cerr<<" 价格 > \n"; cin>>price; //price = 2356 ; //
		cerr<<" 数量 > \n"; vol = 1; // cin>>vol;   
		cerr<<" 止损 > \n"; loss = -2; //cin>>loss;
		p->ReqOrderInsert("c1405",dir,kpp,price,vol,loss); break;
			}
	case 7: { 
		cerr<<" 序号 > "; cin>>orderSeq;
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
	//初始化UserApi
	CThostFtdcTraderApi* _pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();

	_pUserApi->RegisterSpi(this);									// 注册事件类
	_pUserApi->SubscribePublicTopic(THOST_TERT_RESUME);				// 注册公有流
	_pUserApi->SubscribePrivateTopic(THOST_TERT_RESUME);			 // 注册私有流
	_pUserApi->RegisterFront(FRONT_ADDR);							// 注册交易前置地址

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
	cerr<<" 连接交易前置...成功"<<endl;
	SetEvent(g_hEvent);
}

void CtpTraderSpi::OnFrontDisconnected(int nReason)
{
	//当发生这个情况后，API会自动重新连接，客户端可不做处理
	cerr<<" 响应 | 连接中断..." 
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
	cerr<<" 请求 | 发送登录..."<<((ret == 0) ? "成功" :"失败") << endl;	
}


void CtpTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
								  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ( pRspUserLogin ) {  
		// 保存会话参数	
		RspUserInfo.frontId = pRspUserLogin->FrontID;
		RspUserInfo.sessionId = pRspUserLogin->SessionID;
		int nextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		sprintf(RspUserInfo.orderRef, "%d", ++nextOrderRef);
		cerr<<" 响应 | 登录成功...当前交易日:"
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
	cerr<<" 请求 | 发送结算单确认..."<<((ret == 0)?"成功":"失败")<<endl;
}


void CtpTraderSpi::OnRspSettlementInfoConfirm(
	CThostFtdcSettlementInfoConfirmField  *pSettlementInfoConfirm, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if( pSettlementInfoConfirm){
		/*cerr<<" 响应 | 结算单..."<<pSettlementInfoConfirm->InvestorID
		<<"...<"<<pSettlementInfoConfirm->ConfirmDate
		<<" "<<pSettlementInfoConfirm->ConfirmTime<<">...确认"<<endl;*/
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
	cerr<<" 请求 | 发送资金查询..."<<((ret == 0)?"成功":"失败")<<endl;

}

void CtpTraderSpi::OnRspQryTradingAccount(
	CThostFtdcTradingAccountField *pTradingAccount, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 
	if (pTradingAccount){
		cerr<<" 响应 | 权益:"<<pTradingAccount->Balance
			<<" 可用:"<<pTradingAccount->Available   
			<<" 保证金:"<<pTradingAccount->CurrMargin
			<<" 平仓盈亏:"<<pTradingAccount->CloseProfit
			<<" 持仓盈亏"<<pTradingAccount->PositionProfit
			<<" 手续费:"<<pTradingAccount->Commission
			<<" 冻结保证金:"<<pTradingAccount->FrozenMargin
			//<<" 冻结手续费:"<<pTradingAccount->FrozenCommission 
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
	cerr<<" 请求 | 发送合约查询..."<<((ret == 0)?"成功":"失败")<<endl;
}

void CtpTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, 
									  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ 	
	if (pInstrument){
		cerr<<" 响应 | 合约:"<<pInstrument->InstrumentID
			<<" 交割月:"<<pInstrument->DeliveryMonth
			<<" 多头保证金率:"<<pInstrument->LongMarginRatio
			<<" 空头保证金率:"<<pInstrument->ShortMarginRatio<<endl;    
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
	cerr<<" 请求 | 发送持仓查询..."<<((ret == 0)?"成功":"失败")<<endl;
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
		cerr<<" 响应 | 合约:"<<pInvestorPosition->InstrumentID
			<<" 方向:"<<MapDirection(pInvestorPosition->PosiDirection)
			<<" 总持仓:"<<pInvestorPosition->Position
			<<" 昨仓:"<<pInvestorPosition->YdPosition 
			<<" 今仓:"<<pInvestorPosition->TodayPosition
			<<" 成本:"<<pInvestorPosition->OpenCost
			<<" 持仓盈亏:"<<pInvestorPosition->PositionProfit
			<<" 保证金:"<<pInvestorPosition->UseMargin<<endl;
	}
	if(bIsLast) SetEvent(g_hEvent);	
}


void CtpTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

bool CtpTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (ret){
		cerr<<" 响应 | "<<pRspInfo->ErrorMsg<<endl;
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
	strcpy(req.BrokerID, "7060");  //应用单元代码	
	strcpy(req.InvestorID, "1052095"); //投资者代码	
	strcpy(req.InstrumentID, instId); //合约代码	
	strcpy(req.OrderRef, RspUserInfo.orderRef);  //报单引用
	int nextOrderRef = atoi(RspUserInfo.orderRef);
	sprintf(RspUserInfo.orderRef, "%d", ++nextOrderRef);

	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;//价格类型=限价	
	req.Direction = MapDirection(dir,true);  //买卖方向	
	req.CombOffsetFlag[0] = MapOffset(kpp[0],true); //THOST_FTDC_OF_Open; //组合开平标志:开仓
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	  //组合投机套保标志
	req.LimitPrice = price;	//价格
	req.VolumeTotalOriginal = vol;	///数量	
	req.TimeCondition = THOST_FTDC_TC_GFD;  //有效期类型:当日有效
	req.VolumeCondition = THOST_FTDC_VC_AV; //成交量类型:任何数量
	req.MinVolume = 1;	//最小成交量:1	
	req.ContingentCondition = THOST_FTDC_CC_Immediately;  //触发条件:立即

	TThostFtdcPriceType	StopPrice =price + deta ;  //止损价
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;	//强平原因:非强平	
	req.IsAutoSuspend = 0;  //自动挂起标志:否	
	req.UserForceClose = 0;   //用户强评标志:否

	int ret = pUserApi->ReqOrderInsert(&req, ++requestId);
	cerr<<" 请求 | 发送报单..."<<((ret == 0)?"成功":"失败")<< endl;
}

void CtpTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
									CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( !IsErrorRspInfo(pRspInfo) && pInputOrder ){
		cerr<<"响应 | 报单提交成功...报单引用:"<<pInputOrder->OrderRef<<endl;  
	}
	if(bIsLast) SetEvent(g_hEvent);	
}




void CtpTraderSpi::ReqOrderAction(TThostFtdcSequenceNoType orderSeq)
{
	bool found=false; unsigned int i=0;
	for(i=0;i<orderList.size();i++){
		if(orderList[i]->BrokerOrderSeq == orderSeq){ found = true; break;}
	}
	if(!found){cerr<<" 请求 | 报单不存在."<<endl; return;} 

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, "7060");   //经纪公司代码	
	strcpy(req.InvestorID, "1052095"); //投资者代码
	//strcpy(req.OrderRef, pOrderRef); //报单引用	
	//req.FrontID = frontId;           //前置编号	
	//req.SessionID = sessionId;       //会话编号
	strcpy(req.ExchangeID, orderList[i]->ExchangeID);
	strcpy(req.OrderSysID, orderList[i]->OrderSysID);
	req.ActionFlag = THOST_FTDC_AF_Delete;  //操作标志 

	int ret = pUserApi->ReqOrderAction(&req, ++requestId);
	cerr<< " 请求 | 发送撤单..." <<((ret == 0)?"成功":"失败") << endl;
}

void CtpTraderSpi::OnRspOrderAction(
									CThostFtdcInputOrderActionField *pInputOrderAction, 
									CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	if (!IsErrorRspInfo(pRspInfo) && pInputOrderAction){
		cerr<< " 响应 | 撤单成功..."
			<< "交易所:"<<pInputOrderAction->ExchangeID
			<<" 报单编号:"<<pInputOrderAction->OrderSysID<<endl;
	}
	if(bIsLast) SetEvent(g_hEvent);	
}


void CtpTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr<<" 响应 | 心跳超时警告..." 
		<< " TimerLapse = " << nTimeLapse << endl;
}



///成交通知
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
	cerr<<" 回报 | 报单已成交...成交编号:"<<trade->TradeID<<endl;
	SetEvent(g_hEvent);
}


void CtpTraderSpi::PrintOrders(){
	CThostFtdcOrderField* pOrder; 
	for(unsigned int i=0; i<orderList.size(); i++){
		pOrder = orderList[i];
		cerr<<" 报单 | 合约:"<<pOrder->InstrumentID
			<<" 方向:"<<MapDirection(pOrder->Direction,false)
			<<" 开平:"<<MapOffset(pOrder->CombOffsetFlag[0],false)
			<<" 价格:"<<pOrder->LimitPrice
			<<" 数量:"<<pOrder->VolumeTotalOriginal
			<<" 序号:"<<pOrder->BrokerOrderSeq 
			<<" 报单编号:"<<pOrder->OrderSysID
			<<" 状态:"<<pOrder->StatusMsg<<endl;
	}
	SetEvent(g_hEvent);
}


void CtpTraderSpi::PrintTrades(){
	CThostFtdcTradeField* pTrade;
	for(unsigned int i=0; i<tradeList.size(); i++){
		pTrade = tradeList[i];
		cerr<<" 成交 | 合约:"<< pTrade->InstrumentID 
			<<" 方向:"<<MapDirection(pTrade->Direction,false)
			<<" 开平:"<<MapOffset(pTrade->OffsetFlag,false) 
			<<" 价格:"<<pTrade->Price
			<<" 数量:"<<pTrade->Volume
			<<" 报单编号:"<<pTrade->OrderSysID
			<<" 成交编号:"<<pTrade->TradeID<<endl;
	}
	SetEvent(g_hEvent);
}



///报单回报
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
	cerr<<" 回报 | 报单已提交...序号:"<<order->BrokerOrderSeq<<endl;
	BrokerOrderSeq = order->BrokerOrderSeq;
	SetEvent(g_hEvent);	
}
