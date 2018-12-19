#include <stdio.h>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "Ngram.h"
using namespace std;

void readMappingFile(FILE *fpMap, map<string, set<string> > &mapping);
vector<string> processInput(char *cpLine);
vector<set<string> > findMapping(vector<string> inputLine, map<string, set<string> > mapping);

double getBigramProb(const char *w1, const char *w2);
pair<vector<string>, double> FindBestBigram(
    vector< pair<vector<string>, double> > path,
    string currentWord,
    Ngram &lm,
    Vocab &vocab);

vector<string> Viterbi(vector<set<string> > AllNodes, Ngram &lm, Vocab &vocab);
