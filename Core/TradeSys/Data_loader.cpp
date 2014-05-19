// QDA_Parse.cpp : Defines the entry point for the console application.
//

#include <time.h>
#include "Data_loader.h"


struct UnkownData
{
	union
	{
		char a[4];
		
		int b;
		float c;
		short e[2];
	};
};

struct header
{
	int Header;
	int type;
	int StockCount;
};

struct data_struct
{
	char Id[12];
	char Name[12];
	int DayCount;
};

struct day_struct
{
	int Tm;
	float OpenPrice;
	float HighPrice;
	float LowPrice;
	float ClosePrice;
	float Volum;
	float amount;
	int DealCount;
};


DataInterface * db;

void ReaddayFile_QDA()
{
	FILE * fp = fopen("SQ2007.QDA", "rb");

	struct header header;
	struct data_struct data;
	struct day_struct day;
	int i ; 
	int j;

	fread(&header, sizeof(header), 1, fp);

	FILE *out = fopen("01.data", "w");

	for(i = 0 ; i < header.StockCount; i++)
	{
		fread(&data, sizeof(data), 1, fp);
		fprintf(out, "%s  %d\n", data.Id, data.DayCount);
		for(j = 0 ; j < data.DayCount; j++)
		{
			fread(&day, sizeof(day), 1, fp);
			fprintf(out , "%d,%f,%f,%f,%f,%f,%f,%d \n", day.Tm, day.OpenPrice, day.HighPrice, day.LowPrice, day.ClosePrice,
				day.Volum, day.amount, day.DealCount);
		}
	}
	fclose(out);


	fclose(fp);
};


struct QRT_header
{
	int Magic;
	int Version;
	int Unkown;
	int RecoderSize;
	int Unkown2;
	int DataStart;
	int Unkown3;
	int CodeNum;
};



struct QRT_Iterm1
{
	char ID[10];
};
struct QRT_Iterm2
{
	int tick_count;
	short tick_Position[33];
	short unkown_1;
	struct UnkownData a[3];
	
	float SettlementPrice;
	float OpenPrice;
	float HighPrice;
	float LowPrice;//?
	float ClosePrice;
	float TdVolume;
	struct UnkownData b;
	float Position;//持仓
	struct UnkownData c;
	float change;//涨跌
	struct UnkownData d;
	float changeRation;//涨跌幅

	struct UnkownData e[28];

	//float f_data[61];
};


struct tick_data
{
	unsigned int tm;
	float price;
	float amount;//成交量
	float amountPrice;//成交额
	short buy[3];
	short sell[3];
	char AbsBuyP[3];
	char AbsSellP[3];
	char unknow[2]; 
};

float prev = 0;

unsigned long long prev_tm = -1;

char ID_convert(char * out, char * in_ID, int year, int month)
{
	int i = 0; 
	char tt[32];
	int tt_count =0 ;
	int num = 0;
	memset(tt, 0x00, 32);
	memset(out, 0x00, 32);

	for(i = 0 ; i < strlen(in_ID); i++)
	{
		if(in_ID[i] >= '0' && in_ID[i] <= '9')
		{
			tt[tt_count++] = in_ID[i];
		}
		else
		{
			out[i] = in_ID[i];
		}
	}
	if(tt_count > 0)
	{
		num = atoi(tt);

		if(tt_count == 2)
		{
			if(num <= month)
			{
				year++;
			}
			num += year*100;
		}

		sprintf(out, "%s%d", out, num);
	}
	return 1;
}

void tick_data_output(FILE* fp, struct tick_data * data  , char * IDs)
{
	unsigned  long long iTime = data->tm - 8*60*60;	//UTC to +8 tm
	time_t tt= data->tm - 8*60*60;
	struct tm *t = localtime(&tt);
	struct tick_data_type mydata;

	mydata.version = 0x2;
	mydata.ask_price[0] = 0;
	mydata.ask_volume[0] = 0;
	mydata.bid_price[0] = 0;
	mydata.bid_volume[0] = 0;

	mydata.last_Price = data->price;
	//mydata.SettlementPrice = data->SettlementPrice;
	//mydata.turnover = data->Turnover;
	mydata.volume = data->amount -prev;

	//mydata.average_price = data->AveragePrice;
	//mydata.ClosePrice = data->ClosePrice;
	//mydata.CurrDelta = data->CurrDelta;
	//mydata.HighestPrice = data->HighestPrice;
	//mydata.LowestPrice = data->LastPrice;
	//strcpy(mydata.IDs, IDs);

	ID_convert(mydata.IDs , IDs, t->tm_year - 100, t->tm_mon+1);
	//mydata.PreDelta = data->PreDelta;
	mydata.tm = iTime;
	

	static int counte = 0;
	
	if(iTime == prev_tm)
	{
		mydata.tm = iTime*1000 + 500*counte;
		if(counte == 2)
		{
			mydata.tm -= 200;
		}
		counte++;
	}
	else
	{
		mydata.tm = iTime*1000;
		counte = 0;
	}

	prev_tm = iTime;



	//mydata.OpenPrice = data->OpenPrice;


	db->Put_Tick_Data_batch(mydata.IDs, 0, mydata.tm , mydata );
	//fprintf(fp, "%d%02d%02d , %02d:%02d:%02d ,%f ,%f , %f\n",t->tm_year+1900,t->tm_mon+1, t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec , data->price, data->amount, data->amount -prev);	
	prev = data->amount;
}


void Readtick_QRT(char * filename)
{
	FILE * fp = fopen(filename, "rb");

	if(!fp)
		return ;

	unsigned long sek = fseek(fp, 0 , SEEK_END);
	unsigned long size_a=ftell(fp);
	char * dd = (char *)malloc(size_a);

	fseek(fp, 0 , SEEK_SET);
	fread(dd, size_a, 1, fp );
	fclose(fp);

	printf("%s \n", filename);

	struct QRT_header * header;
	struct QRT_Iterm1 * ID;
	struct QRT_Iterm2  Data;
	//struct day_struct day;
	int i ; 
	int j;

	FILE *out = NULL;// = fopen("QRT_01.data", "w");

	//fread(&header, sizeof(header), 1, fp);

	header = (struct QRT_header * )dd;

	db->start_batch();
	//float  prev = 0;;

	for(i = 0 ; i  < header->CodeNum ; i++)
	{
		int offset = sizeof(struct QRT_header) + i*(sizeof(struct QRT_Iterm1)+sizeof(struct QRT_Iterm2));
		//fseek(fp, sizeof(struct QRT_header) + i*(sizeof(struct QRT_Iterm1)+sizeof(struct QRT_Iterm2)) , SEEK_SET);
		//fread(&(ID), sizeof(struct QRT_Iterm1), 1 , fp);
		ID  = (struct QRT_Iterm1 *) (dd+ offset);

		offset += sizeof(struct QRT_Iterm1);
		//fread(&(Data), sizeof(struct QRT_Iterm2), 1 , fp);
		memcpy( &Data, (dd+ offset), sizeof(struct QRT_Iterm2));
		

		//char output_file[256];
		//sprintf(output_file, "%s.txt", ID.ID);
		//out = fopen(output_file, "a+");
		
		prev = 0;
	
		//fprintf(out,"%s\n", ID.ID);

		if(Data.tick_count < 10)
			int cc = 0;

		int total = 0;

		int m = 0;
		for(int m = 0 ; m < 33; m++)
		{
			if(Data.tick_Position[m] == -1)
				break;
			//fseek(fp, header.DataStart + (header.RecoderSize*sizeof(struct tick_data)*Data.tick_Position[m]) , SEEK_SET);

			struct tick_data  tData;
			for(j = 0 ; j < header->RecoderSize; j++)
			{
				//fread(&tData, sizeof(tData), 1, fp);
				memcpy(&tData ,dd+header->DataStart + (header->RecoderSize*sizeof(struct tick_data)*Data.tick_Position[m]) + j*sizeof(tData), sizeof(tData));
				total ++;

				tick_data_output(out,& tData, ID->ID);
#if 0
				time_t iTime = tData.tm - 8*60*60;	//UTC to +8 tm		
				struct tm *t = localtime(&iTime);
				
				fprintf(out, "%d%02d%02d  %02d:%02d:%02d %f %f \n",t->tm_year+1900,t->tm_mon+1, t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec , tData.price, tData.amount - prev);
				prev = tData.amount;
#endif

				if(total >= Data.tick_count)
					break;
			}
		}

	//	fclose(out);

		//fprintf(out,"\n\n");
	}

	db->end_batch();
	
	//fclose(fp);
};

int Readtick_QM1(char * filename, int year, int mon)
{
	FILE * fp = fopen(filename, "rb");

	if(!fp)
		return 1;

	printf("%s \n", filename);

	struct FileHeaderType
	{
		struct UnkownData U[2];
		int ItemCount;
	}header;


	struct ItemHeaderType
	{
		char Name1[8];//ag01
		struct UnkownData N1;
		char Name2[12];//by1501
		//struct UnkownData header[64];
		int min_size;
	}item_header;


	fread(&header, 1, sizeof(header), fp);


	db->start_batch();

	for(int i = 0 ; i <header.ItemCount ; i++ )
	{
		fread(&item_header, 1, sizeof(item_header), fp);

		struct MinDataType
		{
			int tm;
			float open;
			float high;
			float low;
			float close;
			//struct UnkownData N2;
			float volume;
			float holding;
			int tt;
		}min_data;
		
		char ID[32];

		memmove(item_header.Name1, item_header.Name1+2, 10);

		ID_convert(ID, item_header.Name1, year - 2000, mon);

		int PutFlag = 0;

		if(!strcmp(ID, "agMI"))
		{
			PutFlag = 1;
		}

		//char output_file[256];
		//sprintf(output_file, "%s.txt",ID);
		//FILE * out = fopen(output_file, "w");

		int count = 0;
		for(int i = 0 ; i < item_header.min_size; i++)
		{
			fread(&min_data, 1, sizeof(min_data), fp);

			time_t tt= min_data.tm - 8*60*60;
			struct tm *t = localtime(&tt);

			k_data_type one_data;
			memset(&one_data, 0x00, sizeof(one_data));

			one_data.tm = tt;
			one_data.high = min_data.high;
			one_data.low = min_data.low;
			one_data.open_price = min_data.open;
			one_data.close_price = min_data.close;
			one_data.volume = min_data.volume;
			one_data.holding = min_data.holding;


			
			char key[256];

			memset(key, 0x00, sizeof(key));
			create_1min_key(key, ID, 0, one_data.tm*1000);

			if(PutFlag)
				db->Put_Data_batch( key, &one_data , sizeof(one_data));

			//fprintf(out, "%d%02d%02d  %02d:%02d:%02d %f %f %f %f %f %f %d\n" ,t->tm_year+1900,t->tm_mon+1, t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec,
			//min_data.D1, min_data.D2, min_data.D3, min_data.D4 , min_data.volume1, min_data.volume, min_data.tt);
			//printf("%d%02d%02d  %02d:%02d:%02d\n" ,t->tm_year+1900,t->tm_mon+1, t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec);	
			count++;
		}

		//fclose(out);
	}
	
	db->end_batch();

	

	return 1;
}


DataInterface * In;

int LoadData_Inter(DataInterface * interface)
{
	int i = 0 ;
	int j;
	
	db = interface;


	//Readtick_QRT( "D:\\github\\data\\20130916.QRT");
	//Readtick_QRT( "D:\\github\\data\\20130917.QRT");

	return 2;
	for(j = 12; j > 11; j--)
	{
		for(i = 16 ; i < 32; i++)
		{
			char  files[256];
			//sprintf(files, "D:\\github\\data\\20140124.QRT");
			sprintf(files, "D:\\github\\data\\2013%02d%02d.QRT",j ,i);
			//Readtick_QRT(files);
			Readtick_QRT(files);
			//break;
		}
		//break;
	}
	
	return 0;
}

void load_min_data()
{
	int year = 2013;
	for(int i = 1 ; i < 13; i++)
	{
		for(int j = 0 ; j < 32; j++)
		{
			char  files[256];
			sprintf(files, "D:\\github\\min_data\\sQ%04d%02d%02dm1\\Quote.QM1",year, i ,j);
			Readtick_QM1(files,year, i);
		}
	}
}


#include <stdio.h>
#include <Windows.h>

/*
DWORD WINAPI ThreadFun(LPVOID pM)
{
	LoadData_Inter(In);
	
	return 0;
}

DWORD WINAPI ThreadFun_1(LPVOID pM)
{
	LoadData_Inter_1(In);

	return 0;
}
*/
//主函数，所谓主函数其实就是主线程执行的函数。
int LoadData(DataInterface * interfaceM)
{
	In = interfaceM;


	db = interfaceM;

	//load_min_data();
	
	LoadData_Inter(db);
	

	//ThreadFun(NULL);

//	HANDLE handle1 = CreateThread(NULL, 0, ThreadFun_1, NULL, 0, NULL);
//	HANDLE handle = CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL);

//	WaitForSingleObject(handle, INFINITE);
	return 0;
}

