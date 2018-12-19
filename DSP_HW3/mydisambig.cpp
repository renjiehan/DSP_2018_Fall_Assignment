#include <stdio.h>
#include <iostream>
#include <map>
#include <set>
//#include <vector>

#include "utils.h"

#define BUFF_SIZE  4096
using namespace std;

void Print(vector<string> line){
    cout << "<s>";
    for(int i = 0 ; i < line.size() ; i++){
       cout << " " << line[i];
    }
    cout << " </s>" << endl;
}

int main(int argc, char** argv) {
    FILE *fpMap = fopen("Big5-ZhuYin.map", "r");
    map<string, set<string> > mapping;
    Vocab vocab;
    Ngram lm(vocab, 2);

    File lmFile("bigram.lm", "r");
    lm.read(lmFile);
    lmFile.close();

    readMappingFile(fpMap, mapping);

    char* filename = argv[1];
    FILE *fp = fopen(filename, "r");

    int count =0, k;
    char tmpstr[BUFF_SIZE];
    while( (k = fscanf(fp, "%[^\n]%*c", tmpstr)) != EOF) {
        vector<string> splitInput =processInput(tmpstr);
        vector<set<string> > candidate = findMapping(splitInput, mapping);
        vector<string> result = Viterbi(candidate, lm, vocab) ;
        Print(result);
    }
    return 0;
}
