#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "BandLoader.h"
#include "OurISODATA.h"


using namespace cv;
using namespace std;

class FDModel{
public:
	FDModel();
	~FDModel();

	void Analyze();
	vector<double> goodnesses;

	bool runUnittests();
private:
	BandLoader Loader;
	OurISODATA ISODATA;
	int pass;
	double goodness;
};