#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>;
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "dirent.h"
#include "MyException.h"
//#include GDAL
//#include "gdal_priv.h"
//#include "cpl_conv.h"
//#include "cpl_string.h"

using namespace cv;
using namespace std;

class BandLoader
{
public:
	BandLoader();
	~BandLoader();

	//CreateBands() would create the bands from the .tif file with the help of GDAL and put it into an input folder.
	//IN:  input_filename - the name of the multiband .tif file to be processed
	//OUT: input folder filled with 1band .tifs created from the input file
	void CreateBands(string input_filename);

	//If it's given, then load the natural waters image
	void LoadNaturalWatersImage(string input_filename);
	//If it's given, then load the referency image
	void LoadReferencyImage(string input_filename);

	//Getters
	Mat GetIB0(int pass);
	Mat GetIB1(int pass);
	Mat GetRIM();
	Mat GetNatWatersIM();
	int GetMaxPasses();

	bool runUnittests();
private:

	int bandcount;
	vector<string> ib_permutations;
	Mat IB0;
	Mat IB1;
	Mat Referency_Im;
	Mat NatWaters_Im;
	//CreateIBOrder() opens the input folder and creates an order of the bands to be processed.
	//IN:  -
	//OUT: [band_name] - array of name of band files in all possible permutations.
	void CreateIBOrder();
	//vector<string>
	void countBands();

	bool UT_GetMaxPasses();
	bool UT_CreateIBOrder();

	MyException ex;
};