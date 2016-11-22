#pragma once
#include <opencv2/core.hpp>
#include <vector>
#include <string>

#define HOR_PARTS	3
#define VER_PARTS	3
#define HIST_SIZE_H	9
#define HIST_SIZE_S	4
#define HIST_SIZE_V	4
#define NORM_VAL	180

#define HUE_ONLY

/*
enum PAT_TYPE
{
	PAT_UNKNOWN = -2,
	PAT_NOISE =-1,
	PAT_PLAIN = 0,

	PAT_LINE_TOP, // -
	PAT_LINE_MIDDLE,
	PAT_LINE_BOTTOM,

	PAT_LINE_LEFT, // |
	PAT_LINE_CENTER,
	PAT_LINE_RIGHT,




};

struct image_feature
{
	std::vector<cv::Scalar> _dom_colors;
	cv::Mat 
};*/


class image_feature
{
public:
	image_feature(cv::Mat img = cv::Mat());
	void Set(cv::Mat img);
	cv::Mat CompareFile(std::string img_file);
	cv::Mat CompareImg(cv::Mat img);
	cv::Mat Compare(const image_feature& feature);
protected:
	std::vector<cv::Mat> _hist;

};

double CheckCompareMat(cv::Mat cmp_mat, cv::Mat w);