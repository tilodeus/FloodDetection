#include "FDModel.h"

FDModel::FDModel(){
	goodness = 0.0;
}

FDModel::~FDModel(){

}

void FDModel::Analyze()
{
	pass = 0;
	Loader.CreateBands("input/multi.tif");
	ISODATA.input_natural_waters = Loader.GetNatWatersIM();
	ISODATA.input_referency = Loader.GetRIM();
	while (pass < 1)//Loader.GetMaxPasses())
	{
		pass++;
		ISODATA.input_band0 = Loader.GetIB0(pass);
		ISODATA.input_band1 = Loader.GetIB1(pass);
		ISODATA.initISODATA(pass);
		ISODATA.runISODATA();

		goodness = ISODATA.result_goodness;
		///!!!!!!!
		cout << goodness << endl;
		goodnesses.push_back(goodness);
	}
	
}

bool FDModel::runUnittests()
{
	return Loader.runUnittests() && ISODATA.runUnittests();
}