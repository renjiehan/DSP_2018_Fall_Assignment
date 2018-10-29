//#include <iostream>

/*
#	define MAX_STATE	10
#	define MAX_OBSERV	26
#	define	MAX_SEQ		200
#	define MAX_LINE 	256

typedef struct{
   char *model_name;
   int state_num;					//number of state
   int observ_num;					//number of observation
   double initial[MAX_STATE];			//initial prob.
   double transition[MAX_STATE][MAX_STATE];	//transition prob.
   double observation[MAX_OBSERV][MAX_STATE];	//observation prob.
} HMM;
*/

#include "hmm.h"
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef _SAMPLE
#define _SAMPLE 10000
#endif

double _alpha[_SAMPLE][MAX_SEQ][MAX_STATE];
double _beta[_SAMPLE][MAX_SEQ][MAX_STATE];
double _gamma[_SAMPLE][MAX_SEQ][MAX_STATE];
double accu_gamma[MAX_STATE];
double accu_obs_gamma[MAX_STATE][MAX_STATE];
double epsilon[_SAMPLE][MAX_SEQ][MAX_STATE][MAX_STATE];
double accu_epsilon[MAX_STATE][MAX_STATE];
double updated_initial[MAX_STATE];

void alpha_cal(HMM *hmm, char* sample, int sample_num)
{
    int len = strlen(sample);
    int data = sample[0] - 'A';
    // Observation 0
    for(int j=0; j<hmm->state_num; j++){
        _alpha[sample_num][0][j] = hmm->initial[j] * hmm->observation[data][j];
    }

    // Observation 1 to the last
    for(int obs_idx=1; obs_idx<len; obs_idx++) {
        data = sample[obs_idx] - 'A';

        for(int j=0; j< hmm->state_num; j++) { // go through observation
            int obs_preidx = obs_idx -1;
            double tmp_sum =0;

            for(int pre_state=0; pre_state < hmm->state_num; pre_state++){ // transit from previous state
                tmp_sum += _alpha[sample_num][obs_preidx][pre_state]
                            * hmm->transition[pre_state][j];
            }

            _alpha[sample_num][obs_idx][j] = tmp_sum * hmm->observation[data][j];
        }
    }
}

void beta_cal(HMM *hmm, char* sample, int sample_num)
{
    int len = strlen(sample);
    int data = sample[0] - 'A';
    // Observation T (last)
    for(int j=0; j<hmm->state_num; j++){
        _beta[sample_num][len-1][j] = 1;
    }

    // Observation second last to 1 (beginning)
    for(int obs_idx=len-2; obs_idx>-1; obs_idx--) {
        data = sample[obs_idx] - 'A';

        for(int i=0; i< hmm->state_num; i++) { // go through observation
            int obs_postidx = obs_idx +1;

            for(int post_state=0; post_state<hmm->state_num; post_state++) {
                _beta[sample_num][obs_idx][i] += hmm->transition[i][post_state]
                                            * hmm->observation[data][post_state]
                                            * _beta[sample_num][obs_postidx][post_state];
            }
        }
    }
}

void gamma_cal(HMM *hmm, char* sample, int sample_num)
{
    int obs_num = strlen(sample);

    for(int obs_idx=0; obs_idx <obs_num; obs_idx++) {
        double norm_numerator = 0;

        for(int st_num=0; st_num < hmm->state_num; st_num++) {
            _gamma[sample_num][obs_idx][st_num] =
                _alpha[sample_num][obs_idx][st_num] *
                _beta[sample_num][obs_idx][st_num];

            norm_numerator += _gamma[sample_num][obs_idx][st_num] ;
        }

        for(int st_num=0; st_num < hmm->state_num; st_num++) {
            _gamma[sample_num][obs_idx][st_num] /= norm_numerator;
        }
    }

    for(int st_num=0; st_num < hmm->state_num; st_num++) {
        updated_initial[st_num] += _gamma[sample_num][0][st_num];

        for(int obs_idx=0; obs_idx <obs_num-1; obs_idx++) {
            int data = sample[obs_idx] - 'A';

            accu_gamma[st_num] += _gamma[sample_num][obs_idx][st_num];
            accu_obs_gamma[data][st_num] += _gamma[sample_num][obs_idx][st_num];
        }
    }
}

void epsilon_cal(HMM *hmm, char* sample, int sample_num)
{
    int obs_num = strlen(sample);

    for(int obs_idx=0; obs_idx<obs_num-1; obs_idx++) {
        int post_obs = sample[obs_idx+1] - 'A';
        double norm_numerator = 0;

        for(int i=0; i<hmm->state_num; i++) {
            for(int j=0; j<hmm->state_num; j++) {
                epsilon[sample_num][obs_idx][i][j] = hmm->transition[i][j] *
                                _alpha[sample_num][obs_idx][i] *
                                _beta[sample_num][obs_idx+1][j] *
                                hmm->observation[post_obs][j];

                norm_numerator += epsilon[sample_num][obs_idx][i][j];
            }
        }

        for(int i=0; i<hmm->state_num; i++) {
            for(int j=0; j<hmm->state_num; j++) {
                epsilon[sample_num][obs_idx][i][j] /= norm_numerator;
            }
        }
    }

    for(int obs_idx=0; obs_idx<obs_num-1; obs_idx++) {
        for(int i=0; i<hmm->state_num; i++) {
            for(int j=0; j<hmm->state_num; j++) {
                accu_epsilon[i][j] +=
                    epsilon[sample_num][obs_idx][i][j];
            }
        }
    }
}

void update_param(HMM* hmm, int state_total)
{
    for(int st=0; st<hmm->state_num; st++){
        hmm->initial[st] = updated_initial[st] / state_total;
    }

    for(int st=0; st<hmm->state_num; st++) {
        for(int post_st=0; post_st<hmm->state_num; post_st++) {
            hmm->transition[st][post_st] = accu_epsilon[st][post_st] / accu_gamma[st];
        }
    }

    for(int observ=0; observ<hmm->state_num; observ++) {
        for(int st=0; st<hmm->state_num; st++) {
            hmm->observation[observ][st] = accu_obs_gamma[observ][st] / accu_gamma[st];
        }
    }

}

void accu_init(HMM *hmm)
{
    for(int i=0; i<hmm->state_num; i++) {
        accu_gamma[i] = 0;
        updated_initial[i] = 0;

        for(int j=0; j<hmm->state_num; j++) {
            accu_epsilon[i][j] = 0;
            accu_obs_gamma[i][j] = 0;
        }
    }
}

void train_one_sample(HMM *hmm, char *sample, int sample_num)
{
    alpha_cal(hmm, sample, sample_num);
    beta_cal(hmm, sample, sample_num);
    gamma_cal(hmm, sample, sample_num);
    epsilon_cal(hmm, sample, sample_num);
};

void train(int iter_time, HMM *hmm, FILE *training_data)
{
    int sample_num;
    char sample[MAX_LINE];

    // int number_of_state = HMM_model->state_num;
    // int number_of_observation = HMM_model->observ_num;

    for (int i = 0; i < iter_time; i++)
    {
        fseek(training_data, 0, SEEK_SET);
        accu_init(hmm);
        for (sample_num = 0; fscanf(training_data, "%s", sample) > 0; sample_num++)
        {
            train_one_sample(hmm, sample, sample_num);
            // printf("end\n");
        }
        update_param(hmm, sample_num);
    }
}

int main(int argc, char* argv[])
{

        if( argc != 5)
            throw;

        // int iteration_time = strtol(argv[1], NULL, 10);
        int iteration_time = atoi(argv[1]);

        // printf("%d\n", iteration_time);
        HMM HMM_model;
        loadHMM(&HMM_model, argv[2]);
        // dumpHMM(stderr, &HMM_model);


            FILE *training_data = open_or_die(argv[3], "r");
            FILE *trained_model = open_or_die(argv[4], "w");


        train(iteration_time, &HMM_model, training_data);
        dumpHMM(trained_model, &HMM_model);
        fclose(trained_model);
        fclose(training_data);

}
