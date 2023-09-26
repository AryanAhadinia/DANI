#include "stdafx.h"
#include <iostream>
#include <string>
using namespace std;
int main(int argc, char *argv[])
{
	Env = TEnv(argc, argv, TNotify::StdNotify);

	Env.PrepArgs(TStr::Fmt("\nDANI. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
	TExeTm ExeTm;

	Try const TStr InFNm = Env.GetIfArgPrefixStr("-i:", argv[1], "Input cascades (one file)");
	const TStr YAL = Env.GetIfArgPrefixStr("-Y:", "Weight", "Output file name(s) prefix");

	DANI dani;
	dani.myfile.open(YAL.CStr());
	printf("\n Loading input cascades:  %s\n", InFNm.CStr());

	//  load cascade from file
	TFIn FIn(InFNm);
	dani.LoadCascadesTxt(FIn);

	dani.runDANI();

	printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
	Catch return 0;
}
