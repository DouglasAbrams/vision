/*
	Jason Gurevitch and Douglas Abrams
	Project 2
	Task 1

*/

#ifndef OR_pipeline_H
#define OR_pipeline_H
 
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"


struct point {
	int x;
	int y;


	point(){
		x = 0;
		y = 0;
	}
	point(int xnew, int ynew){
		x = xnew;
		y = ynew;
	}
};

struct Region_features{
	std::vector<float> features;
	point centroid;
	// feature 0 is perfecnt filled
	// feature 1 is bounding box ratio
	// feature 2 is second order moment about central axis
};

struct Region_information{
	std::vector<point> corners;
	point centroid;
	float major_axis;
	float major_extent;
	float minor_extent;
};

void grayscaleThreshold(cv::Mat &src, cv::Mat &dest, int threshold );

std::vector<Region_features> OR_pipeline(cv::Mat& src,  int threshold, bool visualize, cv::Mat& dest);

void erode(cv::Mat &src, cv::Mat &dest,  int iterations, int dim, bool useCross);

void dilate(cv::Mat &src, cv::Mat &dest,  int iterations, int dim, bool useCross);

int labelRegions(cv::Mat &src, cv::Mat &dest, bool visualize);

std::vector<std::vector<point>> getRegionVectors(cv::Mat &src, int numLabels);

int getMoment(int p, int q, std::vector<point> region_points);

point getCentroid(std::vector<point> region_points);

float getMajorAxis(std::vector<point> region_points);

float getExtent(float axis_angle, std::vector<point> region_points, point centroid);

float project(float axis_angle, point p, point centroid);

std::vector<point> getOrientedBoundingBoxCorners(std::vector<point> region_points, point centroid, float major_axis);

float getPercentFilled(std::vector<point> region_points, std::vector<point> corners, float major_extent, float minor_extent);

float getBoundingBoxRatio(std::vector<point> region_points, point centroid, float major_axis);

float getSecondOrderMomentAboutCentralAxis(std::vector<point> region_points, float major_axis, point centroid);

void drawRegionInformation(cv::Mat& src, std::vector<Region_information>  region_information);


#endif