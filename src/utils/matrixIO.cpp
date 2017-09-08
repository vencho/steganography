//------------------------------------------------------------------------
// READ MATRIX ROUTINES

// Integer matrix from file pointer
void loadMatrix(FILE *fin, matrixInt &mx)
{
int i, j;
int N, M;


fscanf(fin, "%d%d", &N, &M);

mx.clear();
mx.resize(N); for(i = 0; i < N; i++) mx[i].resize(M);

for(i = 0; i < N; i++)
    for(j = 0; j < M; j++)
        fscanf(fin, "%d", &mx[i][j]);
}


// Integer matrix from file name
void loadMatrix(string inName, matrixInt &mx)
{
FILE *fin = fopen(inName.c_str(), "rb");
loadMatrix(fin, mx);
fclose(fin);
}



// Double matrix from file pointer
void loadMatrix(FILE *fin, matrixDouble &mx)
{
int i, j, N, M;

fscanf(fin, "%d%d", &N, &M);

mx.clear();
mx.resize(N); for(i = 0; i < N; i++) mx[i].resize(M);

for(i = 0; i < N; i++)
    for(j = 0; j < M; j++)
        fscanf(fin, "%lf", &mx[i][j]);
}


// Double matrix from file name
void loadMatrix(string inName, matrixDouble &mx)
{
FILE *fin = fopen(inName.c_str(), "rb");
loadMatrix(fin, mx);
fclose(fin);
}

// END OF READ MATRIX ROUTINES
//------------------------------------------------------------------------










//------------------------------------------------------------------------
// WRITE MATRIX ROUTINES








// Integer matrix to a file pointer
void writeMatrix(FILE *fout, matrixInt &mx)
{
int i, j, N, M;
N = mx.size(); M = mx[0].size();

FECHO(fout, "%d %d\n", N, M);
for(i = 0; i < N; i++)
    {
    for(j = 0; j < M-1; j++)
        FECHO(fout, "%d ", mx[i][j]);
    FECHO(fout, "%d\n", mx[i][j]);
    }

}


// Integer matrix to a file
void writeMatrix(string outName, matrixInt &mx)
{
FILE *fout = fopen(outName.c_str(), "wb");
writeMatrix(fout, mx);
fclose(fout);
}








// Double matrix to a file pointer
void writeMatrix(FILE *fout, matrixDouble &mx)
{
int i, j, N, M;
N = mx.size(); M = mx[0].size();

FECHO(fout, "%d %d\n", N, M);
for(i = 0; i < N; i++)
    {
    for(j = 0; j < M-1; j++)
        FECHO(fout, "%.25lf ", mx[i][j]);
    FECHO(fout, "%.35lf\n", mx[i][j]);
    }

}


// Double matrix to a file name
void writeMatrix(string outName, matrixDouble &mx)
{
FILE *fout = fopen(outName.c_str(), "wb");
writeMatrix(fout, mx);
fclose(fout);
}



// END OF WRITE MATRIX ROUTINES
//------------------------------------------------------------------------













//------------------------------------------------------------------------
// WRITE VECTOR ROUTINES




// Vector to file pointer
void writeVector(FILE *fout, vecDouble &v)
{
int i;
FECHO(fout, "%d\n", v.size());
for(i = 0; i < v.size()-1; i++)
	FECHO(fout, "%.16lf ", v[i]);
FECHO(fout, "%.16lf\n", v[i]);
}

// Vector to a file pointer, without the size.
void writeVectorEntries(FILE *fout, vecDouble &v)
{
int i;
for(i = 0; i < v.size()-1; i++)
	FECHO(fout, "%.16lf ", v[i]);
FECHO(fout, "%.16lf\n", v[i]);
}


// Vector to file name
void writeVector(string outName, vecDouble &v)
{
FILE *fout = fopen(outName.c_str(), "wb");
writeVector(fout, v);
fclose(fout);
}



//------------------------------------------------------------------------



void readVector(FILE *fin, vecDouble &v)
{
int i, N;
fscanf(fin, "%d", &N);
v.resize(N);
for(i = 0; i < N; i++) fscanf(fin, "%lf", &v[i]);
}


void readVector(string inName, vecDouble &v)
{
FILE *fin = fopen(inName.c_str(), "rb");
readVector(fin, v);
fclose(fin);
}




void readSequenceOfVectors(string inputFile, vector<vecDouble > &samples)
{
samples.clear();

int i, N;
FILE *fin = fopen(inputFile.c_str(), "r");
while(1)
	{
	if(fscanf(fin, "%d", &N) != 1) break;
	vecDouble v;
	v.resize(N);
	for(i = 0; i < N; i++) fscanf(fin, "%lf", &v[i]);
	samples.push_back(v);
	}
fclose(fin);
}




