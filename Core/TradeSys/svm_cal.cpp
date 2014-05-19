
#include "svm.h"
#include "svm_cal.h"
#include <assert.h>

int max_line_len = 1024;

void readline(FILE *input, char *line)
{
	int len;

	if(fgets(line,max_line_len,input) == NULL)
		return ;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line, max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return ;
}


svm_cal_class::svm_cal_class()
{
	x = (struct svm_node *) malloc(64*sizeof(struct svm_node));
}

svm_cal_class::~svm_cal_class()
{
	free(x);
}

void svm_cal_class::load_mode(char * model_name)
{
	if((model=svm_load_model(model_name))==0)
	{
		//fprintf(stderr,"can't open model file %s\n",);
		model = NULL;
	}
	return;
}

void svm_cal_class::set_scale(char * filename)
{
	FILE * fp_restore = fopen(filename,"r");
	
	int idx, c;
	double fmin, fmax;
	int next_index = 1;

	if((c = fgetc(fp_restore)) == 'y')
	{
		assert(0);
#if 0
		fscanf(fp_restore, "%lf %lf\n", &y_lower, &y_upper);
		fscanf(fp_restore, "%lf %lf\n", &y_min, &y_max);
		y_scaling = 1;
#endif
	}
	else
		ungetc(c, fp_restore);

	feature_range.clear();
	if (fgetc(fp_restore) == 'x') 
	{
		fscanf(fp_restore, "%lf %lf\n", &lower, &upper);
		while(fscanf(fp_restore,"%d %lf %lf\n",&idx,&fmin,&fmax)==3)
		{
			//int i;
#if 0
			for(i = next_index;i<idx;i++)
				if(feature_min[i] != feature_max[i])
					fprintf(stderr,
					"WARNING: feature index %d appeared in file %s was not seen in the scaling factor file %s.\n",
					i, argv[argc-1], restore_filename);
#endif
			struct feature_range_type f;

			f.max = fmax;
			f.min = fmin;
			f.idx = idx;
			
			feature_range.push_back(f);

			next_index = idx + 1;
		}

#if 0
		for(i=next_index;i<=max_index;i++)
			if(feature_min[i] != feature_max[i])
				fprintf(stderr,
				"WARNING: feature index %d appeared in file %s was not seen in the scaling factor file %s.\n",
				i, argv[argc-1], restore_filename);
#endif
	}
	fclose(fp_restore);


	return;
}


double svm_cal_class::output(int index, double value)
{
	/* skip single-valued attribute */
	if(feature_range[index].min == feature_range[index].max)
		return 0;

	if(value <= feature_range[index].min)
		value = lower;
	else if(value >= feature_range[index].max)
		value = upper;
	else
		value = lower + (upper-lower) * 
		(value-feature_range[index].min)/
		(feature_range[index].max-feature_range[index].min);

	return value;
}

double svm_cal_class::predict(std::vector <double> value)
{
	int svm_type=svm_get_svm_type(model);
	int nr_class=svm_get_nr_class(model);
	double target_label, predict_label;
	double *prob_estimates=NULL;

	prob_estimates = (double *) malloc(nr_class*sizeof(double));

	int i;
	for( i = 0 ; i < value.size(); i++)
	{
		x[i].index = i+1;
		x[i].value = output(i, value[i]);
	}
	x[i].index = -1;

	if (0&(svm_type==C_SVC || svm_type==NU_SVC))
	{
		predict_label = svm_predict_probability(model,x,prob_estimates);
		//fprintf(output,"%g",predict_label);
		//for(int j=0;j<nr_class;j++)
		//	fprintf(output," %g",prob_estimates[j]);
		//fprintf(output,"\n");
	}
	else
	{
		predict_label = svm_predict(model,x);
		//fprintf(output,"%g\n",predict_label);
	}

	free(prob_estimates);
	return predict_label;
}



