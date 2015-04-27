#include "FDModel.h"

FDModel::FDModel(){
	debug_file.open("debug.txt", std::ios_base::app);
	string debug = "";
	debug_file.close();
}

FDModel::~FDModel(){

}

void FDModel::LoadImages(){
	input_band0 = imread("input/"+band0_name, CV_LOAD_IMAGE_UNCHANGED);
	if (!input_band0.data){ // input check
		system("cls");
		cout << "\nProgram's message:\n\n Error! File does not exist or can not be opened! Please try again!" << endl;
	}
	input_band1 = imread("input/"+band1_name, CV_LOAD_IMAGE_UNCHANGED);
	if (!input_band1.data){ // input check
		system("cls");
		cout << "\nProgram's message:\n\n Error! File does not exist or can not be opened! Please try again!" << endl;
	}
	input_natural_waters = imread("input/termviz.tif", CV_8UC1);
	if (!input_natural_waters.data){ // input check
		system("cls");
		cout << "\nProgram's message:\n\n Error! File does not exist or can not be opened! Please try again!" << endl;
	}

	// HA A STANDARD INPUTROL SZERETNENK BEOLVASNI, AKKOR AZT IGY KELLENE SZEPEN:
	//system("cls");
	//cout << endl << " Name of natural water's image: ";
	//cin >> input_nat_wat_name;
	//input_natural_waters = imread("input/" + input_nat_wat_name, CV_8UC1);

	//while (!input_natural_waters.data){ // input check
	//	system("cls");
	//	cout <<  "\nProgram's message:\n\n Error! File does not exist or can not be opened! Please try again!" << endl;
	//	cout << endl << " Name of natural water's image: ";
	//	cin >> input_nat_wat_name;
	//	input_natural_waters = imread("input/" + input_nat_wat_name, CV_8UC1);
	//}
}

void FDModel::Init(){
	input_im_rows = input_band0.rows;
	input_im_cols = input_band0.cols;
	
	for (int i = 0; i < 256; ++i){
		for (int j = 0; j < 256; ++j){
			intensity_space[i][j].counter = 0;
		}
	}

	cluster_centers.clear();
	for (int i = 1; i < 12; ++i)
		cluster_centers.push_back(Vec2f(21*i, 21*i));

	//debug:
	debug = "####################################################################################################################################################################################################################\nFile name: " + band0_name + ", " + band1_name;
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

void FDModel::compute_intensity_space(){
	for (int i = 0; i < input_im_rows; ++i){
		for (int j = 0; j < input_im_cols; ++j){
			int band1_value = (int)input_band0.at<float>(i, j);
			int band2_value = (int)input_band1.at<float>(i, j);
			if (!(band1_value == 0 && band2_value == 0))
				intensity_space[band1_value][band2_value].counter += 1;
		}
	}
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

void FDModel::compute_output_images(){
	intensity_space_im = Mat(256, 256, CV_8UC3);
	for (int i = 0; i < 256; ++i){
		for (int j = 0; j < 256; ++j){
			int counter = intensity_space[i][j].counter;
			if (counter == 0)
				intensity_space_im.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
			else if (counter < 8)
				intensity_space_im.at<Vec3b>(i, j) = Vec3b(40, 40, 0);
			else if (counter < 40)
				intensity_space_im.at<Vec3b>(i, j) = Vec3b(60, 60, 0);
			else if (counter < 200)
				intensity_space_im.at<Vec3b>(i, j) = Vec3b(90, 90, 0);
			else if (counter < 1000)
				intensity_space_im.at<Vec3b>(i, j) = Vec3b(170, 170, 0);
			else if (counter < 2500)
				intensity_space_im.at<Vec3b>(i, j) = Vec3b(210, 210, 0);
			else if (counter < 4000)
				intensity_space_im.at<Vec3b>(i, j) = Vec3b(255, 255, 0);
			else
				intensity_space_im.at<Vec3b>(i, j) = Vec3b(255, 255, 180);
		}
	}
	for (int i = 0; i < cluster_centers.size(); ++i){
		draw_cluster_center(cluster_centers[i]);
	}

	clusters_im = Mat(input_im_rows, input_im_cols, CV_8UC3);;
	clusters_with_nat_wat_im = Mat(input_im_rows, input_im_cols, CV_8UC3);
	flooded_areas_im = Mat(input_im_rows, input_im_cols, CV_8UC1);
	for (int i = 0; i < input_im_rows; ++i){
		for (int j = 0; j < input_im_cols; ++j){
			int band1_value = (int)input_band0.at<float>(i, j);
			int band2_value = (int)input_band1.at<float>(i, j);
			if (!(band1_value == 0 && band2_value == 0)){
		
				bool natural_water_pixel = false;
				if (i >= 1 && i < input_im_rows - 1 && j >= 1 && j < input_im_cols-1 ){
					for (int k = i - 1; k < i + 2; ++k){
						for (int l = j - 1; l < j + 2; ++l){
							if ((int)input_natural_waters.at<uchar>(k, l) != 255) natural_water_pixel = true;
						}
					}
				}

				int c_i = intensity_space[band1_value][band2_value].cluster_index;
				const int min_int = 140; //minimum intensity for colouring non-water clusters
				const int step = (255-min_int)/(cluster_centers.size()-1); //intensity steps between colours of non-water clusters

				switch (c_i){
				case 0:
					clusters_im.at<Vec3b>(i, j) = Vec3b(255, 0, 0);
					natural_water_pixel ? clusters_with_nat_wat_im.at<Vec3b>(i, j) = Vec3b(255, 255, 0) : clusters_with_nat_wat_im.at<Vec3b>(i, j) = Vec3b(255, 0, 0);
					natural_water_pixel ? flooded_areas_im.at<uchar>(i, j) = 255 : flooded_areas_im.at<uchar>(i, j) = 100;
					break;
				default:
					clusters_im.at<Vec3b>(i, j) = Vec3b(min_int + c_i * step, min_int + c_i * step, min_int + c_i * step);
					clusters_with_nat_wat_im.at<Vec3b>(i, j) = Vec3b(min_int + c_i * step, min_int + c_i * step, min_int + c_i * step);
					flooded_areas_im.at<uchar>(i, j) = 255;
					break;
				}
			}
			else{
				clusters_im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
				clusters_with_nat_wat_im.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
				flooded_areas_im.at<uchar>(i, j) = 255;
			}
		}
	}
}

void FDModel::write_images_to_file(){
	imwrite("output/01_intensity_space_and_cluster_centers.bmp", intensity_space_im);
	imwrite("output/02_clusters.bmp", clusters_im);
	imwrite("output/03_clusters_with_nat_wat.bmp", clusters_with_nat_wat_im);
	imwrite("output/04_flooded_areas.bmp", flooded_areas_im);
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

void FDModel::draw_cluster_center(Vec2f cc){
	int cc0 = (int)cc[0]; int cc1 = (int)cc[1];
	intensity_space_im.at<Vec3b>(cc0, cc1) = Vec3b(0, 220, 255);
	intensity_space_im.at<Vec3b>(cc0, cc1 + 1) = Vec3b(0, 220, 255);
	intensity_space_im.at<Vec3b>(cc0+1, cc1) = Vec3b(0, 220, 255);
	intensity_space_im.at<Vec3b>(cc0+1, cc1 + 1) = Vec3b(0, 220, 255);
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














////talan kell meg teszteleshez:
////int x = 3130; int y = 3947;
//int x = 4113; int y = 5773;

//float atlag = 0;
//for (int i = 0; i < 10; ++i){
//	//cout << endl;
//	for (int j = 0; j < 10; ++j){
//		//cout << input_band0.at<float>(3130 + i, 3947 + j) << "  ";
//		atlag += input_band0.at<float>(x + i, y + j);
//	}
//}
//atlag /= 100;
//cout << endl << atlag << endl << endl;
//atlag = 0;
//for (int i = 0; i < 10; ++i){
//	//cout << endl;
//	for (int j = 0; j < 10; ++j){
//		//cout << input_band1.at<float>(3130 + i, 3947 + j) << "  ";
//		atlag += input_band1.at<float>(x + i, y + j);
//	}
//}
//atlag /= 100;
//cout << endl << atlag << endl << endl;