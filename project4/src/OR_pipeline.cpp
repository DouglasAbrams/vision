/*
	Jason Gurevitch and Douglas Abrams
	Project 3
	Task 3

*/
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <cstring>
#include "opencv2/opencv.hpp"
#include "OR_pipeline.h"
#include <math.h>



std::vector<Region_features> OR_pipeline(cv::Mat& src,  int threshold, bool visualize, cv::Mat& dest){
	//std::cout << "in OR pipeline" << std::endl;
	cv::Mat grayscaled;
    grayscaleThreshold(src, grayscaled, threshold );
	//std::cout << "grayscaled" << std::endl;
	cv::Mat tmp = grayscaled.clone();
	for (int j = 1; j < 5; j++){
		erode(tmp, tmp, j , 3, true);
		dilate(tmp, tmp, j , 3, true);
	}
	//std::cout << " did morphological processing" << std::endl;
	//output = tmp.clone();
	cv::Mat region_id_map;
	int numRegions = labelRegions(tmp, region_id_map, false);
	cv::Mat pretty_output;
	labelRegions(tmp, pretty_output, true);
	//std::cout << "labeled regions" << std::endl;
	std::vector<std::vector<point>> region_vectors = getRegionVectors(region_id_map, numRegions);
	region_vectors = std::vector<std::vector<point>>(region_vectors.begin()+1, region_vectors.end());
	numRegions--;
	//std::cout << "got region vectors" << std::endl;
	std::vector<Region_information> region_info;
	for(int i = 0; i < numRegions; i++){
		Region_information info;
		std::vector<point> region = region_vectors[i];
		info.centroid = getCentroid(region);
		info.major_axis = getMajorAxis(region);
		info.major_extent = getExtent(info.major_axis, region, info.centroid);
		info.minor_extent = getExtent( (info.major_axis + M_PI/2), region, info.centroid);		
		info.corners = getOrientedBoundingBoxCorners(region, info.centroid, info.major_axis);
		region_info.push_back(info);
	}
	//std::cout << "got region information" << std::endl;
	float img_size = src.rows * src.cols;
	std::vector<Region_features> region_features;
	std::vector<Region_information> proper_info;
	for(int i = 0; i < numRegions; i++){
		Region_features features;
		Region_information info = region_info[i];
		std::vector<point> region = region_vectors[i];
		
		float area = (info.major_extent * 2) * (info.minor_extent * 2);
		if(area/img_size < 0.001){
			continue;
		}
		proper_info.push_back(info);


		float percentFilled = getPercentFilled(region, info.corners, info.major_extent, info.minor_extent);
		float boundingBoxRatio = getBoundingBoxRatio(region, info.centroid, info.major_axis);
		float SecondOrderMomentAboutCentralAxis = getSecondOrderMomentAboutCentralAxis(region, info.major_axis, info.centroid);
		features.centroid = point(info.centroid.x, info.centroid.y);
		features.features.push_back(percentFilled);
		features.features.push_back(boundingBoxRatio);
		features.features.push_back(SecondOrderMomentAboutCentralAxis);
		region_features.push_back(features);
	}
	
	//std::cout << "got region features" << std::endl;
	if(visualize){
		drawRegionInformation(src, proper_info); //press x to exit
		dest = src.clone();
	} else{
		dest = src.clone();
	}
	if(dest.empty()){
		dest = src.clone();
	}

	return region_features;
}


void drawRegionInformation(cv::Mat& src, std::vector<Region_information>  region_info){
	//cv::cvtColor(src, src, cv::COLOR_GRAY2RGB);
	for(auto it = region_info.begin(); it != region_info.end(); ++it){
			Region_information info = (*it);
			
			for(int i = 0; i < 4; i++){
				int end = (i+1)%4;
				line(src, cv::Point(info.corners[i].x, info.corners[i].y), 
						cv::Point(info.corners[end].x, info.corners[end].y),
						cv::Scalar(0, 0, 255), 4);
			}/*
			line(src, cv::Point(info.corners[0].x, info.corners[0].y), 
					cv::Point(info.corners[1].x, info.corners[1].y),
					cv::Scalar(0, 0, 255), 4);
			line(src, cv::Point(info.corners[2].x, info.corners[2].y), 
					cv::Point(info.corners[3].x, info.corners[3].y),
					cv::Scalar(0, 255, 0), 4);*/
	}
	//arrowedLine(src, Point pt1, Point pt2, const Scalar& color, int thickness=1, int line_type=8, int shift=0, double tipLength=0.1)
}


std::vector<std::vector<point>> getRegionVectors(cv::Mat &src, int numLabels){
	std::vector<std::vector<point>> region_ids = std::vector<std::vector<point>>(numLabels);
	for(int i = 0; i < src.rows; i++){
		for(int j = 0; j < src.cols; j++){
			
			int region = (int)src.at<uchar>(cv::Point(j, i));
			point p = point(j, i);

			region_ids[region].push_back(p);
		}
	}
	return region_ids;
}


int getMoment(int p, int q, std::vector<point> region_points){
	int sum = 0;
	for (auto it = region_points.begin(); it != region_points.end(); ++it){
		point p0 = (*it);
		sum += pow(p0.x, p)*pow(p0.y, q);
	}
	return sum;
}

int getMomentAboutCenter(int p, int q, std::vector<point> region_points, point c0){
	int sum = 0;
	for (auto it = region_points.begin(); it != region_points.end(); ++it){
		point p0 = (*it);
		sum += pow(p0.x - c0.x, p) * pow(p0.y - c0.y, q); 
	}
	return sum;
}
point getCentroid(std::vector<point> region_points){
	float dnom = (float) getMoment(0, 0, region_points);
	int Xc = (int)(getMoment(1, 0, region_points) / dnom);
	int Yc = (int)(getMoment(0, 1, region_points) / dnom);
	return point(Xc, Yc);
}

void grayscaleThreshold(cv::Mat &src, cv::Mat &dest, int threshold ){
	cv::Mat gray;
	cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);
	cv::threshold(gray, dest, threshold, 255, 1);
}

float getMajorAxis(std::vector<point> region_points){

	point centroid = getCentroid(region_points);
	float alpha  = 0.5 * std::atan2( (2 * getMomentAboutCenter(1, 1, region_points, centroid)) ,
	 							getMomentAboutCenter(2, 0, region_points, centroid) - 
								getMomentAboutCenter(0, 2, region_points, centroid));
	return alpha;
}

 float getExtent(float axis_angle, std::vector<point> region_points, point centroid){
	float max_val = 0;
	for(auto it = region_points.begin(); it!= region_points.end(); ++it){
		point p0 = (*it);
		float val = project(axis_angle, p0, centroid);
		if(abs(val) > abs(max_val)){
			max_val = val;
		}
	}
	return abs(max_val);
 }

float project(float axis_angle, point p, point centroid){
	float ax = p.x - centroid.x;
	float ay = p.y - centroid.y;

	float bx = cos(axis_angle);
	float by = sin(axis_angle);

	float dot_prod = ax * bx + ay * by;
	return dot_prod;
}

std::vector<point> getOrientedBoundingBoxCorners(std::vector<point> region_points, point centroid, float major_axis){

	float minor_axis = major_axis + M_PI/2;
	
	float major_extent = getExtent(major_axis, region_points, centroid);
	float minor_extent = getExtent(minor_axis, region_points, centroid);


	//std::cout << "oriented bounding box has major axis " << major_axis * 180/M_PI << " with extent " << major_extent << std::endl;
	//std::cout << "oriented bounding box has minor axis " << minor_axis * 180/M_PI << " with extent " << minor_extent << std::endl;
	//combine extens to get four corners
	//return as array of points

	float major_x_extent = major_extent * cos (major_axis); 
	float minor_x_extent = minor_extent * cos (minor_axis); 
	float major_y_extent = major_extent * sin (major_axis); 
	float minor_y_extent = minor_extent * sin (minor_axis); 

	float x_dist = (major_x_extent + minor_x_extent)/2;
	float y_dist = (major_y_extent + minor_y_extent)/2;

	std::vector<point> corners;

	
	// corners.push_back(point(centroid.x + major_x_extent + minor_x_extent + minor_x_extent,
	// 						centroid.y + major_y_extent + minor_y_extent + minor_y_extent));
							
	// corners.push_back(point(centroid.x + major_x_extent + minor_x_extent - minor_x_extent, 
	// 						centroid.y + major_y_extent + minor_y_extent - minor_y_extent));

	// corners.push_back(point(centroid.x - major_x_extent - minor_x_extent - minor_x_extent, 
	// 						centroid.y - major_y_extent - minor_y_extent - minor_y_extent));

	// corners.push_back(point(centroid.x - major_x_extent - minor_x_extent + minor_x_extent, 
	// 						centroid.y - major_y_extent - minor_y_extent + minor_y_extent));
	
	//  corners.push_back(point(centroid.x - x_dist,
	//  						centroid.y + y_dist));
							
	//  corners.push_back(point(centroid.x - x_dist, 
	//  						centroid.y -y_dist));

	//  corners.push_back(point(centroid.x +x_dist , 
	//  						centroid.y -y_dist));

	//  corners.push_back(point(centroid.x +x_dist, 
	//  						centroid.y +y_dist));

	corners.push_back(point(centroid.x + major_x_extent - minor_x_extent,
							centroid.y + major_y_extent - minor_y_extent));
							
	corners.push_back(point(centroid.x + major_x_extent + minor_x_extent, 
							centroid.y + major_y_extent + minor_y_extent));

	corners.push_back(point(centroid.x - major_x_extent + minor_x_extent, 
							centroid.y - major_y_extent + minor_y_extent));

	corners.push_back(point(centroid.x - major_x_extent - minor_x_extent, 
							centroid.y - major_y_extent - minor_y_extent));
	return corners;
}

float getPercentFilled(std::vector<point> region_points, std::vector<point> corners, float major_extent, float minor_extent){
	int amount_foregroud = getMoment(0, 0, region_points);
	float area = (major_extent * 2) * (minor_extent * 2);
	return amount_foregroud/area;
	
}
float getBoundingBoxRatio(std::vector<point> region_points, point centroid, float major_axis){
	float major_extent = getExtent(major_axis, region_points, centroid);
	float minor_extent = getExtent(major_axis + M_PI/2, region_points, centroid);
	return major_extent/minor_extent;
}

float getSecondOrderMomentAboutCentralAxis(std::vector<point> region_points, float major_axis, point c0){
	float sum = 0;
	float minor_axis = major_axis + M_PI/2;
	for (auto it = region_points.begin(); it != region_points.end(); ++it){
		point p0 = (*it);
		float tmpy = (c0.y - p0.y) * cos(minor_axis);
		
		float tmpx = (p0.x - c0.x) * sin(minor_axis); 
        float tmp = tmpx + tmpy;
		sum += tmp * tmp;
	}
	float N = (float)getMoment(0,0, region_points);
	sum = sum / N;
	//printf("%10.1f %10.1f  %10d %10d %10.1f\n", minor_axis, sum, c0.x, c0.y, N);
	return sum;
}

float getFirstOrderMomentAboutCentralAxis(std::vector<point> region_points, float major_axis, point c0){
	float sum = 0;
	float minor_axis = major_axis + M_PI/2;
	for (auto it = region_points.begin(); it != region_points.end(); ++it){
		point p0 = (*it);
		float tmpy = (c0.y - p0.y) * cos(minor_axis);
		
		float tmpx = (p0.x - c0.x) * sin(minor_axis); 
        float tmp = tmpx + tmpy;
	}
	float N = (float)getMoment(0,0, region_points);
	sum = sum / N;
	//printf("%10.1f %10.1f  %10d %10d %10.1f\n", minor_axis, sum, c0.x, c0.y, N);
	return sum;
}

int labelRegions(cv::Mat &src, cv::Mat &dest, bool visualize){
	cv::Mat tmp;
	int numLabels =	cv::connectedComponents(src, tmp, 4);
	

	if(visualize){
		float scale = 255/numLabels;
	  	for( auto it = tmp.begin<int>(); it != tmp.end<int>(); ++it){
		 	if( *it != 0){
				(*it) = ((*it) * scale);
			}
	 	}
	}
	tmp.convertTo(dest, src.type());
	return numLabels;
}

void erode(cv::Mat &src, cv::Mat &dest,  int iterations, int dim, bool useCross){
	int element_type;
	if (useCross){
		element_type = cv::MORPH_CROSS;
	}
	else {
		element_type = cv::MORPH_RECT;
	}
	cv::Mat element = cv::getStructuringElement(element_type, cv::Size(dim, dim));
	cv::Mat tmp;
	tmp = src.clone();
	for (int i = 0; i < iterations; i++){
		cv::erode(tmp, dest, element);
		tmp = dest.clone();
	}

}
void dilate(cv::Mat &src, cv::Mat &dest,  int iterations, int dim=3, bool useCross=true){
	int element_type;
	if (useCross){
		element_type = cv::MORPH_CROSS;
	}
	else {
		element_type = cv::MORPH_RECT;
	}
	cv::Mat element = cv::getStructuringElement(element_type, cv::Size(dim, dim));
	cv::Mat tmp;
	tmp = src.clone();
	for (int i = 0; i < iterations; i++){
		cv::dilate(tmp, dest, element);
		tmp = dest.clone();
	}
}
