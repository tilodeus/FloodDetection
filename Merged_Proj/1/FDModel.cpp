#include "FDModel.h"

FDModel::FDModel(){

}

FDModel::~FDModel(){

}

void FDModel::LoadImage(){
	string input_image;

	cout << endl << "Input image name: ";
	cin >> input_image;

	image = imread(input_image, CV_32FC4);

	if (!image.data){ // input check
		cout << endl << "Error! File does not exist or can not be opened!" << endl;
		cout << endl << "Press ENTER to exit!" << endl;
		cin.get(); cin.get();
	}
	else {
		Vec4b test_OpenCV_vec;
		test_OpenCV_vec = image.at<Vec4b>(60, 35);
		cout << endl << (int)image.at<Vec4b>(60, 35)[3] << endl;
		cout << endl << (int)test_OpenCV_vec[3] << endl;

		const int image_height = image.rows;
		const int image_width = image.cols;
	}
}

void FDModel::preProcess(){

}

void FDModel::Process(){

}
//salalala