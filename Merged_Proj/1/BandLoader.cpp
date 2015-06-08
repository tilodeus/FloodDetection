#include "BandLoader.h"

BandLoader::BandLoader()
{
	bandcount = 0;
}

BandLoader::~BandLoader()
{
}

void BandLoader::CreateBands(string input_filename)
{
	//THIS PART IS NOT WORKING:
	//GDALDataset *poDataset;
	//GDALAllRegister();
	////open multibanded .tif and oad it into poDataset
	//poDataset = (GDALDataset *)GDALOpenShared(input_filename.c_str(), GA_ReadOnly);
	//if (poDataset == NULL)
	//{
	//	cout << " An error occured during accessing/loading the file." << endl;
	//}
	//else
	//{
	//	//iterate through the file's bands and write each of them into a band#.tif 1 banded file
	//	for (int i = 1; i <= poDataset->GetRasterCount(); i++)
	//	{
	//		//destination dataset
	//		GDALDataset *poDstDS;
	//		//driver used for destination dataset is the same as the one we opened the input file
	//		GDALDriver *poDriver;
	//		poDriver = poDataset->GetDriver();
	//		//raster band we will fill from the input file
	//		GDALRasterBand *poBand;
	//		//output file name
	//		string target = "input/band" + to_string(i) + ".tif";
	//		//create output file
	//		//PARAMS: filename, sizeX,sizeY, number of bands, type of data, options
	//		poDstDS = poDriver->Create(target.c_str(), poDataset->GetRasterXSize(), poDataset->GetRasterYSize(), 1, GDT_Byte, NULL);
	//		//set dest file's getrans from input file
	//		poDstDS->SetGeoTransform(poDataset->GetGeoTransform);
	//		//copy duffer size ????
	//		GByte abyRaster[512 * 512];
	//		//access dest file's raster band
	//		poBand = poDstDS->GetRasterBand(1);
	//		//write band values into opened raster band (FROM WHERE????)
	//		poBand->RasterIO(GF_Write, 0, 0, poDataset->GetRasterXSize(), poDataset->GetRasterySize(), abyRaster, 512, 512, GDT_Byte, 0, 0);
	//		//close the opened dataset (close output file)
	//		GDALClose((GDALDatasetH)poDstDS);
	//	}
	//}
	//THIS PART WORKS:

	countBands();

	CreateIBOrder();
}

Mat BandLoader::GetIB0(int pass)
{
	IB0.release();
	IB0 = imread("input/"+ib_permutations[(pass-1) * 2], CV_LOAD_IMAGE_UNCHANGED);
	if (!IB0.data){ // input check
		ex.setErrorMessage("In BandLoader::GetIB0() failed to load the " + ib_permutations[(pass - 1) * 2]);
		throw ex;
	}
	return IB0;
}

Mat BandLoader::GetIB1(int pass)
{
	IB1.release();
	IB1 = imread("input/" + ib_permutations[(pass-1) * 2 + 1], CV_LOAD_IMAGE_UNCHANGED);
	if (!IB1.data){ // input check
		ex.setErrorMessage("In BandLoader::GetIB1() failed to load the " + ib_permutations[(pass - 1) * 2]);
		throw ex;
	}
	return IB1;
}

Mat BandLoader::GetRIM()
{
	Referency_Im = imread("input/referency.tif", CV_LOAD_IMAGE_GRAYSCALE);
	if (!Referency_Im.data){ // input check
		ex.setErrorMessage("In BandLoader::GetRIM() failed to load the referency.tif");
		throw ex;
	}
	return Referency_Im;
}

Mat BandLoader::GetNatWatersIM()
{
	NatWaters_Im = imread("input/natural_waters.tif", CV_8UC1);
	if (!NatWaters_Im.data){ // input check
		ex.setErrorMessage("In BandLoader::GetNatWatersIM() failed to load the natural_waters.tif");
		throw ex;
	}
	return NatWaters_Im;
}

int BandLoader::GetMaxPasses()
{
	return ib_permutations.size() / 2;
}

void BandLoader::countBands() 
{
	char *dir = "input";
	struct dirent *dp;
	DIR *fd;

	if ((fd = opendir(dir)) == NULL) {
		ex.setErrorMessage("Cannot open 'input' directory in BandLoader::countBands().");
		throw ex;
	}
	while ((dp = readdir(fd)) != NULL) {
		if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
			continue;    /* skip self and parent */
		if (strstr(dp->d_name,"band") != NULL)
			bandcount++;
		//printf("%s/%s\n", dir, dp->d_name);
	}
	closedir(fd);
}

void BandLoader::CreateIBOrder()
{
	ib_permutations.clear();
	for (int i = 0; i < bandcount; i++)
	{
		for (int j = i + 1; j < bandcount; j++)
		{
			ib_permutations.push_back("band" + to_string(i) + ".tif");
			ib_permutations.push_back("band" + to_string(j) + ".tif");
		}
	}
}







//UNITTESTS
bool BandLoader::UT_CreateIBOrder()
{
	bandcount = 3;
	bool l = false;
	vector<string> assert_vec;
	assert_vec.push_back("band0.tif");
	assert_vec.push_back("band1.tif");
	assert_vec.push_back("band0.tif");
	assert_vec.push_back("band2.tif");
	assert_vec.push_back("band1.tif");
	assert_vec.push_back("band2.tif");
	CreateIBOrder();
	for (int i = 0; i < assert_vec.size() && !l; i++){
		l = (assert_vec[i] == ib_permutations[i]);
	}
	bandcount = 0;
	ib_permutations.clear();
	if (l)
		return true;
	else
	{
		ex.setErrorMessage("BandLoader::UT_CreateIBOrder() failed.");
		throw ex;
	}
}

bool BandLoader::UT_GetMaxPasses()
{
	
	for (int i = 0; i < 4; i++)
		ib_permutations.push_back("band" + to_string(i));
	if (!(GetMaxPasses() == 2)){
		ib_permutations.clear();
		ex.setErrorMessage("BandLoader::UT_GetMaxPasses() failed.");
		throw ex;
	}
	else{
		ib_permutations.clear();
		return true;
	}
}

bool BandLoader::runUnittests()
{
	if (UT_CreateIBOrder() && UT_GetMaxPasses())
		return true;
}