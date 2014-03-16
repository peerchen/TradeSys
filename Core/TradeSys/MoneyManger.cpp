
#include "MoneyManger.h"


MoneyManger::MoneyManger(int initial_in)
{
	initial = initial_in;
	current = initial;
}

int MoneyManger::Buy(char * id, int price_, int count, int direction)
{
#if 0
	for(i = 0 ; i < holding.size(); i++)
	{
		if(!strcmp(id, holding[i].id))
		{
			return 1;
		}
	}
#endif
	

	struct Holding_struct c;

	int i;
	memset(&c, 0x00, sizeof(c));

	memcpy(c.id, id, strlen(id));
	c.buy_price = price_;
	c.holding = count;
	c.direction = direction;

	holding.push_back(c);

	current -= c.buy_price*c.holding;

	return 1;
}

int MoneyManger::Sell(char * id, int price_, int count)
{

	current += price_*count;

	return 1;
}
