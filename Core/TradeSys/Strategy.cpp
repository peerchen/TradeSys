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

			// ��������
			MA_20.Set_MA_para(20);
			MA_20.cal_MA(data, &ma_20);
			//ma20 should return all instrument price and strategy will not know the instrument

			ma_20 = 2362; //for c1409

			//��ѯ�ֲ֡�����
			m_pTrader->ReqQryTradingAccount(); 
			m_pTrader->ReqQryInvestorPosition("");
			//�볡����
			//check if have return else
			if(last_Price>ma_20+2) 
			{
				//��Լ ���� (0:buy, 1:sell) ��ƽ(0:��,  1:ƽ )  �۸� ���� ֹ��;
				m_pTrader->ReqOrderInsert("c1409",0,"0",last_Price+1,1,0);
			}
			
			//ֹ��ƽ��
			if(last_Price<ma_20-2)
				m_pTrader->ReqOrderInsert("c1409",1,"1",last_Price-1,1,0);
			//ֹӯƽ��
			if(last_Price<ma_20+4)
				m_pTrader->ReqOrderInsert("c1409",1,"1",last_Price+1,1,0);
		
			return;
			}
		}
			
	}

	//

	

}