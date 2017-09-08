




/* -------------------------------------------------------- */
/* MAHALANOBIS INITIALISATION. */

matrixDouble covInv;
vecDouble means;



void prepareDistance(string distanceFile)
{
int i, j;
int N;

FILE *fin = fopen(distanceFile.c_str(), "rb");


fscanf(fin, "%d", &N);
means.resize(N);
for(i = 0; i < N; i++) fscanf(fin, "%lf", &means[i]);


fscanf(fin, "%d%d", &N, &N);
resizeMatrix(covInv, N, N);
for(i = 0; i < N; i++)
	for(j = 0; j < N; j++)
		fscanf(fin, "%lf", &covInv[i][j]);

fclose(fin);
}

/* -------------------------------------------------------- */







/* -------------------------------------------------------- */
/* NORM FUNCTIONS. */

double Norm(vecDouble &v)
{
vecDouble a, b;
int i, j;
int n = means.size();
a = v - means;
b = covInv * a;
double ans = a * b;
ans = sqrt(ans);
return ans;
}

/* -------------------------------------------------------- */




/* -------------------------------------------------------- */
/* DISTANCE FUNCTIONS. */

double Distance(vecDouble &u, vecDouble &v)
{
vecDouble a, b;
int i, j;
int n = u.size();
/*
ECHO("I'm asked to calculate the distance between:\n");
writeVector(stdout, u);
writeVector(stdout, v);
*/
a = u - v;
b = covInv * a;
double ans = a * b;
ans = sqrt(ans);
return ans;
}

/* -------------------------------------------------------- */



