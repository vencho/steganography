// DWTCalculation.cpp


/* 
Some shorthands. Undefined at the end. 
extractRow(MX, i) means x := row i of MX; xLength = N.
extractColumn(MX, j) means x := column j of MX; xLength = M.
putRow(MX, i) means row i of MX := x.
putColumn(MX, j) means column j of MX := x.
*/
#define extractRow(MX, i) { for(j = 0; j < M; j++) x[j] = (MX)[(i)][j]; xLength = M; }
#define extractColumn(MX, j) { for(i = 0; i < N; i++) x[i] = (MX)[i][(j)]; xLength = N; }
#define putRow(v, MX, i) { for(j = 0; j < M; j++) (MX)[(i)][j] = (v)[j]; }
#define putColumn(v, MX, j) { for(i = 0; i < N; i++) (MX)[i][(j)] = (v)[i]; }
#define maxDim 2048
#define maxFilterLength 16


class DWTCalculation
{
/*
The class has no meaningful state space. 
These arrays are used for convenience, 
as temporary storage of the input and
output of the filtering routine.
x[0..xLength) is the input to doFiltering().
yLow[0..xLength) and yHigh[0..xLength) are its output.
lowFilter[0..filterLength) and highFilter[0..filterLength) 
are the two filters.
*/
double yLow[maxDim], yHigh[maxDim];
double lowFilter[maxFilterLength], highFilter[maxFilterLength]; int filterLength;
double x[maxDim]; int xLength;


public:
DWTCalculation() 
	{
    // Set the two filters to be the Daubechies 8-tap QMF.
	int i;
	vecDouble filter;
	filter.resize(8);
	filter[0] = 0.230377813309;
	filter[1] = 0.714846570553;
	filter[2] = 0.630880767930;
	filter[3] = -0.027983769417;
	filter[4] = -0.187034811719;
	filter[5] = 0.030841381836;
	filter[6] = 0.032883011667;
	filter[7] = -0.010597401785;
	filterLength = filter.size();
	for(i = 0; i < filterLength; i++) lowFilter[i] = filter[filterLength-i-1];
	for(i = 0; i < filterLength; i++) highFilter[i] = filter[i];
	for(i = 0; i < filterLength; i += 2) highFilter[i] = -highFilter[i];
	}

/*
DWT procedure. Its input is the image.
The filter is fixed to be 8-tap Daubechies.
The matrices L and H store the intermediate
results of the DWT calculation - the
results of row-wise filtering. These
are then filtered column-wise to 
produce the four output matrices
LL, LH, HL and HH.

The procedure's signature is STL-oriented,
in sync with the rest of my code. However,
the inner workings favour arrays over vectors.
This is intentional and aims to improve
efficiency, as array access is somewhat
faster than vector access.
*/
public:
void oneLevelOf2DDWT(matrixDouble &image, matrixDouble &L, matrixDouble &H, matrixDouble &LL, matrixDouble &LH, matrixDouble &HL, matrixDouble &HH, bool downsample)
		{
		register int i, j;

		// Dimensions of the image.
		int N, M;
		N = image.size();
		M = image[0].size();

		// Specify dimensions of LL, LH, HL, HH to be NxM. Later we'll downsample, reducing each dimension by half.
		resizeMatrix(HH, N, M)
		resizeMatrix(HL, N, M)
		resizeMatrix(LH, N, M)
		resizeMatrix(LL, N, M)
		resizeMatrix(L, N, M)
		resizeMatrix(H, N, M)

		// Filter each row. Store the low-frequency output in LL, and the high-frequency output in HH.
		for(i = 0; i < N; i++)
			{
			extractRow(image, i)
			doFiltering();
			putRow(yLow, L, i)
			putRow(yHigh, H, i)
			}

		for(j = 0; j < M; j++)
			{
			extractColumn(L, j)
			doFiltering();
			putColumn(yLow, LL, j)
			putColumn(yHigh, LH, j)

			extractColumn(H, j)
			doFiltering();
			putColumn(yLow, HL, j)
			putColumn(yHigh, HH, j)
			}

		if(downsample)
			{
			// Now downsample rows by discarding every other.
			N /= 2;
			for(i = 0; i < N; i++)
				{
				LL[i] = LL[i+i];
				LH[i] = LH[i+i];
				HL[i] = HL[i+i];
				HH[i] = HH[i+i];
				}

			// Now downsample columns by discarding every other.
			M /= 2;
			for(i = 0; i < N; i++)
				for(j = 0; j < M; j++)
					{
					LL[i][j] = LL[i][j+j];
					LH[i][j] = LH[i][j+j];
					HL[i][j] = HL[i][j+j];
					HH[i][j] = HH[i][j+j];
					}

			// Now set dimensions of LL, LH, HL and HH to be (M/2)x(N/2).
			resizeMatrix(HH, N, M)
			resizeMatrix(HL, N, M)
			resizeMatrix(LH, N, M)
			resizeMatrix(LL, N, M)
    		}
	}





/*
A lazy recalculation of the DWT.
If the image is changed at (X, Y),
and L, H, LL, LH, HL, HH are the
DWT results prior to the change,
do the least amount of work to
update the results to reflect the
changed image.
*/
public:
void recalculateDWT(matrixDouble &image, matrixDouble &L, matrixDouble &H, matrixDouble &LL, matrixDouble &LH, matrixDouble &HL, matrixDouble &HH, int X, int Y)
	{
	int c, i, j;
	int N, M;
	N = image.size();
	M = image[0].size();

	extractRow(image, X)
	doFiltering();
	putRow(yLow, L, X)
	putRow(yHigh, H, X)

	for(j = Y - 7; j <= Y; j++)
		{
		if(j >= 0) c = j; else c = j + M;
		extractColumn(L, c)
		doFiltering();
		putColumn(yLow, LL, c)
		putColumn(yHigh, LH, c)

		extractColumn(H, c)
		doFiltering();
		putColumn(yLow, HL, c)
		putColumn(yHigh, HH, c)
		}
	}

// The filtering routine. 
private:
void doFiltering()
	{
	register int i, j, k;
	// Convolve.
	double sum0, sum1;
	for(i = 0; i < xLength; i++)
		{
		sum0 = sum1 = 0;
		for(j = 0; j < filterLength; j++)
			{
			// Note the periodicity extension.
			k = i + j; if(k >= xLength) k -= xLength;
			sum0 += lowFilter[filterLength - j - 1]*x[k];
			sum1 += highFilter[filterLength - j - 1]*x[k];
			}
		yLow[i] = sum0;
		yHigh[i] = sum1;
		}
	}
};

#undef putRow
#undef putColumn
#undef extractRow
#undef extractColumn
