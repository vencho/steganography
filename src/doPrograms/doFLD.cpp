#include"../header.cpp"
#include"../utils/matrixIO.cpp"
#include"../utils/matrixArithmetic.cpp"
#include"../utils/vectorStatistics.cpp"
#include"../utils/GaussJordan.cpp"

using namespace std;

void findWeight();
void massDot();
void findAccuracy();
void findStabilityValues();


// The sets of samples.
vector<vecDouble > trainingA, trainingB, testingA, testingB; // Training cover wams, training stego wams, testing cover wams, testing stego wams.
vector<vecDouble > allA, allB; // All cover wams (both testing and training), all stego wams (both testing and training).

// FLD weights.
vecDouble  weights;	

// Used for tuning the threshold to find the best accuracy an opponent can achieve with this weights vector.
vector<pair<double, int> > v;

// Output values.
double ACCURACY, VAL_ONE, VAL_TWO, VAL_THREE;

int main(int argc, char *argv[])
{
if(argc != 5)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("FLD trainingA trainingB testingA testingB\n\n");
	exit(0);
	}

readSequenceOfVectors(argv[1], trainingA);
readSequenceOfVectors(argv[2], trainingB);
readSequenceOfVectors(argv[3], testingA);
readSequenceOfVectors(argv[4], testingB);
allA.assign(trainingA.begin(), trainingA.end());
allA.insert(allA.end(), testingA.begin(), testingA.end());
allB.assign(trainingB.begin(), trainingB.end());
allB.insert(allB.end(), testingB.begin(), testingB.end());


findWeight();
massDot();
findAccuracy();
findStabilityValues();

ECHO("%lf %lf %lf %lf\n", ACCURACY, VAL_ONE, VAL_TWO, VAL_THREE);

return 0;
}





void findWeight()
{
vecDouble meanTrA, meanTrB;
matrixDouble covTrA, covTrB;

calculateMeanAndCovariance(trainingA, meanTrA, covTrA);
calculateMeanAndCovariance(trainingB, meanTrB, covTrB);

matrixDouble covInClass = covTrA + covTrB;
GaussJordanCalculation G = GaussJordanCalculation();
matrixDouble covInClassInv = G.inverse(covInClass);


vecDouble meanTrDif = meanTrA - meanTrB;
weights = covInClassInv * meanTrDif;
}



void massDot()
{
int i;
double x;
for(i = 0; i < testingA.size(); i++)
	{
	x = testingA[i] * weights;
	v.push_back(make_pair(x, 0));
	}
for(i = 0; i < testingB.size(); i++)
	{
	x = testingB[i] * weights;
	v.push_back(make_pair(x, 1));
	}
}





void findAccuracy()
{
int i, j;
int N = v.size();

sort(v.begin(), v.end());


int zeroesLeft, onesLeft, zeroesRight, onesRight;
int preferredFalses;
int bestFalses = N; int besti;
for(i = 0; i < N; i++)
	{
	zeroesLeft = onesLeft = 0;
	for(j = 0; j < i; j++)
		{
		if(v[j].second == 0) zeroesLeft++;
		else onesLeft++;
		}
	zeroesRight = onesRight = 0;
	for(j = i; j < N; j++)
		{
		if(v[j].second == 0) zeroesRight++;
		else onesRight++;
		}

	preferredFalses = MIN(zeroesLeft + onesRight, zeroesRight + onesLeft);
	if(preferredFalses < bestFalses)
		{
		bestFalses = MIN(bestFalses, preferredFalses);
		besti = i;
		}


	/* If left is 0 and right is 1,
	then correct ones are:
		zeroesLeft + onesRight
	and false ones are:
		zeroesRight + onesLeft */
	}

zeroesLeft = onesLeft = 0;
for(j = 0; j < besti; j++)
	{
	if(v[j].second == 0) zeroesLeft++;
	else onesLeft++;
	}
zeroesRight = onesRight = 0;
for(j = besti; j < N; j++)
	{
	if(v[j].second == 0) zeroesRight++;
	else onesRight++;
	}

ACCURACY = (N - bestFalses) / (double) N;
//ECHO("Least number of falses is %d out of %d, accuracy is %lf.\n", bestFalses, N, (N - bestFalses) / (double) N);
}



void findStabilityValues()
{
vecDouble meanA, meanB;
matrixDouble covA, covB;
calculateMeanAndCovariance(allA, meanA, covA);
calculateMeanAndCovariance(allB, meanB, covB);
GaussJordanCalculation G = GaussJordanCalculation();

vecDouble meanDif = meanB - meanA;
matrixDouble covInvA, covInvB;
covInvA = G.inverse(covA);
covInvB = G.inverse(covB);

vecDouble temp;

// VAL_ONE = (mu_1 - mu_0) * inv(cov_0) * (mu_1 - mu_0)
temp = covInvA * meanDif;
VAL_ONE = meanDif * temp;

// VAL_TWO = (mu_1 - mu_0) * inv(cov_1) * (mu_1 - mu_0)
temp = covInvB * meanDif;
VAL_TWO = meanDif * temp;

// VAL_THREE = ln(det cov_1) - ln(det cov_0) + trace(inverse(cov_1) * cov_0 - I)
double detA = G.det(covA);
double detB = G.det(covB);
matrixDouble tmp = covInvB * covA;
for(int i = 0; i < tmp.size(); i++) tmp[i][i] -= 1;
VAL_THREE = log(detB) - log(detA) + G.trace(tmp);
}

