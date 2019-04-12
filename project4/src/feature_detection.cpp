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

int threshold =  200;
int block_size = 2;


int main(int argc, char *argv[]) {
	cv::VideoCapture *capdev;
	char label[256];
	int quit = 0;

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

    cv::createTrackbar("threshold:", "Video", &threshold, 255, NULL);
    cv::createTrackbar("block_size:", "Video", &block_size, 50, NULL);



	for(;!quit;) {

		 *capdev >> frame; // get a new frame from the camera, treat as a stream

		if( frame.empty() ) {
		  printf("frame is empty\n");
		  break;
		}
		cv::Mat gray;
		cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);

        //taken from openCV docs
        int sobel_size = 3;
        double k = 0.04;
        
        cv::Mat harris_corners = cv::Mat(gray.size(), CV_32FC1);
        cv::cornerHarris(gray, harris_corners, block_size+1, sobel_size, k);
        cv::Mat normalized_harris_corners;
        cv::normalize (harris_corners, normalized_harris_corners, 0,255, cv::NormTypes::NORM_MINMAX, CV_32FC1, cv::Mat());
        cv::Mat scaled_normalized_harris_corners;
        cv::convertScaleAbs(normalized_harris_corners, scaled_normalized_harris_corners);

        for(int i = 0; i < normalized_harris_corners.rows; i++){
            for(int j = 0; j < normalized_harris_corners.cols; j++){
                //std::cout << i << " , " << j << std::endl;
                int val = normalized_harris_corners.at<float>(i,j);
                if(val > threshold){
                    double scaled = ((val-threshold)/(255.0-threshold)) * 10;
                    //std::cout << val << std::endl;
                   
                    cv::circle(scaled_normalized_harris_corners, cv::Point(j,i), scaled, cv::Scalar(0));
                }
            }
        }

		cv::imshow("Video", scaled_normalized_harris_corners);
		int key = cv::waitKey(10);

	    switch(key) {
            case 'h':

                break;
            case 'o': 
                break;
            case 'q':
                quit = 1;
                break;
            default:
                break;
		}

	}

	printf("Terminating\n");
	delete capdev;

	return(0);
}
