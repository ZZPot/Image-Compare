#define _CRT_RAND_S

#include <stdlib.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "ImageFeature.h"
#include "common.h"



#define TEMPLATE_IMG	"1.jpg"
//Use your own dir, okay?
#define CANDIDATE_DIR	"i:\\Images\\обои11"

#define RANDOM_NAME_MAX_CHARS 30
#define RANDOM_CHUNK_SIZE	5

std::string CreateRandomName(unsigned num_chars, std::string prefix = "", std::string postfix = "");

int main()
{
	cv::Mat templ_img = cv::imread(TEMPLATE_IMG);
	//cv::resize(templ_img, templ_img, cv::Size(), 0.3, 0.3);
	cv::imshow("Template", templ_img);
	image_feature feat(templ_img);
	// for weight based comparsion (center heavier)
	cv::Mat weights(VER_PARTS, HOR_PARTS, CV_64FC1, cv::Scalar::all(0));
	weights.at<double>(VER_PARTS/2, HOR_PARTS/2) = 1;
	cv::GaussianBlur(weights, weights, cv::Size(HOR_PARTS, VER_PARTS), 0);
	Collect collector;
	CrawlFolder(CANDIDATE_DIR, 0, 0, &collector);
	for(auto img_file: collector.file_names)
	{
		cv::Mat img = cv::imread(img_file);
		cv::resize(img, img, templ_img.size(), cv::INTER_CUBIC);
		cv::blur(img, img, cv::Size(3, 3));
		cv::Mat diff = feat.CompareImg(img);
		bool same = CheckCompareMat(diff, weights, 0.1);
		if(same)
		{
			cv::imshow("Same", img);
			CopyFile(img_file.c_str(), CreateRandomName(6, "same\\", ".jpg").c_str(), false);
		}
		else
		{
			cv::imshow("Different", img);
		}
		char key = cv::waitKey(30);
		if(key == 27)
			break;
	}
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