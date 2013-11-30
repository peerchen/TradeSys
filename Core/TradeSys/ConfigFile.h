#pragma once
#include <vector>
#include <string>

using namespace std;
struct parameter
{
	char name[64];
	std::vector<std::string> value;
};

class config_class
{
public:
	config_class(char * file);
	
	void get_string(char * name , string & data);
	void get_vector(char * name, vector<string> &data);


private:
	std::vector<struct parameter> priv_config;
};