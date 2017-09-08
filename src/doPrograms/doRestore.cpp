#include"../header.cpp"
#include"../utils/matrixIO.cpp"
#include"../utils/matrixArithmetic.cpp"
#include"../utils/norms.cpp"
#include"../pgmReader/pgmReader.cpp"

#include"../WAM/DWTCalculation.cpp"
#include"../WAM/WAMCalculation.cpp"
#include"../WAM/WAMUpdater.cpp"

#include"../restorers/Restorer.cpp"
#include"../restorers/RestorerIneff.cpp"
#include"../restorers/RestorerGreedy.cpp"
#include"../restorers/RestorerRand.cpp"
#include"../restorers/RestorerGenetic.cpp"
#include"../restorers/RestorerVarianceSort.cpp"
#include"../restorers/RestorerQP.cpp"




int main(int argc, char *argv[])
{
Restorer *R;
srand(time(0));
if(argc <= 5)
	{
	ECHO("Too few arguments. Usage:\n\n");
	ECHO("doRestore algorithm originalImage stegoImage embeddingLog [arguments] queryLimit queryStep distanceFile target\n\n");
	ECHO("Available algorithms: ineff, greedy, rand, genetic, varsort, qp\n");
	ECHO("queryStep specifies how often intermediate semi-restored images are output.\n");
	ECHO("distanceFile contains a mean feature vector and an inverse covariance matrix.\n");
	ECHO("target is either mean or original.\n");
	exit(0);
	}

string algorithm = string(argv[1]);
string originalImage = string(argv[2]);
string stegoImage = string(argv[3]);
string embeddingLog = string(argv[4]);
int lots, changesPerLot;
int initialSize;
int queryLimit;
int queryStep;
string distanceFile;
string target;

if(algorithm == "rand")
	{
	if(argc != 11) exit(0);

	lots = atoi(argv[5]);
	changesPerLot = atoi(argv[6]);
	queryLimit = atoi(argv[7]);
	queryStep = atoi(argv[8]);
	distanceFile = string(argv[9]);
	target = string(argv[10]);

	R = new RestorerRand(originalImage, stegoImage, embeddingLog, lots, changesPerLot, queryLimit, queryStep, distanceFile, target);
	}
else if(algorithm == "genetic")
	{
	if(argc != 10) exit(0);

	initialSize = atoi(argv[5]);
	queryLimit = atoi(argv[6]);
	queryStep = atoi(argv[7]);
	distanceFile = string(argv[8]);
	target = string(argv[9]);

	R = new RestorerGenetic(originalImage, stegoImage, embeddingLog, initialSize, queryLimit, queryStep, distanceFile, target);
	}
else if(algorithm == "ineff" || algorithm == "greedy" || algorithm == "varsort" || algorithm == "qp")
	{
	if(argc != 9) exit(0);
	queryLimit = atoi(argv[5]);
	queryStep = atoi(argv[6]);
	distanceFile = string(argv[7]);
	target = string(argv[8]);

	if(algorithm == "ineff")
		R = new RestorerIneff(originalImage, stegoImage, embeddingLog, queryLimit, queryStep, distanceFile, target);
	else if(algorithm == "greedy")
		R = new RestorerGreedy(originalImage, stegoImage, embeddingLog, queryLimit, queryStep, distanceFile, target);
	else if(algorithm == "varsort")
		R = new RestorerVarianceSort(originalImage, stegoImage, embeddingLog, queryLimit, queryStep, distanceFile, target);
	else if(algorithm == "qp")
		R = new RestorerQP(originalImage, stegoImage, embeddingLog, queryLimit, queryStep, distanceFile, target);
	}
else
	{ exit(0); }


(*R).apply();
return 0;
}


