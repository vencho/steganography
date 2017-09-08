// WAMCalculation.cpp.
// A stateless class capturing the calculation of the WAM features.

class WAMCalculation
{
private:
matrixDouble prefixedSquares;

public:
WAMCalculation() { }

vecDouble calculateWAM(
	matrixDouble &image,
	matrixDouble &L,
	matrixDouble &H,
	matrixDouble &LL,
	matrixDouble &LH,
	matrixDouble &HL,
	matrixDouble &HH,
	matrixDouble &variancesH,
	matrixDouble &variancesV,
	matrixDouble &variancesD,
	matrixDouble &residualsH,
	matrixDouble &residualsV,
	matrixDouble &residualsD,
	vecDouble &momentsH,
	vecDouble &momentsV,
	vecDouble &momentsD
		)
	{
	vecDouble features;

	// Dimensions of the image.
	int N = image.size(); 
	int M = image[0].size();

	// Calculate the DWT.
	DWTCalculation DWT = DWTCalculation();
	DWT.oneLevelOf2DDWT(image, L, H, LL, LH, HL, HH, false);

	// Calculate the 9 moments from H.
    computeVariances(LH, variancesH);
    computeResiduals(LH, variancesH, residualsH);
    computeMoments(residualsH, momentsH);

	// Calculate the 9 moments from V.
    computeVariances(HL, variancesV);
    computeResiduals(HL, variancesV, residualsV);
    computeMoments(residualsV, momentsV);

	// Calculate the 9 moments from D.
    computeVariances(HH, variancesD);
    computeResiduals(HH, variancesD, residualsD);
	computeMoments(residualsD, momentsD);

	// Assemble the 27-d feature vector.
	features.insert(features.end(), momentsH.begin(), momentsH.end());
	features.insert(features.end(), momentsV.begin(), momentsV.end());
	features.insert(features.end(), momentsD.begin(), momentsD.end());

	// Return it.
	return features;
	}

/*
The calculation of local variances. 
Given a matrix of dwt coefficients,
estimate the local variance of the
submatrix (fromX, fromY) - (toX, toY).
*/
void computeVariances(matrixDouble &dwt, matrixDouble &var, int fromX, int toX, int fromY, int toY)
    {
	int N = dwt.size();
	int M = dwt[0].size();
	int i, j, r, c;
    for(i = fromX; i < toX; i++)
        for(j = fromY; j < toY; j++)
			{
			r = (i + N) % N;
			c = (j + M) % M;
			var[r][c] = computeOneVariance(r, c);
			}
    }
// Routine to compute the variances of the whole dwt matrix.
void computeVariances(matrixDouble &dwt, matrixDouble &var)
	{
	int N = dwt.size();
	int M = dwt[0].size();
	resizeMatrix(var, N, M);
    transform(dwt);
	computeVariances(dwt, var, 0, N, 0, M);
	}	

/*
Each coefficient's local variance is 
estimated separately, based on four
windows centred around it - of sizes 
3, 5, 7 and 9. 
*/
double computeOneVariance(int x, int y)
	{
	double v[6] = {0, 0, 0, 0, 0};
	int d;
	int X, Y, F, G;
	int N = prefixedSquares.size();
	int M = prefixedSquares[0].size();
	v[1] = v[2] = v[3] = v[4] = 0;

	// For each window size 2*d+1:
	for(d = 1; d <= 4; d++)
		{
		// The window is (X, Y) - (F, G).
		X = x - d;
		Y = y - d;
		F = x + d;
		G = y + d;

		// Crop window at edges.
		if(X < 0) X = 0;
		if(Y < 0) Y = 0;
		if(F >= N) F = N-1;
		if(G >= M) G = M-1;

		// Use prefixedSquares to get the numerator.
		if(!X && !Y) v[d] = prefixedSquares[F][G];
		else if(!X) v[d] = prefixedSquares[F][G] - prefixedSquares[F][Y-1];
		else if(!Y) v[d] = prefixedSquares[F][G] - prefixedSquares[X-1][G];
		else v[d] = prefixedSquares[F][G] - prefixedSquares[F][Y-1] - prefixedSquares[X-1][G] + prefixedSquares[X-1][Y-1];

		#ifdef WAM_fix_denominator
		// Use a fixed denominator. We used this.
		v[d] /= (2*d + 1)*(2*d + 1);
		#else
		// Use a non-fixed denominator.
		v[d] /= (F-X+1)*(G-Y+1);
		#endif
		}

	// Find the least v[d].
	double best = v[1];
	for(d = 2; d <= 4; d++) best = MIN(best, v[d]);

	// Subtract a half, clamp to 0.
	best -= 0.5;
	if(best < 0) best = 0;

	// Return.
	return best;
	}

/*
The calculation of all residuals, given
the dwt and variance matrices. For a
coefficient x with local variance
estimate v, the residual is 0.5*x / (0.5+v).
*/
void computeResiduals(matrixDouble &dwt, matrixDouble &var, matrixDouble &res)
	{
	int N = dwt.size();
	int M = dwt[0].size();
	resizeMatrix(res, N, M);
	computeResiduals(dwt, var, res, 0, N, 0, M);
	}

/* 
The calculation of only some residuals.
Those appearing in the window (fromX, fromY) - (toX, toY).
*/
void computeResiduals(matrixDouble &dwt, matrixDouble &var, matrixDouble &res, int fromX, int toX, int fromY, int toY)
	{
    int i, j, r, c;
	int N = dwt.size();
	int M = dwt[0].size();
	for(i = fromX; i < toX; i++)
		for(j = fromY; j < toY; j++)
			{
			r = (i + N) % N;
			c = (j + M) % M;
			res[r][c] = 0.5*dwt[r][c] / (0.5 + var[r][c]);
			}
	}

/*
The calculation of moments, given the
residuals. The m-th moment is defined by
(sum(i, j) of |residuals[i][j] - average|^m) / (M*N),
where average is the mean value of the matrix residuals.
We're interested in the first nine moments.
*/
void computeMoments(matrixDouble &res, vecDouble &moments)
	{
	int N = res.size();
	int M = res[0].size();
	int i, j, m;
	double average = 0;

	// Initialise.
	moments.resize(9);
	for(m = 0; m < 9; m++) moments[m] = 0;

	// Find average residual.
	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
			average += res[i][j];
	average /= (N*M);

	// Do the moments.
	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
			{
			double R = res[i][j] - average;
			R = ABS(R);

			double v = R;
			for(m = 0; m < 9; m++)
				{
				moments[m] += v;
				v *= R;
				}
			}
	for(m = 0; m < 9; m++) moments[m] /= (M*N);
	}

/*
Routine which computes the
prefixed squares form of mx and
puts it into prefixedSquares.
Useful for variances.
*/
void transform(matrixDouble &mx)
    {
    int i, j;
	int N = mx.size();
	int M = mx[0].size();
	resizeMatrix(prefixedSquares, N, M);

    for(i = 0; i < N; i++)
        for(j = 0; j < M; j++)
            prefixedSquares[i][j] = mx[i][j]*mx[i][j];

    for(i = 0; i < N; i++)
        for(j = 1; j < M; j++)
            prefixedSquares[i][j] += prefixedSquares[i][j-1];

    for(j = 0; j < M; j++)
        for(i = 1; i < N; i++)
            prefixedSquares[i][j] += prefixedSquares[i-1][j];
    }
};
