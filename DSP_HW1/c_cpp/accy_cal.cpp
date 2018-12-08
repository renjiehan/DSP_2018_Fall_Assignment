#include "hmm.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    FILE *result_file = open_or_die(argv[1], "r");
    FILE *answer_file = open_or_die(argv[2], "r");

    char result[MAX_LINE];
    char answer[MAX_LINE];
    int total = 0;
    int bingo = 0;
    // int ten = 0;

    for (int i = 0; fscanf(result_file, "%s", result) > 0 & fscanf(answer_file, "%s", answer) > 0; i++)
    {
        // printf("%s ", result);
        // printf("%s ", answer);
        if (strcmp(result, answer) == 0)
        {
            bingo++;
            // printf("bingo\n");
        }
        else
        {
            // printf("\n");
        }
        total++;
    }
    double ratio = (double)bingo / (double)total;
    printf("%d / %d = %f\n", bingo, total, ratio);
}
