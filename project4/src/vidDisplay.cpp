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
#include <unistd.h>
#include <string>

std::vector<std::vector<cv::Point2f>> cornersLists_forCalibration;
std::vector<cv::Mat> frameList_forCalibration;

enum Video_state{
	WAITING,
	CLASSIFYING,
};

std::vector<cv::Point3f> createPointSet(int rows, int cols){
	std::vector<cv::Point3f> result;
	for(int i =0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			cv::Point3f p(i, j, 0);
			result.push_back(p);
		}
	}
	return result;
}

void addLabel(Region_features features, cv::Mat displayed, std::string label){
	point centroid = features.centroid;
	char feature_text[100];
	cv::circle(displayed, cv::Point(centroid.x, centroid.y), 3, cv::Scalar(0, 255, 255));
	cv::putText(displayed, label, cv::Point(centroid.x + 5, centroid.y),  cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 255, 0), 1);
	if(features.features.size() == 3){
		sprintf(feature_text, "%5.1f, %5.1f, %5.1f", features.features[0], features.features[1], features.features[2]);
		cv::putText(displayed, std::string(feature_text), cv::Point(centroid.x, centroid.y + 25), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(160, 255, 0), 1);
	}
	else {
		std::cout << features.features.size();
	}
}




int main(int argc, char *argv[]) {
	cv::VideoCapture *capdev;
	Database d;
	char label[256];
	int quit = 0;
	int frameid = 0;
	char buffer[256];
	char database_name[256];
	std::vector<int> pars;
	std::string name;
	pars.push_back(5);

	// if( argc < 3 ) {
	//     printf("Usage: %s <label> <database>\n", argv[0]);
	//     exit(-1);
	// }

	// open the video device
	capdev = new cv::VideoCapture(0);
	if( !capdev->isOpened() ) {
		printf("Unable to open video device\n");
		return(-1);
	}

	// strcpy(label, argv[1]);
	// strcpy(database_name, argv[2]);
	// database_read(database_name, &d);

	cv::Size refS( (int) capdev->get(cv::CAP_PROP_FRAME_WIDTH ),
		       (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));

	cv::namedWindow("Video", 1); // identifies a window?
	cv::Mat frame;
    cv::Mat src;
	Video_state video_state = WAITING;
	std::vector<cv::Point3f> point_set = createPointSet(9,6);
	std::cout << point_set << std::endl;
	for(;!quit;) {


		/** from the OPEN CV docs**/




		 *capdev >> src; // get a new frame from the camera, treat as a stream

		if( src.empty() ) {
		  printf("frame is empty\n");
		  break;
		}
		cv::Mat gray;
		cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);


		cv::Size patternsize(9,6); //interior number of corners
		std::vector<cv::Point2f> corners; //this will be filled by the detected corners

		//CALIB_CB_FAST_CHECK saves a lot of time on images
		//that do not contain any chessboard corners
		bool patternfound = cv::findChessboardCorners(gray, patternsize, corners,
				cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE
				+ cv::CALIB_CB_FAST_CHECK);

		if(patternfound)
		cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
			cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));




		cv::drawChessboardCorners(src, patternsize, cv::Mat(corners), patternfound);
		std::cout << corners << std::endl;



		cv::imshow("Video", src);
		//std::cout << "showed frame" << std::endl;
		int key = cv::waitKey(10);


		//std::cout << "pressed key" << (char) (key) << std::endl;
		switch(key) {
		case 's':
			cornersLists_forCalibration.push_back(corners);
			//frameList_forCalibration(gray);
		 	break;
		case 'q':
		    quit = 1;
		    break;
		default:
		    break;
		}

	}

	// terminate the video capture
	printf("Terminating\n");
	delete capdev;

	return(0);
}
