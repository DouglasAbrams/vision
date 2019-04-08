#include "database.h"
#include <iostream>
#include <climits>
#include <fstream>
#include <string>

void database_read(char* filename, Database *d){
    std::ifstream file;
    file.open(filename);
    std::string line;
    while(file.good()){
        Entry e;
        getline(file, line);
        std::string word;
        std::stringstream sstream(line);
        // get the first element of the line and store it as the name
        getline(sstream, word, ',');
        e.name = word;
        // add the rest of the features, assuming we only have one list for now
        while(getline(sstream, word, ',')){
            float feature = std::stof(word);
            e.features.features.push_back(feature);
        }
        d -> entries.push_back(e);
    }
    file.close();
}

void database_write(char* filename, Database* d){
    std::ofstream file;
    file.open(filename);
    for(int i = 0; i < d->entries.size(); i++){
        Entry e = d->entries[i];
        //std::cout << e.name << " , ";
        file << e.name << ",";
        for(int c = 0; c < e.features.features.size(); c++){
            float feature = e.features.features[c];
            file << feature;
            if(c < e.features.features.size() - 1){
                file << ",";
            }
        }
        if(i <  d->entries.size() - 1){
            file << "\n";
        }
    }
    file.close();
}

void database_addEntry(Entry e, Database* d){
    d -> entries.push_back(e); 
}

void computeFeatureAverages(Database* d){
   // std::cout << "in compute feature averages" << std::endl;
    std::vector<float> feature_averages(NUM_FEATURES);
    std::vector<float> feature_sds(NUM_FEATURES);
    std::vector<float> feature_maxes(NUM_FEATURES);
    std::vector<float> feature_mins(NUM_FEATURES);


    for(int i = 0; i < NUM_FEATURES; i++){
        feature_sds[i] = 0;
        feature_averages[i] = 0;
        feature_maxes[i] = INT_MIN;
        feature_mins[i] = INT_MAX;
    } 

    int numEntries = d->entries.size();
    
    for (int i = 0; i < d->entries.size(); i++){
        Entry e = d->entries[i];
        for (int j = 0; j < NUM_FEATURES; j++){
            float value = e.features.features[j];
            feature_averages[j] += value;
            if(value > feature_maxes[j]){
                feature_maxes[j] = value;
            }
            if(value < feature_mins[j]){
                feature_mins[j] = value;
            }

        }
    }
    for (int i = 0; i < NUM_FEATURES; i++){
        feature_averages[i] /= numEntries;
    }
    //std::cout << "calculated average" << std::endl;
    for(int i = 0; i < d-> entries.size(); i++){
        Entry e = d->entries[i];
        for(int j = 0; j < NUM_FEATURES; j++){
            float value = e.features.features[j]-feature_averages[j];
            feature_sds[i] += value*value;
        }
    }
    for(int i = 0; i < NUM_FEATURES; i++){
        feature_sds[i] = sqrt(feature_sds[i]/d->entries.size());
    }
    //std::cout << "calcualted std dev" << std::endl;
    d->feature_sds = feature_sds;
    d->feature_maxs = feature_maxes;
    d->feature_mins = feature_mins;
    //std::cout << "finished compute feature averages" << std::endl;
}

bool compareLow(std::pair<std::string, int> a, std::pair<std::string, int> b){
	return a.second < b.second;
} 

bool compareHigh(std::pair<std::string, int> a, std::pair<std::string, int> b){
	return b.second > a.second;
}

std::string classifyKNN(Database * d, Region_features newObjectFeatures, int k){
    
    if( k > d->entries.size()){
        std::cout << "there are not enough enteries, using scaled euclidean distance instead" << std::endl;
        return classify_scaledEuclideanDistance(d, newObjectFeatures);
    }
    
	std::vector<std::pair<std::string, int>> matches;
	for ( int j = 0; j < d->entries.size(); j++){
        
        //std::cout << d->entries.size()<< j<< std::endl;
		Entry e = d->entries[j];
		Region_features entry_features = e.features;
		std::string name(e.name);
		
		float compare_sum = 0.0;
        
		std::vector<float> compare_features = newObjectFeatures.features;	

		for (int i = 0; i < entry_features.features.size(); i++){
			float diff= (compare_features[i] - entry_features.features[i]- d->feature_mins[i])/  (d->feature_maxs[i]- d->feature_mins[i]);
            //std::cout << diff << ',';
            diff = abs(diff);
			compare_sum += diff;
		}
        //std::cout << std::endl;
        
		matches.push_back(std::pair <std::string, int>(name, compare_sum));
        
		//matches.push_back(std::pair <std::string, int>("hello", 0.0));

	} 
    
	std::sort(matches.begin(), matches.end(), compareLow);
  // std::cout << matches[0].second << " , " << matches[matches.size()-1].second<< std::endl;
    std::vector<std::pair<std::string, int>> names;
    for(int i = 0; i < k; i++){
        int index = pair_contains(matches[i].first, names);
        if(index == -1){
            std::pair<std::string, int> frequency(matches[i].first, 0);
            names.push_back(frequency);
        }else{
            names[index].second ++;
        }
    }

    int max = 0;
    int max_index = 0;
    for(int i = 0; i < names.size(); i++){
        int freq = names[i].second;
        if (freq > max){
            max = freq;
            max_index = i;
        }
    }
    std::string result = names[max_index].first;
    
    //std::sort(names.begin(), names.end(), compareHigh);
    //std::cout << names[0].second << " , " << names[names.size()-1].second<< std::endl;
    //get top k and most common names
    return result;
	
   //return std::string("woo");
}

int pair_contains(std::string s, std::vector<std::pair<std::string, int>> frequencies){
    for(int i = 0; i < frequencies.size(); i++){
        if(frequencies[i].first == s){
            return i;
        }
    }
    return -1;
}

std::string classify_scaledEuclideanDistance( Database* d, Region_features newObjetFeatures){
    float min = INT_MAX;
    std::string min_name;
    std::cout << "here" << std::endl;
    for (auto it = d->entries.begin(); it != d->entries.end(); ++it){
        Entry e = (*it);
        Region_features entry_features = e.features;
        std::string name = e.name;
        

        float compare_sum = 0;

        for (int i = 0; i < NUM_FEATURES; i++){
            //float diff = (newObjetFeatures.features[i] - entry_features.features[i]) /  d->feature_sds[i];
			float diff= (newObjetFeatures.features[i] - entry_features.features[i]- d->feature_mins[i])/  (d->feature_maxs[i]- d->feature_mins[i]);
//           float diff = (newObjetFeatures.features[i] - entry_features.features[i] - d->feature_mins[i]) / (d->feature_maxs[i] - d->feature_mins[i]);
            diff = abs(diff)/NUM_FEATURES;
            compare_sum += diff;
        }
        if (compare_sum < min){
                min = compare_sum;
                min_name = name;
            }
    }
    std::cout << "difference is " << min << std::endl;
    if(min < 0.1){
        return std::string(min_name);
    } else {
        return std::string("could not be matched");
    }
}