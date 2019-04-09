/*ssssss

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

std::vector<std::vector<cv::Vec2f>> cornersLists_forCalibration;
std::vector<cv::Mat> frameList_forCalibration;

enum Video_state{
	WAITING,
	CLASSIFYING,
};

std::vector<cv::Vec3f> createPointSet(int rows, int cols){
	std::vector<cv::Vec3f> result;
	for(int i =0; i < cols; i++){
		for(int j = 0; j < rows; j++){
			std::cout << i << j << std::endl;
			cv::Vec3f p(i, cols - j - 1, 0);
			result.push_back(p);
		}
	}
	return result;
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

	// open the video device
	capdev = new cv::VideoCapture(0);
	if( !capdev->isOpened() ) {
		printf("Unable to open video device\n");
		return(-1);
	}

	cv::Size refS( (int) capdev->get(cv::CAP_PROP_FRAME_WIDTH ),
		       (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));

	cv::namedWindow("Video", 1); // identifies a window?
	cv::Mat frame;
    cv::Mat src;
	Video_state video_state = WAITING;

	std::vector<cv::Vec3f> point_set = createPointSet(9,6);
	std::vector<std::vector<cv::Vec3f>> point_list;

	// std::cout << point_set << std::endl;
	for(;!quit;) {

		 *capdev >> src; // get a new frame from the camera, treat as a stream

		if( src.empty() ) {
		  printf("frame is empty\n");
		  break;
		}
		cv::Mat gray;
		cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);


		cv::Size patternsize(9,6); //interior number of corners
		std::vector<cv::Vec2f> corners; //this will be filled by the detected corners

		//CALIB_CB_FAST_CHECK saves a lot of time on images
		//that do not contain any chessboard corners
		bool patternfound = cv::findChessboardCorners(gray, patternsize, corners,
				cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE
				+ cv::CALIB_CB_FAST_CHECK);

		if(patternfound)
		cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
			cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

		cv::drawChessboardCorners(src, patternsize, cv::Mat(corners), patternfound);
		// std::cout << corners << std::endl;

		cv::imshow("Video", src);
		//std::cout << "showed frame" << std::endl;
		int key = cv::waitKey(10);

		//std::cout << "pressed key" << (char) (key) << std::endl;
		switch(key) {
		case 's':
			if (corners.size() !=  54){
			 	std::cout << " not callibrated " <<std::endl;
			 }
			 else {
				std::cout << " saving " <<std::endl;
				cornersLists_forCalibration.push_back(corners);
				frameList_forCalibration.push_back(gray);
				point_list.push_back(point_set);
			 }
		 	break;
		case 'c': {
			if (cornersLists_forCalibration.size()>= 5){
				//cv::Mat camera_matrix = initCamera_matrix(gray);
				double init_data[3][3] = { {1, 0, double(gray.cols)/2}, {0, 1, double(gray.rows)/2}, {0, 0, 1}};
				cv::Mat camera_matrix =  cv::Mat( 3, 3, CV_64FC1, init_data);
				
				cv::Mat distCoeffs = cv::Mat::zeros(8, 1 , CV_64FC1);
				//std::vector<double> distCoeffs;
				std::vector<cv::Mat> rvecs, tvecs;
					for(int i =0; i < 15; i++){
						std::cout << point_list[0][i]<< ", " <<  cornersLists_forCalibration[0][i] << std::endl;
					}
				// std::cout << "point list is " << point_list[0][1] << ", " <<  cornersLists_forCalibration[0][1] << std::endl;
				// std::cout << "cornersLists is " << cornersLists_forCalibration.size() << ", " << cornersLists_forCalibration[0].size() << std::endl;
				double error = cv::calibrateCamera(point_list, cornersLists_forCalibration,
								 gray.size(), camera_matrix, 
								distCoeffs, rvecs, tvecs );
				std::cout << "error is " << error << std::endl;

				std::cout << "save? y/n" << std::endl;
				std::string input;
				std::cin >> input;
				if(input == "y"){
					std::ofstream file;
					file.open("output.csv");
					
					file << "coeffs, ";
					for (auto it = distCoeffs.begin<double>(); it != distCoeffs.end<double>(); ++it){
						double val = (*it);
						file << val << ',';
					} 
					file << "\ncamera matrix, ";

					for (auto it = camera_matrix.begin<double>(); it != camera_matrix.end<double>(); ++it){
						double val = (*it);
						file << val << ',';
					} 
					
					file << "\nerror, " << error ;
					file.close();
				}

			}
			else {
				std::cout << " need more callibration images " <<std::endl;

			}
	

		 	break;
		}
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
