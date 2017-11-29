#pragma once
#include "use_opencv.h"

class Histogram1D {
private:

	int histSize[1]; // 빈도수
	float hranges[2]; // 최소/최대 화소값
	const float* ranges[1];
	int channels[1]; // 여기서 1채널만 사용

public:
	Histogram1D();

	// 정의한 멤버 변수로 그레이레벨 영상의 히스토그램을 계산할 때는 다음 메소드를 사용해 수행
	cv::MatND getHistogram(const cv::Mat &image);

};