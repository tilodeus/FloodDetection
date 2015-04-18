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
		cout << endl << "Press ENTER to try again!" << endl;
		cin.get(); cin.get();
		return;
	}

	cout << endl << "image.rows: " << image.rows << "  image.cols: " << image.cols << endl;

	//cout << endl << (int)image.at<Vec4b>(107, 122)[band1] << endl; //k3.tif max. hatarai (k3.tif egyebkent (108 X 164)-es)
	//cout << endl << (int)image.at<Vec4b>(7510, 6300)[band1] << endl; //1.tif max. hatarai (1.tif egyebkent (7511 X 8401)-es)

	for (int i = 0; i < 256; ++i){
		for (int j = 0; j < 256; ++j){
			//intensity_space[i][j].first = 0;
			intensity_space[i][j].counter = 0;
		}
	}
	
	//const int mystery_width = 123; //k3.tif-nel
	const int mystery_width = 6301; //1.tif-nel

	for (int i = 0; i < image.rows; ++i){
		for (int j = 0; j < mystery_width; ++j){
			int band1_value = (int)image.at<Vec4b>(i, j)[band1];
			int band2_value = (int)image.at<Vec4b>(i, j)[band2];
			//intensity_space[band1_value][band2_value].first += 1;
			intensity_space[band1_value][band2_value].counter += 1;
		}
	}

	cluster_centers.push_back(Vec2i(51, 51));
	cluster_centers.push_back(Vec2i(102, 102));
	cluster_centers.push_back(Vec2i(153, 153));
	cluster_centers.push_back(Vec2i(204, 204));

	cout << endl << "original cluster centers:" << endl;
	for (int i = 0; i < cluster_centers.size(); ++i){
		cout << cluster_centers[i] << endl;
	}

	std::vector<int> cluster_member_count;
	std::vector<Vec2i> cluster_average;

	for (int i = 0; i < cluster_centers.size(); ++i){
		cluster_member_count.push_back(0);
		cluster_average.push_back(Vec2i(0, 0));
	}
	
	for (int iteration_count = 0; iteration_count < 2; ++iteration_count){

		for (int i = 0; i < 256; ++i){
			for (int j = 0; j < 256; ++j){
				//if (intensity_space[i][j].first != 0){
				if (intensity_space[i][j].counter != 0){
					int nearest_cluster_center_index = get_nearest_cluster_center_index(i, j);
					//intensity_space[i][j].second = nearest_cluster_center_index;
					intensity_space[i][j].cluster_index = nearest_cluster_center_index;

					//cluster_average[nearest_cluster_center_index][0] += i;// *intensity_space[i][j].first;
					//cluster_average[nearest_cluster_center_index][1] += j;// *intensity_space[i][j].first;
					
					cluster_average[nearest_cluster_center_index][0] = 
						//(float)cluster_member_count[nearest_cluster_center_index] / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].first)
						(float)cluster_member_count[nearest_cluster_center_index] / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].counter)
							* cluster_average[nearest_cluster_center_index][0]
						+
						//(float)intensity_space[i][j].first / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].first)
						(float)intensity_space[i][j].counter / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].counter)
							* i;

					cluster_average[nearest_cluster_center_index][1] =
						//(float)cluster_member_count[nearest_cluster_center_index] / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].first)
						(float)cluster_member_count[nearest_cluster_center_index] / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].counter)
						* cluster_average[nearest_cluster_center_index][1]
						+
						//(float)intensity_space[i][j].first / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].first)
						(float)intensity_space[i][j].counter / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].counter)
						* j;

					//cluster_member_count[nearest_cluster_center_index] += intensity_space[i][j].first;
					cluster_member_count[nearest_cluster_center_index] += intensity_space[i][j].counter;
				}
			}

		}

		if (iteration_count == 1){
			cout << endl << "cluster member count:" << endl;
			for (int i = 0; i < cluster_centers.size(); ++i){
				cout << cluster_member_count[i] << endl;
			}
		}

		for (int i = 0; i < cluster_average.size(); ++i){
			//cluster_average[i] /= cluster_member_count[i];
			cluster_centers[i] = cluster_average[i];

			cluster_member_count[i] = 0;
			cluster_average[i] = Vec2i(0, 0);
		}
	}

	cout << endl << "final cluster centers:" << endl;
	for (int i = 0; i < cluster_centers.size(); ++i){
		cout  << cluster_centers[i] << endl;
	}

			
	Mat intensity_space_image = Mat(256, 256, CV_8UC1);
	for (int i = 0; i < 256; ++i){
		for (int j = 0; j < 256; ++j){
			//intensity_space_image.at<uchar>(i, j) = ((intensity_space[i][j].first != 0) ? 255 : 0);
			intensity_space_image.at<uchar>(i, j) = ((intensity_space[i][j].counter != 0) ? 255 : 0);
		}
	}

	clusters_image = Mat(image.rows, mystery_width, CV_8UC1);
	output_image = Mat(image.rows, mystery_width, CV_8UC1);
	for (int i = 0; i < image.rows; ++i){
		for (int j = 0; j < mystery_width; ++j){
			int band1_value = (int)image.at<Vec4b>(i, j)[band1];
			int band2_value = (int)image.at<Vec4b>(i, j)[band2];
			if (!(band1_value == 0 && band2_value == 0)){
				//clusters_image.at<uchar>(i, j) = intensity_space[band1_value][band2_value].second * (255.0 / (cluster_centers.size()));
				clusters_image.at<uchar>(i, j) = intensity_space[band1_value][band2_value].cluster_index * (255.0 / (cluster_centers.size()));
				//if (intensity_space[band1_value][band2_value].second == 0){
				if (intensity_space[band1_value][band2_value].cluster_index == 0){
					output_image.at<uchar>(i, j) = 0;
				}
				else{
					output_image.at<uchar>(i, j) = 255;
				}
			}
			else{
				clusters_image.at<uchar>(i, j) = 255;
			}
		}
	}
	
	//namedWindow("original image", WINDOW_AUTOSIZE);
	//imshow("original image", image);
	namedWindow("intensity space image", WINDOW_AUTOSIZE);
	imshow("intensity space image", intensity_space_image);
	imwrite("00intensity_space_image.bmp", intensity_space_image);
	namedWindow("clusters", WINDOW_AUTOSIZE);
	imshow("clusters", clusters_image);
	imwrite("00clusters.bmp", clusters_image);
	namedWindow("water", WINDOW_AUTOSIZE);
	imshow("water", output_image);
	imwrite("00water.bmp", output_image);
	waitKey(0);

	cout << endl << "Press ENTER to continue!" << endl;
	cin.get(); cin.get();
}

void FDModel::preProcess(){

}

void FDModel::Process(){

}

int FDModel::get_nearest_cluster_center_index(int i, int j){
	int nearest_cluster_center_index;
	double min_distance = 1000;
	for (int cluster_center_index = 0; cluster_center_index < cluster_centers.size(); ++cluster_center_index){
		double distance = sqrt(
							(cluster_centers[cluster_center_index][0] - i)*
							(cluster_centers[cluster_center_index][0] - i) +
							(cluster_centers[cluster_center_index][1] - j)*
							(cluster_centers[cluster_center_index][1] - j)
						);
		if (distance < min_distance){
			nearest_cluster_center_index = cluster_center_index;
			min_distance = distance;
		}
	}

	return nearest_cluster_center_index;
}
//salalala



/*switch ((intensity_space[band1_value][band2_value].second)){
				case 0: clusters_image.at<uchar>(i, j) = 0; break;
				case 1: clusters_image.at<uchar>(i, j) = 80; break;
				case 2: clusters_image.at<uchar>(i, j) = 170; break;
				case 3: clusters_image.at<uchar>(i, j) = 255; break;
				}*/