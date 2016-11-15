#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "ImageFeature.h"
#include "common.h"
#include <iostream>

#define TEMPLATE_IMG	"1.jpg"
//Use your own dir, okay?
#define CANDIDATE_DIR	"i:\\Images\\Nature_Wallpapers_1920x1080"

int main()
{
	cv::Mat templ_img = cv::imread(TEMPLATE_IMG);
	cv::resize(templ_img, templ_img, cv::Size(), 0.3, 0.3);
	cv::imshow("Template", templ_img);
	image_feature feat(templ_img);
	cv::Mat weights(VER_PARTS, HOR_PARTS, CV_64FC1, cv::Scalar::all(1));
	weights.at<double>(VER_PARTS/2, HOR_PARTS/2) = 1;
	cv::GaussianBlur(weights, weights, cv::Size(HOR_PARTS, VER_PARTS), 0);
	Collect collector;
	CrawlFolder(CANDIDATE_DIR, 0, 0, &collector);
	for(auto img_file: collector.file_names)
	{
		cv::Mat img = cv::imread(img_file);
		cv::resize(img, img, templ_img.size());
		cv::Mat diff = feat.CompareImg(img);
		bool same = CheckCompareMat(diff, weights, 5000);
		if(same)
		{
			cv::imshow("Same", img);
		}
		else
		{
			cv::imshow("Different", img);
		}
		char key = cv::waitKey(0);
		if(key == 27)
			break;
	}
	return 0;
}