
/*
	Bruce A. Maxwell
	S19
	Simple example of video capture and manipulation
	Based on OpenCV tutorials

	Compile command (macos)

	clang++ -o vid -I /opt/local/include vidDisplay.cpp -L /opt/local/lib -lopencv_core -lopencv_highgui -lopencv_video -lopencv_videoio

	use the makefiles provided

	make vid

*/
#include <cstdio>
#include <dirent.h>
#include <opencv2/opencv.hpp>
#include "metrics.h"
#include "OR_pipeline.h"
#include "database.h"
#include <iostream>
#include <climits>
#include <fstream>
#include <string>

int main(int argc, char *argv[]) {

	cv::Mat src;
	char dirname[256];
	char csv[256];
	// usage
	if(argc < 3) {
		printf("Usage %s <image filename>\n", argv[0]);
		exit(-1);
	}
	strcpy(dirname, argv[1]);

   	strcpy(csv, argv[2]);

	//import features database
	Database* d;
	d = database_read(csv);
	
    //calculate sds
    computeFeatureAverages(d);
    // directory shit
	DIR *dirp;
	struct dirent *dp;
	
	dirp = opendir(dirname);
	if( dirp == NULL){
		std::cout << "directory does not exist" << std::endl;
		return(-1);
	}
	    cv::Mat frame;

			std::ofstream file;
			file.open("confusion.csv");
	while( (dp = readdir(dirp)) != NULL){ // read the next image
		//std::cout <<"in loop" << std::endl;
		if( strstr(dp->d_name, ".jpg") ||
			strstr(dp->d_name, ".png") ||
			strstr(dp->d_name, ".ppm") ||
			strstr(dp->d_name, ".tif") ) {

			char filename[100];
			sprintf(filename, "%s/%s", dirname, dp->d_name);
			cv::Mat src = cv::imread(filename);

			std::vector<Region_features> features = OR_pipeline(src, 110, true, frame);


			std::string classified_name = classify_scaledEuclideanDistance(d, features[1]); 
  
			std::string truename = std::string(filename);

			std::size_t last_slash =truename.find_last_of('/');
			truename = truename.substr(last_slash + 1, truename.length());

			std::size_t period =truename.find('.');
			std::string actual_name = truename.substr(0, period);


			std::cout <<" actual" << actual_name << "categorized" << classified_name << std::endl;

			file << actual_name << " " << classified_name << "\n";

		}
	}
				

	return(0);
}
