#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <direct.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "MyException.h"

using namespace cv;
using namespace std;

class OurISODATA
{
	public:
		OurISODATA();
		~OurISODATA();

		void initISODATA(int _pass);
		void runISODATA();

		Mat input_band0, input_band1;
		Mat input_natural_waters;
		Mat input_referency;

		double result_goodness;

		bool runUnittests();
	private:
		struct int_spc_el{
			int counter;
			int cluster_index;
		};

		int pass;
		int input_im_rows, input_im_cols;

		int_spc_el intensity_space[256][256];
		vector<Vec2f> cluster_centers;

		Mat intensity_space_im;
		Mat clusters_im;
		Mat clusters_with_nat_wat_im;
		Mat flooded_areas_im;
		Mat errormap_im;

		const int max_it_count = 50;
		const int allowed_movement = 0.5;

		ofstream debug_file;
		string debug;

		void compute_intensity_space();
		void clusterise();
		void compute_output_images();
		void write_images_to_file();
		void draw_cluster_center(Vec2f);
		double compare_solution(int param);
		int get_nearest_cluster_center_index(int, int);
		void WriteDebug(string message, bool newline);

		bool UT_get_nearest_cluster_center_index();
		bool UT_compare_solution();

		MyException ex;

};