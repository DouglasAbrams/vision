#ifndef DATABASE_H
#define DATABASE_H


#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"
#include <string>
#include "OR_pipeline.h"

#define NUM_FEATURES 3


struct Entry{
    Region_features features;
    std::string name;
    // this is written out as name, features
};

struct Database{
    std::vector<Entry> entries;
    std::vector<float> feature_sds;
    std::vector<float> feature_mins;
    std::vector<float> feature_maxs;
};
std::string classify_scaledEuclideanDistance( Database* d, Region_features newObjetFeatures);

std::string classifyKNN(Database * d, Region_features newObjectFeatures, int k);

void computeFeatureAverages(Database* d);

void database_read(char* filename, Database* d);

void database_write(char* filename, Database* d);

void database_addEntry(Entry e, Database* d);

//database_matchSSD()

//database_matchKNN
bool compareLow(std::pair<std::string, int> a, std::pair<std::string, int> b) ;

bool compareHigh(std::pair<std::string, int> a, std::pair<std::string, int> b) ;


int pair_contains(std::string s, std::vector<std::pair<std::string, int>> frequencies);


#endif