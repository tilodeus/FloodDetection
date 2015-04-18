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
	void preProcess();
	void Process();
	void Clusterise();
	//salalala
private:
	struct int_spc_el{
		int counter;
		int cluster_index;
	};

	Mat image;
	Mat clusters_image;
	Mat output_image;
	//std::pair<int, int> intensity_space[256][256];
	int_spc_el intensity_space[256][256];
	std::vector<Vec2i> cluster_centers;
	const int band1 = 3;
	const int band2 = 1;

	int get_nearest_cluster_center_index(int, int);

	ofstream debug_file;
	string debug;

	void WriteDebug(string message, bool newline);
};