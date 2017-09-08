#include"../header.cpp"
#include"../utils/GaussJordan.cpp"
#include"../utils/matrixIO.cpp"


int main(int argc, char *argv[])
{
if(argc != 2)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("doInvert matrixFile\n");
	exit(0);
	}
matrixDouble A, B;

loadMatrix(argv[1], A);
GaussJordanCalculation G = GaussJordanCalculation();
B = G.inverse(A);

writeMatrix(stdout, B);
return 0;
}



