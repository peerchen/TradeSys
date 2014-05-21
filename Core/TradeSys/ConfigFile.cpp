#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "ConfigFile.h"



config_class::config_class(char * file)
{
	FILE * fp  = fopen(file, "r");


	char buff[512];
	while(fgets(buff, 512, fp))
	{
		char delims[] = " ";
		char *result = NULL;
		struct parameter para;

		result = strtok(buff , delims );
		strcpy(para.name , result);
		result = strtok(NULL , delims );
		while( result != NULL ) {
			if(result[strlen(result)-1] == '\n')
			{
				result[strlen(result)-1]  = 0;
			}
			//for(int i = 0 ; i < strlen(result); i++)
			para.value.push_back(result);
			result = strtok( NULL, delims );
		}

		priv_config.push_back(para);
	}
}

void config_class::get_string(char * name , string & data)
{
	int i = 0;
	for(i = 0 ; i < priv_config.size(); i++)
	{
		if(!strcmp(name, priv_config[i].name))
		{
			data = priv_config[i].value[0]; 
			return;
		}
	}
}
void config_class::get_vector(char * name, vector<string> &data)
{
	int i = 0;
	for(i = 0 ; i < priv_config.size(); i++)
	{
		if(!strcmp(name, priv_config[i].name))
		{
			data = priv_config[i].value;
			return ;
		}
	}
}
