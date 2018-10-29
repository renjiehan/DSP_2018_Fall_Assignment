#include "hmm.h"
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef _SAMPLE
#define _SAMPLE 10000
#endif
#ifndef _MODEL_NUM_
#define _MODEL_NUM_ 5
#endif

double _delta[2][MAX_STATE];
double _path[_MODEL_NUM_][MAX_SEQ][MAX_STATE];
double _terminate[_MODEL_NUM_];

void viterbi(HMM *hmm_model, char *sample, int model_num, int *best_model_index, double *max_path_prob)
{
    int len = strlen(sample);
    int data = sample[0] - 'A';
    double max_prob =0;
    int best_model =-1;

    // Observation 0
    for(int model_idx=0; model_idx<model_num; model_idx++) {
        HMM *hmm = hmm_model + model_idx;
        data = sample[0] - 'A';

        for(int j=0; j<hmm->state_num; j++) {
            _delta[0][j] = hmm->initial[j] * hmm->observation[data][j];
        }

        // Observation 1 to the last
        for(int obs_idx=1; obs_idx<len; obs_idx++) {
            data = sample[obs_idx] - 'A';
            double delta =0.0;

            for(int j=0; j< hmm->state_num; j++) { // go through observation
                int obs_preidx = obs_idx -1;
                double tmp_high_prob =0.0;

                for(int pre_state=0; pre_state < hmm->state_num; pre_state++){ // transit from previous state
                    delta = _delta[0][pre_state] * hmm->transition[pre_state][j];

                    if( delta > tmp_high_prob) {
                        tmp_high_prob = delta;
                    }
                }

                _delta[1][j] = tmp_high_prob * hmm->observation[data][j];
            }

            for(int j=0; j< hmm->state_num; j++) {
                _delta[0][j] = _delta[1][j];
            }
        }

        for(int j=0; j<hmm->state_num; j++) {
            if( _delta[1][j] > max_prob ) {
                max_prob = _delta[1][j];
                best_model = model_idx;
            }
        }

        *max_path_prob = max_prob;
        *best_model_index = best_model;
    }
}


int main(int argc, char *argv[])
{
    HMM models[_MODEL_NUM_];
    int model_num = load_models( "modellist.txt", models, _MODEL_NUM_);

    FILE *input_data = open_or_die(argv[2], "r");
    FILE *output_result = open_or_die(argv[3], "w");
    FILE *handin = open_or_die(argv[4], "w");

    char data[MAX_LINE];
    int limit = 0;

    while (fscanf(input_data, "%s", data))
    {
        int best_model_index =0;
        double max_path_prob =0.0;
        viterbi(models, data, model_num, &best_model_index, &max_path_prob);
        // printf("%s %f\n", HMM_models[best_model_index].model_name, max_path_prob);
        limit++;
        if (limit > 2500)
        {
            break;
        }

        fprintf(handin, "%s %e\n", models[best_model_index].model_name, max_path_prob);
        fprintf(output_result, "%s\n", models[best_model_index].model_name);
    }

    fclose(input_data);
    fclose(output_result);
}
