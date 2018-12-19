#include "utils.h"

using namespace std;

void readMappingFile(FILE *fpMap, map<string, set<string> > &mapping) {
    int pos, i;
    char tmpword[50];
    string word, zhuyin, token;

    while( (i = fscanf(fpMap, "%s", tmpword)) != -1) {
        pos = 0;
        word = string(tmpword);

        fscanf(fpMap, "%[^\t\n]s", tmpword);
        zhuyin = string(tmpword).substr(1);

        while(pos != EOF){
            pos = zhuyin.find("/");
            token = zhuyin.substr(0, pos);
            string tmp = token.substr(0, 2);
            mapping[tmp].insert(word);
            zhuyin = zhuyin.substr(pos + 1);
        }
    }
#ifdef _DEBUG
    map<string, set<string> >::iterator map_it;
    set<string>::iterator set_it;

    for( map_it= mapping.begin();
     map_it != mapping.end(); ++map_it) {
        cout << map_it->first << ": ";

        for(set_it= map_it->second.begin();
        set_it != map_it->second.end(); ++set_it ) {
            cout << *set_it << " ";
        }
        cout << endl;
    }
#endif
}

vector<string> processInput(char *cpLine) {
    vector<string> eraseSpace;
    int pos;
    string tempLine = string(cpLine);

    while( (pos =tempLine.find(" ") ) != -1 ) {
        tempLine.erase(pos, 1);
    }

    assert(tempLine.size() %2 ==0);

    string word;
    for(int i=0; i<tempLine.size(); i+=2) {
        word = tempLine.substr(i,2);
        eraseSpace.push_back(word);
    }
#ifdef _DEBUG
    for(int i=0; i<eraseSpace.size(); i++) {
        cout << eraseSpace[i] << " ";
    }
    cout << endl;
#endif
    return eraseSpace;
}

vector<set<string> > findMapping(vector<string> inputLine, map<string, set<string> > mapping) {
    vector<set<string> > candidate;

    for(int i=0; i< inputLine.size(); i++) {
        string input = inputLine[i];
        set<string> finding;

        if( mapping.find(input) != mapping.end() )
            finding = mapping.find(input)->second;
        else
            finding.insert(input);

        candidate.push_back(finding);
    }
#ifdef _DEBUG
    for(int i=0; i<candidate.size(); i++) {
        set<string>::iterator it;
        cout << "word count= " << i << " ";

        for(it= candidate[i].begin(); it != candidate[i].end(); it++) {
            cout << *it << " ";
        }
        cout <<endl;
    }
    cout << endl;
#endif
    return candidate;
}


double getBigramProb(const char *w1, const char *w2, Ngram &lm, Vocab &vocab) {
    VocabIndex wid1 = vocab.getIndex(w1);
    VocabIndex wid2 = vocab.getIndex(w2);

    if(wid1 == Vocab_None)  //OOV
        wid1 = vocab.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  //OOV
        wid2 = vocab.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid1, Vocab_None };
    return lm.wordProb( wid2, context);
}

pair<vector<string>, double> findBestBigram(
    vector< pair<vector<string>, double> > path,
    string currentWord,
    Ngram &lm,
    Vocab &vocab)
{
    pair<vector<string>, double> candidate;
    vector<string>::iterator str_it;
    int index=0;
    double maxProb = -10000.0;

    for(int i=0; i<path.size(); i++) {
        candidate = path[i];
        str_it = candidate.first.end();
        str_it -= 1;
        string word = (*str_it);

#define _DEBUG
#ifdef _DEBUG
cout << "item= " << path.first << "old P= " << path.second << endl;
#endif

        double prob = candidate.second;
        double probBigram = getBigramProb(word.c_str(), currentWord.c_str(), lm, vocab);
        if((prob + probBigram) > maxProb) {
            maxProb = prob + probBigram;
            index = i;
        }
#define _DEBUG
#ifdef _DEBUG
cout << "item= " << path.first << "new P= " << path.second << endl;
#endif

#ifdef _DEBUG
        //cout << word.c_str() << currentWord.c_str() << " ";
        //cout << prob << " " << probBigram << " " << maxProb << endl;
#endif
    }
    assert(index < path.size());

    candidate = path[index];
    candidate.first.push_back(currentWord);
    candidate.second = maxProb;

    return candidate;
}


vector<string> Viterbi(vector<set<string> > nodes, Ngram &lm, Vocab &vocab) {
    vector< pair<vector<string>, double> > path;
    set<string>::iterator node_iter = nodes[0].begin();

    // init
    for(node_iter; node_iter != nodes[0].end(); node_iter++) {
        vector<string> candidate;
        candidate.push_back( *(node_iter));
        path.push_back(make_pair(candidate, 0.0));  // log Prob.
    }

    for(int i=1; i<nodes.size(); i++ ) {
        vector< pair<vector<string>, double> > newPath;
        node_iter = nodes[i].begin();

        for(node_iter; node_iter != nodes[i].end(); node_iter++) {
            string node = (*node_iter);

            // get best Prob. from previous word set
            pair<vector<string>, double> best = findBestBigram(path, node, lm, vocab);
            newPath.push_back(best);
        }
        path.clear();
        path = newPath;
    }

    double maxProb = -10000.0;
    double prob = 0.0;
    int index = 0;

    for(int i=0; i<path.size(); i++ ) {
        if( path[i].second > maxProb) {
            maxProb = path[i].second;
            index = i;
        }
    }

    return path[index].first;
}
