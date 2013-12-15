
#ifdef DATA_TYPE_H
#else
#define DATA_TYPE_H

struct day_data_type
{
	
};


struct data_type
{
	unsigned int version; //the version of this data type
	char IDs[32];
	unsigned long long tm;
	
	double last_Price;//current price
	int volume; 
	
	double turnover;//should be price*volume

	int open_interest;//hold in hand

	//for history data ,we may just have 1
	double ask_price[5];
	int ask_volume[5];
	double bid_price[5];
	int bid_volume[5];

	double average_price;//of today

//these data , we just save now
	///����
	double	OpenPrice;
	///��߼�
	double	HighestPrice;
	///��ͼ�
	double	LowestPrice;
	///������
	double	ClosePrice;
	///���ν����
	double	SettlementPrice;
	///��ͣ���
	double	UpperLimitPrice;
	///��ͣ���
	double	LowerLimitPrice;
	///����ʵ��
	double	PreDelta;
	///����ʵ��
	double	CurrDelta;
};

#endif