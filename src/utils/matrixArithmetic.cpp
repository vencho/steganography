

/* -------------------------------------------------------- */
/* VECTOR OPERATORS. */

vecDouble operator +(vecDouble &u, vecDouble &v)
{
int i;
vecDouble ans;
ans.resize(v.size());
for(i = 0; i < v.size(); i++)
	ans[i] = u[i] + v[i];
return ans;
}

vecDouble operator -(vecDouble &u, vecDouble &v)
{
int i;
vecDouble ans;
ans.resize(v.size());
for(i = 0; i < v.size(); i++)
	ans[i] = u[i] - v[i];
return ans;
}


double operator *(vecDouble &u, vecDouble &v)
{
int i;
double ans = 0;
for(i = 0; i < u.size(); i++)
	ans += u[i]*v[i];
return ans;
}


matrixDouble transpose(matrixDouble &A)
{
int N, M;
int i, j;
matrixDouble B;

N = A.size();
M = A[0].size();

resizeMatrix(B, M, N);

for(i = 0; i < N; i++)
	for(j = 0; j < M; j++)
		B[j][i] = A[i][j];
return B;
}




matrixDouble operator *(matrixDouble &A, matrixDouble &B)
{
int i, j, k;
int N, K, M;

matrixDouble C;

N = A.size();
K = A[0].size();
M = B[0].size();

resizeMatrix(C, N, M);

for(i = 0; i < N; i++)
	for(j = 0; j < M; j++)
		{
		double x = 0;
		for(k = 0; k < K; k++)
			x += A[i][k]*B[k][j];
		C[i][j] = x;
		}

return C;
}


matrixDouble operator *(double k, matrixDouble &A)
{
int N, M;
int i, j;
matrixDouble B;

N = A.size();
M = A[0].size();

resizeMatrix(B, N, M);

for(i = 0; i < N; i++)
	for(j = 0; j < M; j++)
		{
		B[i][j] = k * A[i][j];
		}

return B;
}




vecDouble operator *(matrixDouble &A, vecDouble &v)
{
vecDouble ans;
int N, M, i, j;

N = A.size(); M = A[0].size();
ans.resize(N);

for(i = 0; i < N; i++)
	{
	double x = 0;
	for(j = 0; j < M; j++)
		x += A[i][j]*v[j];
	ans[i] = x;
	}

return ans;
}


matrixDouble operator +(matrixDouble &A, matrixDouble &B)
{
matrixDouble C;
int N, M;
int i, j;
N = A.size(); M = A[0].size();

resizeMatrix(C, N, M);

for(i = 0; i < N; i++)
	for(j = 0; j < M; j++)
		C[i][j] = A[i][j] + B[i][j];
return C;
}

/* -------------------------------------------------------- */
