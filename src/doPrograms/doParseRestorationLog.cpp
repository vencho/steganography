#include"../header.cpp"

#define MAX_QUERIES 50000
#define EPSILON 0.000001



int main(int argc, char *argv[])
{
if(argc != 3)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("doParseRestorationLog rlogFile maxQueries\n\n");
	exit(0);
	}


string inputFile;
vecDouble v;
double dist; 
int i, queries;
FILE *fin;

v.resize(MAX_QUERIES + 1);
for(i = 0; i < v.size(); i++) v[i] = 0;

inputFile = string(argv[1]);
fin = fopen(inputFile.c_str(), "rb");
while(1)
	{
	if(fscanf(fin, "%lf %d\n", &dist, &i) != 2) break;
	v[i] = dist;
	queries = i;
	}
fclose(fin);

queries = atoi(argv[2]);

for(i = 1; i <= queries; i++)
	if(v[i] < EPSILON)
		v[i] = v[i-1];

for(i = 0; i <= queries; i++)
	ECHO("%.12lf ", v[i]);
ECHO("\n");

return 0;
}


