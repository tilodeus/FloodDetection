#include "FDModel.h"

FDModel::FDModel(){
	debug_file.open("debug.txt", std::ios_base::app);
	string debug = "";
	debug_file.close();
}

FDModel::~FDModel(){

}

void FDModel::Init(){
	for (int i = 0; i < 256; ++i){
		for (int j = 0; j < 256; ++j){
			intensity_space[i][j].counter = 0;
		}
	}

	cluster_centers.clear();
	for (int i = 1; i < 21; ++i)
		cluster_centers.push_back(Vec2f(12*i, 12*i));

	//debug:
	debug = "####################################################################################################################################################################################################################\nFile name: " + input_image_name;
	WriteDebug(debug, true);

	debug = "0:_______________________________________________________________________________________________________________________________________________________________________________________________________\n\t";
	for (int i = 0; i < cluster_centers.size(); i++)	{
		debug += "[" + to_string(cluster_centers[i][0]) + "," + to_string(cluster_centers[i][1]) + "]\t\t\t";
	}
	debug += "\n\t";
	for (int i = 0; i < cluster_centers.size(); i++)	{
		debug += "0\t\t\t";
	}
	WriteDebug(debug, true);
	//end debug
}

void FDModel::LoadImage(){
	system("cls");
	cout << endl << " Name of input image: ";
	cin >> input_image_name;
	input_image = imread("input/" + input_image_name, CV_32FC4);

	while (!input_image.data){ // input check
		system("cls");
		cout << "\nProgram's message:\n\n Error! File does not exist or can not be opened! Please try again!" << endl;
		cout << endl << " Name of input image: ";
		cin >> input_image_name;
		input_image = imread("input/" + input_image_name, CV_32FC4);
	}

	system("cls");
	cout << endl << " Name of natural water's image: ";
	cin >> input_nat_wat_name;
	input_natural_waters = imread("input/" + input_nat_wat_name, CV_8UC1);

	while (!input_natural_waters.data){ // input check
		system("cls");
		cout <<  "\nProgram's message:\n\n Error! File does not exist or can not be opened! Please try again!" << endl;
		cout << endl << " Name of natural water's image: ";
		cin >> input_nat_wat_name;
		input_natural_waters = imread("input/" + input_nat_wat_name, CV_8UC1);
	}
}

void FDModel::Process(){
	system("cls");
	cout << "\nProgram's message:\n\n\n   Processing..." << endl;

	compute_intensity_space();

	clusterise();

	compute_output_images();
	
	write_images_to_file();

	cout << "\n\nOutput images are ready. Press ENTER to continue!" << endl;
	cin.get(); cin.get();
}

void FDModel::clusterise(){
	std::vector<int> cluster_member_count;
	std::vector<Vec2f> cluster_average;
	for (int i = 0; i < cluster_centers.size(); ++i){
		cluster_member_count.push_back(0);
		cluster_average.push_back(Vec2f(0, 0));
	}

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

		stop = true;
		for (int i = 0; i < cluster_centers.size(); ++i){
			float cluster_center_movement = sqrt(
				(cluster_centers[i][0] - cluster_average[i][0]) * (cluster_centers[i][0] - cluster_average[i][0])
				+
				(cluster_centers[i][1] - cluster_average[i][1]) * (cluster_centers[i][1] - cluster_average[i][1])
				);
			if (cluster_center_movement > allowed_movement) stop = false;
			cluster_centers[i] = cluster_average[i];
			cluster_member_count[i] = 0;
			cluster_average[i] = Vec2f(0, 0);
		}
	}
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

void FDModel::compute_intensity_space(){
	for (int i = 0; i < input_image.rows; ++i){
		for (int j = 0; j < ceil(input_image.cols*0.75); ++j){
			int band1_value = (int)input_image.at<Vec4b>(i, j)[band1];
			int band2_value = (int)input_image.at<Vec4b>(i, j)[band2];
			if (!(band1_value == 0 && band2_value == 0))
				intensity_space[band1_value][band2_value].counter += 1;
		}
	}
}

void FDModel::compute_output_images(){
	intensity_space_image = Mat(256, 256, CV_8UC1);
	intensity_space_im_RGB = Mat(256, 256, CV_8UC3);
	for (int i = 0; i < 256; ++i){
		for (int j = 0; j < 256; ++j){
			intensity_space_image.at<uchar>(i, j) = ((intensity_space[i][j].counter != 0) ? 255 : 0);

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

	clusters_image = Mat(input_image.rows, ceil(input_image.cols*0.75), CV_8UC1);
	clusters_of_water_im = Mat(input_image.rows, ceil(input_image.cols*0.75), CV_8UC3);;
	nat_wat_and_fl_areas_im = Mat(input_image.rows, ceil(input_image.cols*0.75), CV_8UC3);
	flooded_areas_image = Mat(input_image.rows, ceil(input_image.cols*0.75), CV_8UC1);
	for (int i = 0; i < input_image.rows; ++i){
		for (int j = 0; j < ceil(input_image.cols*0.75); ++j){
			int band1_value = (int)input_image.at<Vec4b>(i, j)[band1];
			int band2_value = (int)input_image.at<Vec4b>(i, j)[band2];
			if (!(band1_value == 0 && band2_value == 0)){
				clusters_image.at<uchar>(i, j) = intensity_space[band1_value][band2_value].cluster_index * (255.0 / (cluster_centers.size()));
		
				bool natural_water_pixel = false;
				if (i >= 1 && i < input_image.rows - 1 && j >= 1 && j < ceil(input_image.cols*0.75)-1 ){
					for (int k = i - 1; k < i + 2; ++k){
						for (int l = j - 1; l < j + 2; ++l){
							if ((int)input_natural_waters.at<uchar>(k, l*(4 / 3.0f)) != 255) natural_water_pixel = true;
						}
					}
				}

				switch (intensity_space[band1_value][band2_value].cluster_index){
				case 0: case 1:
					clusters_of_water_im.at<Vec3b>(i, j) = Vec3b(255, 0, 0);
					natural_water_pixel ? nat_wat_and_fl_areas_im.at<Vec3b>(i, j) = Vec3b(255, 255, 0) : nat_wat_and_fl_areas_im.at<Vec3b>(i, j) = Vec3b(255, 0, 0);
					natural_water_pixel ? flooded_areas_image.at<uchar>(i, j) = 255 : flooded_areas_image.at<uchar>(i, j) = 100;
					break;
				case 2:	case 3:	case 4:	case 5: case 6:	case 7:
					clusters_of_water_im.at<Vec3b>(i, j) = Vec3b(212, 212, 212);
					nat_wat_and_fl_areas_im.at<Vec3b>(i, j) = Vec3b(212, 212, 212);
					flooded_areas_image.at<uchar>(i, j) = 255;
					break;
				case 8: case 9:	case 10: case 11: case 12: case 13: case 14:
				case 15: case 16: case 17: case 18: case 19: case 20:
					clusters_of_water_im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
					nat_wat_and_fl_areas_im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
					flooded_areas_image.at<uchar>(i, j) = 255;
					break;
				default:
					clusters_of_water_im.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
					nat_wat_and_fl_areas_im.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
					flooded_areas_image.at<uchar>(i, j) = 0;
					break;
				}
			}
			else{
				clusters_image.at<uchar>(i, j) = 255;
				clusters_of_water_im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
				nat_wat_and_fl_areas_im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
				flooded_areas_image.at<uchar>(i, j) = 255;
			}
		}
	}

}

void FDModel::write_images_to_file(){
	imwrite("output/01_intensity_space.bmp", intensity_space_image);
	imwrite("output/02_intensity_space_RGB.bmp", intensity_space_im_RGB);
	imwrite("output/03_clusters.bmp", clusters_image);
	imwrite("output/04_clusters_of_water.bmp", clusters_of_water_im);
	imwrite("output/05_nat_wat_and_fl_areas.bmp", nat_wat_and_fl_areas_im);
	imwrite("output/06_flooded_areas.bmp", flooded_areas_image);
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

