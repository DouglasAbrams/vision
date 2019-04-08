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
#include <opencv2/opencv.hpp>
#include "metrics.h"
#include "OR_pipeline.h"
#include "database.h"


int main(int argc, char *argv[]) {

	cv::Mat src;
	char filename[256];
	char csv[256];
	// usage
	if(argc < 3) {
		printf("Usage %s <image filename>\n", argv[0]);
		exit(-1);
	}
	strcpy(filename, argv[1]);

	// read the image
	src = cv::imread(filename);

	// test if the read was successful
	if(src.data == NULL) {
		printf("Unable to read image %s\n", filename);
		exit(-1);
	}

	//read CSV
	strcpy(csv, argv[2]);

    cv::Mat frame;
    
	//import features database
	Database d;
	database_read(csv, &d);
	
   std::vector<Region_features> features = OR_pipeline(src, 110, true, frame);
	
	
// create a window
	cv::namedWindow(filename, 1);

	// edit the source image here

	// show the image in a window
	cv::imshow(filename, frame);

	// wait for a key press (indefinitely)
	 while(true){ 

	 	char k = (char)cv::waitKey(0);
	 	if (k == 'n'){
			char name[256];
			std::cout << "enter a label" << std::endl;
			std::cin >> name;
			std::cout << "adding entry with name " << name << std::endl;
			Entry e;
			e.name = std::string(name);
			e.features = features[0];
			database_addEntry(e, &d);
			database_write(csv, &d);
	 		break;
	 	}
		 if (k == 'c'){
			std::cout << "computing feature averages" << std::endl;
			computeFeatureAverages(&d);
			//std::cout <<"\n\n\n\n AVGs: " << d.feature_sds[0] << " " << d.feature_sds[1] << std::endl;
			std::cout << "classifying" << std::endl;
			std::string name = classify_scaledEuclideanDistance(&d, features[0]);
			//std::string name = classifyKNN(&d, features[0], 3);

			std::cout << "maching obj : "<< name << std::endl;
		 }
		 if(k == 'x' || k == 'q'){
			 break;
		 }
}

	// get rid of the window
	cv::destroyWindow(filename);
	


	return(0);
}
