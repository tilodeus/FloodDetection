#include "FDModel.h"

FDModel::FDModel(){
	goodness = 0.0;
}

FDModel::~FDModel(){

}

void FDModel::Analyze()
{
	pass = 0;
	Loader.CreateBands("proba.tif");
	ISODATA.input_natural_waters = Loader.GetNatWatersIM();
	ISODATA.input_referency = Loader.GetRIM();
	while (pass < Loader.GetMaxPasses())
	{
		pass++;
		cout << "Analize pass " + to_string(pass) << endl;
		ISODATA.input_band0 = Loader.GetIB0(pass);
		ISODATA.input_band1 = Loader.GetIB1(pass);
		ISODATA.initISODATA(pass);
		ISODATA.runISODATA();

		goodness = ISODATA.result_goodness;
		cout << goodness << endl;
		goodnesses.push_back(goodness);
	}
	
}

bool FDModel::runUnittests()
{
	//return Loader.runUnittests() && ISODATA.runUnittests();
	return ISODATA.runUnittests();
}












////talan kell meg teszteleshez:
////int x = 3130; int y = 3947;
//int x = 4113; int y = 5773;

//float atlag = 0;
//for (int i = 0; i < 10; ++i){
//	//cout << endl;
//	for (int j = 0; j < 10; ++j){
//		//cout << input_band0.at<float>(3130 + i, 3947 + j) << "  ";
//		atlag += input_band0.at<float>(x + i, y + j);
//	}
//}
//atlag /= 100;
//cout << endl << atlag << endl << endl;
//atlag = 0;
//for (int i = 0; i < 10; ++i){
//	//cout << endl;
//	for (int j = 0; j < 10; ++j){
//		//cout << input_band1.at<float>(3130 + i, 3947 + j) << "  ";
//		atlag += input_band1.at<float>(x + i, y + j);
//	}
//}
//atlag /= 100;
//cout << endl << atlag << endl << endl;