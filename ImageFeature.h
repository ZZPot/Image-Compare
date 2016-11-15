#pragma once
#include <opencv2/core.hpp>
#include <vector>
#include <string>

#define HOR_PARTS	1
#define VER_PARTS	(HOR_PARTS)
#define HIST_SIZE_H	36
#define HIST_SIZE_S	4
#define HIST_SIZE_V	4
#define NORM_VAL	180

#define HUE_ONLY
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

bool CheckCompareMat(cv::Mat cmp_mat, cv::Mat w, double max_diff);