#include"../header.cpp"


vecDouble mean;
vecDouble stddev;
matrixDouble logs;

string file;
int N, Q;



void getStd();
void output();
void getMean();
void initialise();
void input();


int main(int argc, char *argv[])
{

if(argc != 4)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("doParseAllLogs allLogsFile images maxQueries\n");
	exit(0);
	}

file = argv[1];
N = atoi(argv[2]);
Q = atoi(argv[3]);

initialise();
input();
getMean();
getStd();
output();

return 0;
}

void input()
{
int i, j;
FILE *fin = fopen(file.c_str(), "r");
for(i = 0; i < N; i++)
	for(j = 0; j <= Q; j++)
		fscanf(fin, "%lf", &logs[i][j]);
fclose(fin);
for(i = 0; i < N; i++)
	{
	for(j = 1; j <= Q; j++) logs[i][j] /= logs[i][0];
	logs[i][0] = 1;
	}
}



void initialise()
{
int i, j;
mean.resize(Q + 1);
stddev.resize(Q + 1);
resizeMatrix(logs, N, Q);
for(i = 0; i <= Q; i++) mean[i] = stddev[i] = 0;
}



void getMean()
{
int i, j;

for(i = 0; i < N; i++)
	for(j = 0; j <= Q; j++) 
		mean[j] += (logs[i][j] / N);
}

void getStd()
{
int i, j;

for(i = 0; i < N; i++)
	for(j = 0; j <= Q; j++)
		stddev[j] += ((logs[i][j] - mean[j])*(logs[i][j] - mean[j]) / (N - 1));
for(j = 0; j <= Q; j++)
	stddev[j] = sqrt(stddev[j]);
}



void output()
{
int i;

for(i = 0; i <= Q; i++)
	ECHO("%.16lf ", mean[i]);
ECHO("\n");

for(i = 0; i <= Q; i++)
	ECHO("%.16lf ", stddev[i]);
ECHO("\n");
}

