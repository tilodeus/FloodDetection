#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;

class FDModel{
public:
	FDModel();
	~FDModel();

	void Init();
	void LoadImage();
	void Process();
private:
	struct int_spc_el{
		int counter;
		int cluster_index;
	};

	string input_image_name;
	string input_nat_wat_name;
	Mat input_image;
	Mat input_natural_waters;
	
	int_spc_el intensity_space[256][256];
	
	std::vector<Vec2f> cluster_centers;

	Mat intensity_space_image;
	Mat intensity_space_im_RGB;
	Mat clusters_image;
	Mat clusters_of_water_im;
	Mat nat_wat_and_fl_areas_im;
	Mat flooded_areas_image;

	const int band1 = 0;
	const int band2 = 3;
	const int max_it_count = 100;
	const int allowed_movement = 0.1;

	ofstream debug_file;
	string debug;

	int get_nearest_cluster_center_index(int, int);
	void compute_intensity_space();
	void clusterise();
	void compute_output_images();
	void write_images_to_file();

	void WriteDebug(string message, bool newline);
};