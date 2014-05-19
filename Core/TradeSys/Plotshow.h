#include <stdio.h>
#include <stdlib.h>
#include <vector>

struct show_type
{
	std::string pos;
	std::string type;
	int map;
};

class Plotshow
{
public:
	Plotshow();
	~Plotshow();

	void reset();

	void plot_line(float data , char * type);
	void plot_line(std::vector<float> data, char * type);

	void plot_line(int map, float data , char * type);
	void plot_line(int map, std::vector<float> data, char * type);

	void show_file(char * filename);
	void line_end();
	void show();

private:
	FILE *pe;
	FILE *show_fd;
	int first_line;
	int index ;
	int multi_map;
	std::vector <struct show_type> types;
};