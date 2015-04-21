#include "FDModel.h"

FDModel::FDModel(){
	debug_file.open("debug.txt", std::ios_base::app);
	string debug = "";
	debug_file.close();
}

FDModel::~FDModel(){

}

void FDModel::LoadImage(){
	cout << endl << "  Input image name: ";
	cin >> input_image_name;
	image = imread(input_image_name, CV_32FC4);

	while (!image.data){ // input check
		cout << endl << "Error! File does not exist or can not be opened! Please try again!" << endl;
		cout << "  Input image name: ";
		cin >> input_image_name;
		image = imread(input_image_name, CV_32FC4);
	}
}

void FDModel::Process(){
	//debug:
	debug = "####################################################################################################################################################################################################################\nFile name: " + input_image_name;
	WriteDebug(debug, true);
		//end debug
	
	//---------------------------- INITIALIZATIONS ----------------------------------
	for (int i = 0; i < 256; ++i){
		for (int j = 0; j < 256; ++j){
			intensity_space[i][j].counter = 0;
		}
	}

	const int mystery_width = ceil(image.cols*0.75); //this is the (max number+1) that the image's columns can be indexed with

	cluster_centers.clear();
	//for (int i = 1; i < 21; ++i)
	//	cluster_centers.push_back(Vec2f(12*i, 12*i));
	for (int i = 1; i < 13; ++i)
		cluster_centers.push_back(Vec2f(21 * i, 21 * i));
	cout << endl << "original cluster centers:" << endl; for (int i = 0; i < cluster_centers.size(); ++i) cout << cluster_centers[i] << endl;//!--!

	std::vector<int> cluster_member_count;
	std::vector<Vec2f> cluster_average;
	for (int i = 0; i < cluster_centers.size(); ++i){
		cluster_member_count.push_back(0);
		cluster_average.push_back(Vec2f(0, 0));
	}


	//-------------------------- COMPUTING THE MATRIX OF THE INTENSITY SPACE --------------------------------------
	for (int i = 0; i < image.rows; ++i){
		for (int j = 0; j < mystery_width; ++j){
			int band1_value = (int)image.at<Vec4b>(i, j)[band1];
			int band2_value = (int)image.at<Vec4b>(i, j)[band2];
			if ( !(band1_value == 0 && band2_value == 0) )
				intensity_space[band1_value][band2_value].counter += 1;
		}
	}
	
	//debug:
	debug = "0:_______________________________________________________________________________________________________________________________________________________________________________________________________\n\t";
	for (int i = 0; i < cluster_centers.size(); i++)
	{
		debug += "[" + to_string(cluster_centers[i][0]) + "," + to_string(cluster_centers[i][1]) + "]\t\t\t";
	}
	debug += "\n\t";
	for (int i = 0; i < cluster_centers.size(); i++)
	{
		debug += "0\t\t\t";
	}
	WriteDebug(debug, true);
		//end debug

	//---------------------------- CLUSTERISING ALGORITHM ------------------------------------
	bool stop = false;
	for (int iteration_count = 0; iteration_count < max_it_count && !stop; ++iteration_count){

		for (int i = 0; i < 256; ++i){
			for (int j = 0; j < 256; ++j){
				if (intensity_space[i][j].counter != 0){
					int nearest_cluster_center_index = get_nearest_cluster_center_index(i, j);
					intensity_space[i][j].cluster_index = nearest_cluster_center_index;
					
					if (cluster_average[nearest_cluster_center_index][0] != 0)
						cluster_average[nearest_cluster_center_index][0] =
						(float)cluster_member_count[nearest_cluster_center_index] / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].counter)
						* cluster_average[nearest_cluster_center_index][0]
						+
						(float)intensity_space[i][j].counter / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].counter)
						* i;
					else
						cluster_average[nearest_cluster_center_index][0] = i;

					if (cluster_average[nearest_cluster_center_index][1] != 0)
						cluster_average[nearest_cluster_center_index][1] =
							(float)cluster_member_count[nearest_cluster_center_index] / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].counter)
							* cluster_average[nearest_cluster_center_index][1]
							+
							(float)intensity_space[i][j].counter / (cluster_member_count[nearest_cluster_center_index] + intensity_space[i][j].counter)
							* j;
					else
						cluster_average[nearest_cluster_center_index][1] = j;

					cluster_member_count[nearest_cluster_center_index] += intensity_space[i][j].counter;
				}
			}
		}
		
		//debug:
		debug = to_string(iteration_count + 1) + ":_______________________________________________________________________________________________________________________________________________________________________________________________________\n\t";
		for (int i = 0; i < cluster_centers.size(); i++)
		{
			debug += "[" + to_string(cluster_average[i][0]) + "," + to_string(cluster_average[i][1]) + "]\t\t\t";
		}
		debug += "\n\t";
		for (int i = 0; i < cluster_member_count.size(); i++)
		{
			debug += to_string(cluster_member_count[i]) + "\t\t\t";
		}
		WriteDebug(debug, true);
			//end debug
		
		cout << endl << " " << iteration_count << ":";//!--!
		stop = true;
		for (int i = 0; i < cluster_centers.size(); ++i){
			float cluster_center_movement = sqrt(
				(cluster_centers[i][0] - cluster_average[i][0]) * (cluster_centers[i][0] - cluster_average[i][0])
				+
				(cluster_centers[i][1] - cluster_average[i][1]) * (cluster_centers[i][1] - cluster_average[i][1])
				);
			if (cluster_center_movement > max_change) stop = false;
			cluster_centers[i] = cluster_average[i];			cout << endl << cluster_centers[i];//!--!
			cluster_member_count[i] = 0;
			cluster_average[i] = Vec2f(0, 0);
		}
		cout << endl;//!--!
	}


	//--------------------------- COMPUTING THE OUTPUT IMAGES ---------------------------------
	Mat intensity_space_image = Mat(256, 256, CV_8UC1);
	for (int i = 0; i < 256; ++i){
		for (int j = 0; j < 256; ++j){
			intensity_space_image.at<uchar>(i, j) = ((intensity_space[i][j].counter != 0) ? 255 : 0);
		}
	}

	Mat intensity_space_im_RGB(256, 256, CV_8UC3);
	for (int i = 0; i < 256; ++i){
		for (int j = 0; j < 256; ++j){
			int counter = intensity_space[i][j].counter;
			if (counter == 0)
				intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
			else if (counter < 8)
				intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(40, 40, 0);
			else if (counter < 40)
				intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(60, 60, 0);
			else if (counter < 200)
				intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(90, 90, 0);
			else if (counter < 1000)
				intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(170, 170, 0);
			else if (counter < 5000)
				intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(255, 255, 0);
			else
				intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(255, 255, 180);
		}
	}

	clusters_image = Mat(image.rows, mystery_width, CV_8UC1);
	water_image = Mat(image.rows, mystery_width, CV_8UC1);
	second_cluster_image = Mat(image.rows, mystery_width, CV_8UC1);
	for (int i = 0; i < image.rows; ++i){
		for (int j = 0; j < mystery_width; ++j){
			int band1_value = (int)image.at<Vec4b>(i, j)[band1];
			int band2_value = (int)image.at<Vec4b>(i, j)[band2];
			if (!(band1_value == 0 && band2_value == 0)){
				clusters_image.at<uchar>(i, j) = intensity_space[band1_value][band2_value].cluster_index * (255.0 / (cluster_centers.size()));
				switch (intensity_space[band1_value][band2_value].cluster_index){
				case 0:
					water_image.at<uchar>(i, j) = 0;
					second_cluster_image.at<uchar>(i, j) = 255;
					break;
				case 1:
					water_image.at<uchar>(i, j) = 255;
					second_cluster_image.at<uchar>(i, j) = 0;
					break;
				default:
					water_image.at<uchar>(i, j) = 255;
					second_cluster_image.at<uchar>(i, j) = 255;
					break;
				}
			}
			else{
				clusters_image.at<uchar>(i, j) = 0;
				water_image.at<uchar>(i, j) = 255;
				second_cluster_image.at<uchar>(i, j) = 255;
			}
		}
	}

	imwrite("00intensity_space_image.bmp", intensity_space_image);
	imwrite("00intensity_space_im_RGB.bmp", intensity_space_im_RGB);
	//imwrite("00intensity_space_clusters.bmp", intensity_space_clusters);
	imwrite("00clusters.bmp", clusters_image);
	imwrite("00water.bmp", water_image);
	imwrite("00second_cluster.bmp", second_cluster_image);

	cout << endl << "Press ENTER to continue!" << endl;
	cin.get(); cin.get();
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

///This function saves some run-time parameters into a text file.
///Studying these values might help the developer to understand:
///what the hell is happening when the algorythm is not working as it should be.
void FDModel::WriteDebug(string message, bool newline)
{
	debug_file.open("debug.txt", std::ios_base::app);
	if (!newline)
		debug_file << message;
	else
		debug_file << message << endl;
	debug_file.close();
}




//Mat intensity_space_clusters(256, 256, CV_8UC3);
//for (int i = 0; i < 256; ++i){
//	for (int j = 0; j < 256; ++j){
//		//int counter = intensity_space[i][j].counter;
//		//if (counter == 0)
//		//	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
//		//else if (counter < 8)
//		//	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(40, 40, 0);
//		//else if (counter < 40)
//		//	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(60, 60, 0);
//		//else if (counter < 200)
//		//	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(90, 90, 0);
//		//else if (counter < 1000)
//		//	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(170, 170, 0);
//		//else if (counter < 5000)
//		//	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(255, 255, 0);
//		//else
//		//	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(255, 255, 180);
//		int index = intensity_space[i][j].cluster_index;
//		switch (index){
//		case 0: intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21 * index, 10 * index, 5 * index); break;
//		case 1:	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21*index, 10*index, 5*index); break;
//		case 2:	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21*index, 10*index, 5*index); break;
//		case 3:	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21*index, 10*index, 5*index); break;
//		case 4:	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21*index, 10*index, 5*index); break;
//		case 5:	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21*index, 10*index, 5*index); break;
//		case 6:	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21*index, 10*index, 5*index); break;
//		case 7:	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21*index, 10*index, 5*index); break;
//		case 8:	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21*index, 10*index, 5*index); break;
//		case 9:	intensity_space_im_RGB.at<Vec3b>(i, j) = Vec3b(0,0,100); break;//Vec3b(21*index, 10*index, 5*index); break;
//		case 10: intensity_space_im_RGB.at<Vec3b>(i, j) =Vec3b(0,0,100); break;// Vec3b(21 * index, 10 * index, 5 * index); break;
//		case 11: intensity_space_im_RGB.at<Vec3b>(i, j) =Vec3b(0,0,100); break;// Vec3b(21 * index, 10 * index, 5 * index); break;
//		case 12: intensity_space_im_RGB.at<Vec3b>(i, j) =Vec3b(0,0,100); break;// Vec3b(21 * index, 10 * index, 5 * index); break;
//		default: intensity_space_im_RGB.at<Vec3b>(i, j) =Vec3b(0,0,100); break;// Vec3b(0, 0, 0); break;
//		}
//	}
//}
