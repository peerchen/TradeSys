#ifndef __STRATEGY__
#define __STRATEGY__

#include "Indicate.h"
#include "DataInterface.h"
#include "Trader.h"

class Strategy
{
public:
	Strategy(SingleInsData* pData,/* Instrument_base_Indicate* indicate, */CtpTraderSpi* pTrader);
	~Strategy();

	void Strate_MA(int nDay);
	void Strate_LOW_HIGH(int nDay);
	void Strate_MA_BAIS(int nDay);

private:
	SingleInsData* m_pData;
	//Instrument_base_Indicate* m_pIndicate;
	CtpTraderSpi* m_pTrader;

};


#endif