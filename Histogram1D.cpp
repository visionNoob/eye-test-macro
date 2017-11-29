#pragma once
#include "Histogram1D.hpp"
#include "use_opencv.h"


Histogram1D::Histogram1D() { // 1차원 히스토그램을 위한 인자 준비
	histSize[0] = 256;
	hranges[0] = 0.0;
	hranges[1] = 255.0;
	ranges[0] = hranges;
	channels[0] = 0; // 기본적으로 채널을 0으로 보기
}

// 정의한 멤버 변수로 그레이레벨 영상의 히스토그램을 계산할 때는 다음 메소드를 사용해 수행
cv::MatND Histogram1D::getHistogram(const cv::Mat &image) {
	// 1차원(1D) 히스토그램 계산.
	cv::MatND hist;
	cv::calcHist(&image, // 히스토그램 계산 
		1,   // 단일 영상의 히스토그램만
		channels, // 대상 채널               
		cv::Mat(), // 마스크 사용하지 않음     
		hist,  // 결과 히스토그램         
		1,   // 1차원(1D) 히스토그램           
		histSize, // 빈도수                  
		ranges  // 화소값 범위             
	);
	return hist;
}
