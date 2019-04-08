
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
	Database d;
	std::cout << "reading into database" <<std::endl;
	database_read(csv, &d);
	std::cout <<"read database" <<std::endl;

    // directory shit
	DIR *dirp;
	struct dirent *dp;
	
	dirp = opendir(dirname);
	if( dirp == NULL){
		std::cout << "directory does not exist" << std::endl;
		return(-1);
	}
	std::cout << "starting loop" << std::endl;

	while( (dp = readdir(dirp)) != NULL){ // read the next image
		//std::cout <<"in loop" << std::endl;
		if( strstr(dp->d_name, ".jpg") ||
				strstr(dp->d_name, ".png") ||
				strstr(dp->d_name, ".ppm") ||
				strstr(dp->d_name, ".tif") ) {

                char filename[100];
				sprintf(filename, "%s/%s", dirname, dp->d_name);
                cv::Mat src = cv::imread(filename);

                std::vector<Region_features> features = OR_pipeline(src, 110, true, src);
                std::string name = std::string(dp->d_name);
                std::size_t found = name.find('.');

                Entry e;
                e.name = name.substr(0, found);
                e.features = features[0];
                database_addEntry(e, &d);
		}
	}

    database_write(csv, &d);

	return(0);
}
