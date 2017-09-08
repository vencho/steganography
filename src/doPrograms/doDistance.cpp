#include"../header.cpp"
#include"../utils/matrixIO.cpp"
#include"../utils/matrixArithmetic.cpp"
#include"../utils/norms.cpp"


int main(int argc, char *argv[])
{
vecDouble u, v;
double x;

if(argc != 4)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("doDistance vectorA vectorB distanceFile\n\n");
	exit(0);
	}

string fileA = string(argv[1]);
string fileB = string(argv[2]);
string distance = string(argv[3]);

prepareDistance(distance);
readVector(fileA, u);
readVector(fileB, v);

x = Distance(u, v);

ECHO("%.16lf\n", x);

return 0;
}

