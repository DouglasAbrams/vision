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

cv::Mat stored_descriptors;
std::vector<cv::KeyPoint> stored_keypoints;

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

	cv::namedWindow("Video", 1); 
    cv::namedWindow("Base image", 1);
    cv::namedWindow("Aligned", 1); 
    cv::namedWindow("final image", 1);
    cv::Mat frame;




	for(;!quit;) {

		 *capdev >> frame; // get a new frame from the camera, treat as a stream

		if( frame.empty() ) {
		  printf("frame is empty\n");
		  break;
		}
		cv::Mat gray;
		cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);

        //taken from openCV docs
       
        std::vector<cv::KeyPoint> keypoints;
        cv::Ptr<cv::ORB> detector = cv::ORB::create();
        cv::Mat descriptors;
        detector->detect(frame, keypoints);
        detector->compute(frame, keypoints, descriptors);
        //if we want to match from here we would use something to match between two images, then drop bad matches
        for(auto it = keypoints.begin(); it!=keypoints.end(); it++){
            cv::circle(frame, (*it).pt, 5, cv::Scalar(0));
        }
/*
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
        */
        cv::imshow("Video", frame);
		int key = cv::waitKey(10);

	    switch(key) {
            case 's':
                std::cout << "saving image" << std::endl;
                stored_descriptors = descriptors;
                stored_keypoints = keypoints;
                cv::imshow("Base image", frame);
                break;
            case 'm':{
                std::cout << "aligning image" << std::endl;
                //modified from learnopencv.com/image-alignment-feature-based-using-opencv-c-python/
                std::vector<cv::DMatch> matches;
                cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
                matcher->match(stored_descriptors, descriptors, matches, cv::Mat());
                std::sort(matches.begin(), matches.end());
                std::vector<cv::DMatch> good_matches;
                for(int i = 0; i < matches.size() * .1; i++){
                    good_matches.push_back(matches[i]);
                }

                std::vector<cv::Point2f> base_points, new_points;
                for(int i = 0; i < good_matches.size(); i++){
                    base_points.push_back(stored_keypoints[matches[i].queryIdx].pt);
                    new_points.push_back(keypoints[matches[i].trainIdx].pt);
                }

                cv::Mat h = cv::findHomography(new_points, base_points, cv::RANSAC);
                cv::Mat fixed;
                cv::warpPerspective(frame, fixed, h, frame.size());
                cv::imshow("final image", frame);
                cv::imshow("Aligned", fixed);
            }
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
