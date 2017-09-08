#include"../header.cpp"
#include"../utils/matrixIO.cpp"
#include"../utils/matrixArithmetic.cpp"
#include"../utils/vectorStatistics.cpp"


vector<vecDouble > samples;
vecDouble mean;
matrixDouble cov;

int main(int argc, char *argv[])
{
if(argc != 2)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("doCovariance wamfile\n\n");
	exit(0);
	}

string file = string(argv[1]);
readSequenceOfVectors(file, samples);
calculateMeanAndCovariance(samples, mean, cov);

writeVector(stdout, mean);
writeMatrix(stdout, cov);

return 0;
}


