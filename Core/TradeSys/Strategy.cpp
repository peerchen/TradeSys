#include "Strategy.h"
#include "Indicate.h"

Strategy::Strategy(SingleInsData* pData,/* Instrument_base_Indicate* indicate,*/ CtpTraderSpi* pTrader)
{
	assert(pData);
	//assert(indicate);
	assert(pTrader);
	m_pData = pData;
	//m_pIndicate = indicate;
	m_pTrader = pTrader;
}

Strategy::~Strategy()
{
}

void Strategy::Strate_MA(int nDay)
{
	float ma_20=0; struct rang_data_type data; memset(&data,0,sizeof(struct rang_data_type));
	Instrument_base_Indicate MA_20("ma_20");
	std::vector<struct tick_data_type > tickData;

	m_pTrader->ReqQryTradingAccount(); 
			m_pTrader->ReqQryInvestorPosition("");


	while(1)
	{
		m_pData->GetLastTickData(tickData);
		if(tickData.size())
		{
			for(vector<struct tick_data_type>::const_iterator itor=tickData.begin(); itor!=tickData.end(); itor++)
			{
			double last_Price = itor->last_Price;

			// 参数设置
			MA_20.Set_MA_para(20);
			MA_20.cal_MA(data, &ma_20);
			//ma20 should return all instrument price and strategy will not know the instrument

			ma_20 = 2362; //for c1409

			//查询持仓　数量
			m_pTrader->ReqQryTradingAccount(); 
			m_pTrader->ReqQryInvestorPosition("");
			//入场开仓
			//check if have return else
			if(last_Price>ma_20+2) 
			{
				//合约 方向 (0:buy, 1:sell) 开平(0:开,  1:平 )  价格 数量 止损;
				m_pTrader->ReqOrderInsert("c1409",0,"0",last_Price+1,1,0);
			}
			
			//止损平仓
			if(last_Price<ma_20-2)
				m_pTrader->ReqOrderInsert("c1409",1,"1",last_Price-1,1,0);
			//止盈平仓
			if(last_Price<ma_20+4)
				m_pTrader->ReqOrderInsert("c1409",1,"1",last_Price+1,1,0);
		
			return;
			}
		}
			
	}

	//

	

}