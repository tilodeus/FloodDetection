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

	void LoadImage();
	void Process();
	void Clusterise();
private:
	struct int_spc_el{
		int counter;
		int cluster_index;
	};

	string input_image_name;
	Mat image;
	Mat clusters_image;
	Mat water_image;
	Mat second_cluster_image;
	int_spc_el intensity_space[256][256];
	std::vector<Vec2f> cluster_centers;
	const int band1 = 0;
	const int band2 = 3;
	const int max_it_count = 100;
	const int max_change = 0.1;

	int get_nearest_cluster_center_index(int, int);

	ofstream debug_file;
	string debug;

	void WriteDebug(string message, bool newline);
};