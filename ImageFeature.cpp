#include "ImageFeature.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

image_feature::image_feature(cv::Mat img)
{
	_hist.resize(HOR_PARTS * VER_PARTS);
	if(!img.empty())
		Set(img);
}
void image_feature::Set(cv::Mat img)
{
	cv::Rect roi_rect(0, 0, img.cols/HOR_PARTS, img.rows / VER_PARTS);
	cv::Mat img_hsv;
	cvtColor(img, img_hsv, CV_BGR2HSV);
#ifdef HUE_ONLY
	std::vector<int> channels = {0};
	std::vector<int> hist_size = {HIST_SIZE_H};
	std::vector<float> ranges = {0, 180};
#else
	std::vector<int> channels = {0, 1, 2};
	std::vector<int> hist_size = {HIST_SIZE_H, HIST_SIZE_S, HIST_SIZE_V};
	std::vector<float> ranges = {0, 180, 0, 255, 0, 255};
#endif
	for(unsigned i = 0; i < HOR_PARTS * VER_PARTS; i++)
	{
		roi_rect.x = (i % HOR_PARTS) * roi_rect.width;
		roi_rect.y = (i / HOR_PARTS) * roi_rect.height;
		cv::Mat hsv_chunk = img_hsv(roi_rect);
		std::vector<cv::Mat> chunk_channels;
		cv::split(hsv_chunk, chunk_channels);
#ifdef HUE_ONLY
		chunk_channels.resize(1);
#endif
		cv::calcHist(chunk_channels, channels, cv::Mat(), _hist[i], hist_size, ranges); // 3D histogram
		cv::normalize(_hist[i], _hist[i], NORM_VAL);
	}
}
cv::Mat image_feature::CompareFile(std::string img_file)
{
	return CompareImg(cv::imread(img_file));
}
cv::Mat image_feature::CompareImg(cv::Mat img)
{
	return Compare(image_feature(img));
}
cv::Mat image_feature::Compare(const image_feature& feature)
{
	cv::Mat res(VER_PARTS, HOR_PARTS, CV_64FC1);
	for(unsigned i = 0; i < _hist.size(); i++)
	{
		res.at<double>(i / HOR_PARTS, i % HOR_PARTS) = 
			cv::compareHist(_hist[i], feature._hist[i], cv::HISTCMP_BHATTACHARYYA);
	}
	return res;
}

double CheckCompareMat(cv::Mat cmp_mat, cv::Mat w)
{
	cv::Mat res_mat = cmp_mat.clone();
	cv::multiply(cmp_mat, w, res_mat);
	cv::Scalar diff = cv::sum(res_mat);
	//diff[0] /= res_mat.cols * res_mat.rows;
#ifdef _DEBUG
	std::cout<< "--------------------------\n";
	std::cout<< cmp_mat <<std::endl;
	std::cout<< w <<std::endl;
	std::cout<< res_mat <<std::endl;
	std::cout<< "Diff: " << diff[0] << std::endl;
#endif
	return diff[0];
}