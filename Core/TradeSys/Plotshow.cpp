#include <assert.h>
#include <sstream>
#include "Plotshow.h"

using namespace std;

Plotshow::Plotshow()
{
	if((pe = _popen("C:\\gnuplot\\bin\\pgnuplot.exe","w"))==NULL)
		assert(0);

	show_fd = fopen("showdata.txt", "w");
	first_line = 1;
	index = 1;
	
}

Plotshow::~Plotshow()
{
	fclose(pe);
	fclose(show_fd);
}

void Plotshow::reset()
{
	fclose(show_fd);
	show_fd = fopen("showdata.txt", "w");
	first_line = 1;
	multi_map = 0;
	return;
}


void Plotshow::plot_line(int map, vector<float> data, char * type_in)
{
	for(int i = 0 ;i < data.size();i++)
	{
		fprintf(show_fd, "%f ", data[i]);
	}

	if(first_line == 1)
	{	
		struct show_type type;
		ostringstream t;
		//std::string type;
		t << "0";
		for(int i = 0 ; i < data.size(); i++)
		{
			t<<":"<< index ;
			index++;
		}

		if(map > multi_map)
			multi_map = multi_map;

		type.pos = t.str();
		string t2(type_in);;
		type.type = t2;
		type.map = map;
		types.push_back(type);
	}
}

void Plotshow::plot_line(int map, float data , char * type_in)
{
	fprintf(show_fd, "%f ", data);
	if(first_line == 1)
	{
		struct show_type type;
		ostringstream t;
		//std::string type;
		t << "0:"<<index;
		index++;;
		type.pos = t.str();
		string t2(type_in);;
		type.type = t2;
		type.map = map;
		
		if(map > multi_map)
			multi_map = map;

		types.push_back(type);
	}
}

void Plotshow::plot_line( vector<float> data, char * type_in)
{
	plot_line(0, data, type_in);
}

void Plotshow::plot_line( float data, char * type_in)
{
	plot_line(0, data, type_in);
}

void Plotshow::line_end()
{
	fprintf(show_fd, "\n");
	first_line = 0;
}

void Plotshow::show()
{

	int loop = 0;

	if(multi_map > 0)
	{
		fprintf(pe, "set size 1,1\n");
		fprintf(pe, "set origin 0,0\n");
		fprintf(pe, "set multiplot\n");
		loop = 1;
		fflush(pe);
	}

	struct  
	{
		float pos;
		float y;
	}group[3][3]
	=
	{
		{{1.0, 0} ,{1.0, 0},{1.0, 0}},
		{{0.7, 0.3} ,{0.3, 0},{1.0, 0}},
		{{0.5, 0.5} ,{0.3, 0.2 },{0.2, 0}}
	};


	fprintf(pe, "set autoscale y\n");
	fprintf(pe, "set lmargin 9\n");

	for(int j = 0 ; j <= multi_map; j++)
	{
		if(1 || j == 0 && multi_map)
		{
			fprintf(pe, "set size 1,%f\n", group[multi_map][j].pos);
			fprintf(pe, "set origin 0,%f\n", group[multi_map][j].y);
		}
		else if(j == 1)
		{
			fprintf(pe, "set size 1,0.3\n");
			fprintf(pe, "set origin 0,0\n");
		}

		
		//fprintf(pe, "unset label 2\n");
		//fprintf(pe, "unset title\n");
		fprintf(pe, "set bmargin 0\n");
		fprintf(pe, "set tmargin 0\n");
		fprintf(pe, "plot ");

		for(int i = 0; i < types.size();i++)
		{
			if(types[i].map == j)
				fprintf(pe, "'showdata.txt' using %s notitle with %s , ",types[i].pos.c_str(), types[i].type.c_str());
		}
		fprintf(pe, "\n");
		fflush(pe);
		
	}

	if(multi_map > 0)
	{
		fprintf(pe, "unset multiplot\n");
	}

	fflush(pe);

	getchar();
}

void Plotshow::show_file(char * filename)
{
	//fprintf(pe, "plot '%s' using 2 \"\%%lf 1:\%%lf \" with lines pt 2 \n" , filename);

	fprintf(pe, "plot '%s' using 0:1:2:3:4 notitle with financebars lt 8  , \
				'%s' using 0:5 notitle with lines lt 3, \
				'%s' using 0:6  notitle with lines lt 1, \
				'%s' using 0:7 notitle with lines lt 2, \n\
				" , filename, filename, filename, filename);
	fflush(pe);
	getchar();
}


