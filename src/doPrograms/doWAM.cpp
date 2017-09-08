#include"../header.cpp"
#include"../utils/matrixIO.cpp"
#include"../pgmReader/pgmReader.cpp"
#include"../WAM/DWTCalculation.cpp"
#include"../WAM/WAMCalculation.cpp"
#include"../WAM/WAMUpdater.cpp"




int main(int argc, char *argv[])
{
matrixDouble image;
string fileName;

if(argc != 2)
	{
	ECHO("Wrong number of arguments.Usage:\n");
	ECHO("doWAM imageFile\n\n");
	ECHO("Outputs to stdout the wam features of the given image. This format works for doDistance.\n\n");
	exit(0);
	}

fileName = string(argv[1]);
WAMUpdater W = WAMUpdater(fileName, 0);
vecDouble v = W.getWAM();
ECHO("%d ", v.size());
for(int i = 0; i < v.size(); i++)
	ECHO("%.16lf ", v[i]);
ECHO("\n");
return 0;
}

