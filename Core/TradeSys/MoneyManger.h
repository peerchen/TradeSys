#include <vector>


struct Holding_struct{
	char id[32];
	int buy_price;
	int holding;
	int direction;
};


class MoneyManger
{
public:
	MoneyManger(int initial);

	int Buy(char * id, int price_, int count, int direction);
	int Sell(char * id, int price_, int count);


private:
	int initial;
	int current;

	std::vector<struct Holding_struct> holding;
};