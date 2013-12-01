// QDA_Parse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <time.h>

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

void tick_data_output(FILE* fp, struct tick_data * data )
{
	time_t iTime = data->tm - 8*60*60;	//UTC to +8 tm		
	struct tm *t = localtime(&iTime);
	

	fprintf(fp, "%d%02d%02d , %02d:%02d:%02d ,%f ,%f , %f\n",t->tm_year+1900,t->tm_mon+1, t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec , data->price, data->amount, data->amount -prev);	
	prev = data->amount;
}


void Readtick_QRT(char * filename)
{
	FILE * fp = fopen(filename, "rb");

	if(!fp)
		return ;

	struct QRT_header header;
	struct QRT_Iterm1 ID;
	struct QRT_Iterm2 Data;
	struct day_struct day;
	int i ; 
	int j;

	FILE *out ;// = fopen("QRT_01.data", "w");

	fread(&header, sizeof(header), 1, fp);

	//float  prev = 0;;

	for(i = 0 ; i  < header.CodeNum ; i++)
	{

		fseek(fp, sizeof(header) + i*(sizeof(struct QRT_Iterm1)+sizeof(struct QRT_Iterm2)) , SEEK_SET);
		fread(&(ID), sizeof(struct QRT_Iterm1), 1 , fp);

		fread(&(Data), sizeof(struct QRT_Iterm2), 1 , fp);
		

		char output_file[256];
		sprintf(output_file, "%s.txt", ID.ID);
		out = fopen(output_file, "a+");
		
		prev = 0;
	
		//fprintf(out,"%s\n", ID.ID);

		int total = 0;

		int m = 0;
		for(int m = 0 ; m < 33; m++)
		{
			if(Data.tick_Position[m] == -1)
				break;
			fseek(fp, header.DataStart + (header.RecoderSize*sizeof(struct tick_data)*Data.tick_Position[m]) , SEEK_SET);

			struct tick_data tData;
			for(j = 0 ; j < header.RecoderSize; j++)
			{
				fread(&tData, sizeof(tData), 1, fp);
				total ++;
	
				tick_data_output(out, &tData);
#if 0
				time_t iTime = tData.tm - 8*60*60;	//UTC to +8 tm		
				struct tm *t = localtime(&iTime);
				
				fprintf(out, "%d%02d%02d  %02d:%02d:%02d %f %f \n",t->tm_year+1900,t->tm_mon+1, t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec , tData.price, tData.amount - prev);
				prev = tData.amount;
#endif

				if(total > Data.tick_count)
					break;
			}
		}

		fclose(out);

		//fprintf(out,"\n\n");
	}

	
	fclose(fp);
};


int _tmain(int argc, _TCHAR* argv[])
{
	int i = 0 ;
	
	for(i = 0 ; i < 32; i++)
	{
		char  files[256];

		sprintf(files, "D:\\期货\\data\\tmp\\dump_all\\201305%02d.QRT", i);
		Readtick_QRT(files);
	}

	
	
	//ReaddayFile_QDA();

	return 0;
}

