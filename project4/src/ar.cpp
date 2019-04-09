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
#include <iostream>
#include <fstream>
#include "database.h"


std::vector<cv::Vec3f> createPointSet(int rows, int cols){
	std::vector<cv::Vec3f> result;
	for(int i =0; i < cols; i++){
		for(int j = 0; j < rows; j++){
			cv::Vec3f p(i, cols - j - 1, 0);
			result.push_back(p);
		}
	}
	return result;
}

int main(int argc, char *argv[]) {
	cv::VideoCapture *capdev;
	char label[256];
	int quit = 0;
	int frameid = 0;
	char buffer[256];
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

	std::vector<cv::Vec3f> point_set = createPointSet(9,6);
	std::vector<std::vector<cv::Vec3f>> point_list;

    std::vector<double> temp_camera_matrix;
    std::vector<double> temp_coeffs;

    //read in params 
    // params are hardcoded in
    std::ifstream file;
    file.open("output.csv");
    std::string line;

    getline(file, line);
	std::string word;
    getline(file, line);
    std::stringstream sstream(line);

    while(getline(sstream, word, ',')){
        float coeff = std::stof(word);
        temp_coeffs.push_back(coeff);
    }   
	double* a = &temp_coeffs[0];
    cv::Mat distCoeffs = cv::Mat(5, 1, CV_64FC1, a );



     // get camera matrix
    getline(file, line);
	getline(file, line);

    sstream = std::stringstream(line);
    while(getline(sstream, word, ',')){
        float element = std::stof(word);
        temp_camera_matrix.push_back(element);
    }
	double* b = &temp_camera_matrix[0];
    cv::Mat camera_matrix = cv::Mat(3, 3,CV_64FC1, b);

	for (auto it = camera_matrix.begin<double>(); it != camera_matrix.end<double>(); ++it){
		double val = (*it);
		std::cout << val << std::endl;
	}

    while(file.good()){
        getline(file, line);
        std::string word;
        // get the first element of the line and store it as the name
        getline(sstream, word, ',');
        
        // add the rest of the features, assuming we only have one list for now
        while(getline(sstream, word, ',')){
            float feature = std::stof(word);
        }
    }
    file.close();
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

		if(patternfound){
		    cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
			cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

            cv::Mat rvecs, tvecs;

            bool solvable = cv::solvePnP(point_set, corners, camera_matrix,distCoeffs, rvecs, tvecs );

			// for (auto it = rvecs.begin<double>(); it != rvecs.end<double>(); ++it){
			// 	double val = (*it);
            //     std::cout << val << std::endl;
            // }
            //   std::cout << "tvecs" << std::endl;

			// for (auto it = tvecs.begin<double>(); it != tvecs.end<double>(); ++it){
			// 	double val = (*it);
            //     std::cout << val << std::endl;
            // }
            // if (solvable){
            //     std:
            // }
			if (solvable == 1){
			// for (int i = 0; i < point_set.size(); i++){
			// 		std::cout << point_set[i] << std::endl;
			// }
			std::vector<cv::Point3f> ps;
			ps.push_back(cv::Point3f(0,-3,0));
			ps.push_back(cv::Point3f(0,6,0));
			ps.push_back(cv::Point3f(6,-3,0));
			ps.push_back(cv::Point3f(6,6,0));
			ps.push_back(cv::Point3f(0,-3,-3));
			ps.push_back(cv::Point3f(0,6,-3));
			ps.push_back(cv::Point3f(6,-3,-3));
			ps.push_back(cv::Point3f(6,6,-3));
				//project points
			std::vector<cv::Point2f> projections;
			cv::projectPoints(ps, rvecs, tvecs, camera_matrix, distCoeffs, projections );
			// //for (int i = 0; i < 8; i++){
			// 	cv::circle(src, projections[7], 3, cv::Scalar(0, 0, 255));
			// //}
			

		//	for (int i = 0; i < 8; i++){
				cv::line(src, projections[7],  projections[6],  cv::Scalar(0, 0, 255), 4);
				cv::line(src, projections[7],  projections[5],  cv::Scalar(0, 0, 255), 4);
				cv::line(src, projections[6],  projections[4],  cv::Scalar(0, 0, 255), 4);
				cv::line(src, projections[5],  projections[4],   cv::Scalar(0, 0, 255), 4);
				cv::line(src, projections[7],  projections[3],   cv::Scalar(0, 0, 255), 4);
				cv::line(src, projections[6],  projections[2],   cv::Scalar(0, 0, 255), 4);
				cv::line(src, projections[5],  projections[1],   cv::Scalar(0, 0, 255), 4);
				cv::line(src, projections[4],  projections[0],   cv::Scalar(0, 0, 255), 4);
				// for (int i = 0; i < projections.size(); i++){
				// 	std::cout << projections[i] << std::endl;
				// }
			}

        }



		cv::imshow("Video", src);
		//std::cout << "showed frame" << std::endl;
		int key = cv::waitKey(10);

		//std::cout << "pressed key" << (char) (key) << std::endl;
		switch(key) {
		case 's':
		
		case 'c': {
			

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
