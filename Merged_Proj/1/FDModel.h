#include <iostream>
#include <vector>
#include <stdio.h>
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
	Mat image;
	Mat clusters_image;
	Mat output_image;
	std::pair<int, int> intensity_space[256][256];
	std::vector<Vec2i> cluster_centers;
	const int band1 = 3;
	const int band2 = 1;

	int get_nearest_cluster_center_index(int, int);
};