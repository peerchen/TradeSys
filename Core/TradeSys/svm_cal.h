


#include "svm.h"
#include <vector>

struct feature_range_type
{
	int idx;
	double max;
	double min;
};

class svm_cal_class
{
public:
	svm_cal_class();
	~svm_cal_class();
	void load_mode(char * model_name);

	void set_scale(char * filename);

	double predict(std::vector<double> node);
	
	double output(int index, double value);
private:
	struct svm_model* model;
	struct svm_node * x;
	std::vector<struct feature_range_type> feature_range;
	double lower,upper;
};