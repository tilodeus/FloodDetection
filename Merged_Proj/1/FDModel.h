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
	void LoadImages();
	void Process();
private:
	struct int_spc_el{
		int counter;
		int cluster_index;
	};

	Mat input_band0, input_band1;
	Mat input_natural_waters;
	int input_im_rows, input_im_cols;
	
	int_spc_el intensity_space[256][256];
	std::vector<Vec2f> cluster_centers;

	Mat intensity_space_im;
	Mat clusters_im;
	Mat clusters_with_nat_wat_im;
	Mat flooded_areas_im;

	const string band0_name = "band0.tif", band1_name = "band3.tif";
	const int max_it_count = 50;
	const int allowed_movement = 0.5;

	ofstream debug_file;
	string debug;

	void compute_intensity_space();
	void clusterise();
	void compute_output_images();
	void write_images_to_file();
	void draw_cluster_center(Vec2f);
	int get_nearest_cluster_center_index(int, int);
	void WriteDebug(string message, bool newline);
};