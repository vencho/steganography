#include"../header.cpp"
#include"../utils/matrixIO.cpp"
#include"../utils/matrixArithmetic.cpp"
#include"../utils/vectorStatistics.cpp"
#include"../utils/GaussJordan.cpp"


string type, wamFile, outputFile;
vector<vecDouble > samples;
vecDouble mean;
matrixDouble cov;
matrixDouble covinv;

void input();
void solve();
void output();


int main(int argc, char *argv[])
{
if(argc != 4)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("doCreateDistance type fileOfWams outputFile\n");
	ECHO("Recognised distance types are euc, mah and diag.\n\n");
	exit(0);
	}

type = string(argv[1]);
wamFile = string(argv[2]);
outputFile = string(argv[3]);
input();
solve();
output();
return 0;
}


void input()
{ readSequenceOfVectors(wamFile, samples); }

void solve()
{
calculateMeanAndCovariance(samples, mean, cov);

int N = mean.size();
int i, j;

resizeMatrix(covinv, N, N)

if(type == "euc")
	{
	for(i = 0; i < N; i++)
		for(j = 0; j < N; j++)
			if(i == j) covinv[i][j] = 1;
			else covinv[i][j] = 0;
	}
else if(type == "diag")
	{
	for(i = 0; i < N; i++)
		for(j = 0; j < N; j++)
			if(i == j) covinv[i][j] = 1. / cov[i][j];
			else covinv[i][j] = 0;
	}
else // type == "mah"
	{
	GaussJordanCalculation G;
	covinv = G.inverse(cov);
	}
}


void output()
{
FILE *fout = fopen(outputFile.c_str(), "wb");
writeVector(fout, mean);
writeMatrix(fout, covinv);
fclose(fout);
}


