/*
This source file provides four routines for working with
pgm images. The maximum grey value is assumed to be 255 throughout.

1)
void pgmToMatrix(string fileName, matrixDouble &mx)
Reads a pgm image and loads it into a given matrix.

2)
void matrixToPgm(matrixDouble &mx, string fileName)
Creates a pgm image from a matrix of doubles. The doubles are
all floored.

3)
void plainToMatrix(string fileName, matrixDouble &mx)
Reads an image in plain pgm form and loads it into a matrix.

4)
void matrixToPlain(matrixDouble &mx, string fileName)
Writes an image from a matrix of doubles into plain pgm format.
All doubles are floored.
*/




/* 
A tiny routine which checks how many numbers appear at the start of
a given string, to a maximum of three. This is need to work around
comments in pgms.
*/
int countNumbersInString(string s)
{
int a, b, c;
return (sscanf(s.c_str(), "%d%d%d", &a, &b, &c));
}


void pgmToMatrix(string fileName, matrixDouble &mx)
{
int N, M;
int i, j;
FILE *fin = fopen(fileName.c_str(), "rb");




fscanf(fin, "P5");
fscanf(fin, "%*[\n\r\t ]"); // This line ignores all whitespace.

/*
    The code below works around comments.
    Read lines and concatenate them,
    ignoring ones that start with #.
    For each line, count how many numbers it contains
    by feeding it to sscanf. When that count reaches 3,
    we know we've exhausted the header. Feed the concatenated
    result once more to sscanf to actually read the three
    header numbers, ignore the one whitespace character
    after them, and proceed to reading the matrix.
*/
string temp1;
char temp2[1024];
temp1 = "";
int numbersScanned = 0;
while(numbersScanned < 3)
    {
    fgets(temp2, 1024, fin);
    if(temp2[0] != '#')
        {
        numbersScanned += countNumbersInString(string(temp2));
        temp1 += string(temp2);
        }
    }
sscanf(temp1.c_str(), "%d %d 255%*c",&M, &N);

mx.clear();
mx.resize(N);
for(i = 0; i < N; i++) mx[i].resize(M);

/*
    This is where the description of the actual image begins.
    We are to read N rows of M characters,
    each from 0 to 255 describing the intensities of pixels
    (0 being black, 255 being white).
	We put the pixels into the matrix.
*/
for(i = 0; i < N; i++)
    for(j = 0; j < M; j++)
		{
        unsigned char temp = fgetc(fin);
		mx[i][j] = (double) temp;
        }
fclose(fin);
}






void matrixToPgm(matrixDouble &mx, string fileName)
{
int N, M;
int i, j;

N = mx.size();
M = mx[0].size();

FILE *fout = fopen(fileName.c_str(), "w");

FECHO(fout, "P5\n");
FECHO(fout, "%d %d\n255\n", M, N);

for(i = 0; i < N; i++)
    for(j = 0; j < M; j++)
		{
		if(mx[i][j] > 255) ECHO("WARNING\n");
		if(mx[i][j] < 0) ECHO("WARNING\n");
		FECHO(fout, "%c", (char)MAX(0, MIN(255, mx[i][j])));
		}

fclose(fout);
}










void plainToMatrix(string fileName, matrixDouble &mx)
{
int N, M;
int i, j;

FILE *fin = fopen(fileName.c_str(), "rb");
fscanf(fin, "%d %d 255", &M, &N);


mx.clear();
mx.resize(N);
for(i = 0; i < N; i++) mx[i].resize(M);


for(i = 0; i < N; i++)
    for(j = 0; j < M; j++)
        fscanf(fin, "%lf", &mx[i][j]);
fclose(fin);
}



void matrixToPlain(matrixDouble &mx, string fileName)
{
int N, M;
int i, j;

N = mx.size();
M = mx[0].size();

FILE *fout = fopen(fileName.c_str(), "wb");
FECHO(fout, "%d %d\n255\n", M, N);
for(i = 0; i < N; i++)
    {
    for(j = 0; j < M; j++)
		FECHO(fout, "%d ", (int)mx[i][j]);
	FECHO(fout, "\n");
    }
fclose(fout);
}

