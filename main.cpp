#define _CRT_RAND_S

#include <stdlib.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "ImageFeature/ImageFeature.h"
#include "common.h"

#pragma warning(disable: 4996 4267)

#define TEMPLATE_IMG	"1.jpg"
//Use your own dir, okay?
#define CANDIDATE_DIR	"W:\\GH\\VS_CV_ImageCompare\\wp"

#define RANDOM_NAME_MAX_CHARS 30
#define RANDOM_CHUNK_SIZE	5

#define WND_NAME_TEMPLATE	"Template"

#define BORDER_W		4
#define SCREEN_W		1280
#define SCREEN_H		1024
#define ROW_SIZE		10
#define SCROLL_MAX		50

#define COLOR_HEIGHT	20


std::string CreateRandomName(unsigned num_chars, std::string prefix = "", std::string postfix = "");
void ShowSame(int new_pos, void* param);
void ShowColors(cv::Mat img);

struct img_to_compare
{
	cv::Mat img;
	image_features feat;
	image_compare_result diff;
	double same[3];
	std::string name;
};

std::vector<img_to_compare> images_to_compare;
cv::Size wnd_size(SCREEN_W/ROW_SIZE - BORDER_W*2, SCREEN_H/ROW_SIZE - BORDER_W*2);

bool img_pred(img_to_compare& img1, img_to_compare& img2)
{
	double diff1 = (img1.same[0]+img1.same[1] + img1.same[2])/3;
	double diff2 = (img2.same[0]+img2.same[1] + img2.same[2])/3;
		
	return diff1 > diff2;
}
int main()
{
	cv::Mat templ_img = cv::imread(TEMPLATE_IMG);
	cv::imshow(WND_NAME_TEMPLATE, templ_img);
	image_features template_feature(templ_img);
	cv::imshow(WND_NAME_TEMPLATE, templ_img);
	cv::createTrackbar("0-1", WND_NAME_TEMPLATE, 0, SCROLL_MAX, ShowSame, nullptr);
	
	// for weight based comparsion (center heavier)
	cv::Mat weights(VER_PARTS, HOR_PARTS, CV_64FC1, cv::Scalar::all(1));
	weights.at<double>(VER_PARTS/2, HOR_PARTS/2) = 1;
	cv::GaussianBlur(weights, weights, cv::Size(HOR_PARTS, VER_PARTS), 0);
	//cv::blur(weights, weights, cv::Size(HOR_PARTS, VER_PARTS));
	cv::Scalar full_w = cv::sum(weights);
	weights /= full_w[0]; 
	Collect collector;
	CrawlFolder(CANDIDATE_DIR, 0, 0, &collector);
	images_to_compare.reserve(collector.file_names.size());
	for(auto img_file: collector.file_names)
	{
		img_to_compare temp;
		temp.name = CreateRandomName(6, "", "_img");
		temp.img = cv::imread(img_file);
		temp.feat.Set(temp.img);
		cv::resize(temp.img, temp.img, wnd_size);
		cv::copyMakeBorder(temp.img, temp.img, BORDER_W, BORDER_W, BORDER_W, BORDER_W,
							cv::BORDER_CONSTANT, cv::Scalar::all(0));
		temp.diff = template_feature.Compare(temp.feat);
		temp.same[0] = CheckCompareMat(temp.diff.hist, weights);
		temp.same[1] = CheckCompareMat(temp.diff.pat, cv::Mat(temp.diff.pat.size(), CV_8UC1, cv::Scalar(1)));
		temp.same[2] = CheckCompareMat(temp.diff.color_diff, weights);
		images_to_compare.push_back(temp);
	}
	sort(images_to_compare.begin(), images_to_compare.end(), img_pred);
	ShowSame(0, nullptr);
	char key = cv::waitKey(0);
	return 0;
}

std::string CreateRandomName(unsigned num_chars, std::string prefix, std::string postfix)
{
	std::string new_name = prefix;
	if(num_chars > RANDOM_NAME_MAX_CHARS)
		num_chars = RANDOM_NAME_MAX_CHARS;
	unsigned u_rand;
	char rand_chunk[RANDOM_CHUNK_SIZE + 1];
	for(; num_chars; num_chars -= cv::min<unsigned>(num_chars, RANDOM_CHUNK_SIZE))
	{
		rand_s(&u_rand);
		itoa(u_rand % (unsigned)pow(10, cv::min<unsigned>(num_chars, RANDOM_CHUNK_SIZE)), rand_chunk, 10);
		new_name += rand_chunk;
	}
	new_name += postfix;
	return new_name;
}
void ShowSame(int new_pos, void* param)
{
	for(unsigned i = 0; i < images_to_compare.size(); i++)
	{
		cv::Scalar color;
		double diff = (images_to_compare[i].same[0]+images_to_compare[i].same[1] + images_to_compare[i].same[2])/3;
		//double diff = images_to_compare[i].same[2];
		if(diff * SCROLL_MAX < (double)new_pos)
			color = cv::Scalar(0, 255, 0);
		else
			color = cv::Scalar(0, 0, 255);
		cv::rectangle(images_to_compare[i].img, cv::Point(0, 0), 
			cv::Point(images_to_compare[i].img.cols - 1, images_to_compare[i].img.rows - 1), color, BORDER_W);
		int top = i / ROW_SIZE * (wnd_size.height+20);
		int left = i % ROW_SIZE * (wnd_size.width + 6);
		cv::imshow(images_to_compare[i].name, images_to_compare[i].img);
		cv::moveWindow(images_to_compare[i].name, left, top);
	}
}