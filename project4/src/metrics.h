/*
	Jason Gurevitch and Douglas Abrams
	Project 2
	Task 2

*/
#ifndef METRICS_H
#define METRICS_H
 
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"

typedef struct{
	char name[100];
	float metric;
}img_metric;

// struct point {
// 	int x;
// 	int y;

// 	point(int xnew, int ynew){
// 		x = xnew;
// 		y = ynew;
// 	}
// };	

typedef struct{
	int red_hist[255];
	int blue_hist[255];
	int green_hist[255];
	bool is_grayscale;
	int total_pixels;
}color_hist;
 
bool compare(const img_metric &a, const img_metric &b);

int SSD(cv::Mat &comp, cv::Mat &source, int N);

color_hist* make_hist(bool is_grayscale);

color_hist* make_hist_from_image(cv::Mat& src);

color_hist* compare_hists(color_hist* src, color_hist* compare);

int hist_ssd(color_hist* src);

void sobel_gradient(cv::Mat& src, cv::Mat& dest);

void standard_chromaticize(cv::Mat& src);

double HIST(cv::Mat &comp, cv::Mat &source);

float getColorRatio(cv::Vec3b pixel, char  colorOfInteret);

float GREENESS (cv::Mat &comp, cv::Mat &source);

float extensionMetric (cv::Mat &comp, cv::Mat &source);

double multi_layerHIST(cv::Mat &comp, cv::Mat &source, int layers);

// void grayscaleThreshold(cv::Mat &src, cv::Mat &dest, int threshold );

// void OR_pipeline(cv::Mat& src, cv::Mat& frame, int threshold);

// void erode(cv::Mat &src, cv::Mat &dest,  int iterations, int dim, bool useCross);

// void dilate(cv::Mat &src, cv::Mat &dest,  int iterations, int dim, bool useCross);

// int findComponents(cv::Mat &src, cv::Mat &dest, bool visualize);

// std::vector<std::vector<point>> getRegions(cv::Mat &src, int numLabels);

// int getMoment(int p, int q, std::vector<point> region_points);

// point getCentroid(std::vector<point> region_points);


#endif
