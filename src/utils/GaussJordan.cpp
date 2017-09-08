
class GaussJordanCalculation
{
private:
matrixDouble A, B;
vecInt used;
vecInt rowsOrder;

double determinant;


vecInt where;

public:
GaussJordanCalculation() { }

matrixDouble inverse(matrixDouble &mx)
	{
	A.clear();
	B.clear();
	used.clear();
	rowsOrder.clear();

	int N;
	int i, j;
	N = mx.size();

	determinant = 1;
	A.resize(N);
	B.resize(N);
	used.resize(N);
	where.resize(N);

	for(i = 0; i < N; i++)
		{
		used[i] = -1;
		where[i] = i;
		A[i].resize(N);
		B[i].resize(N);
		}

	for(i = 0; i < N; i++)
		for(j = 0; j < N; j++)
			{
			A[i][j] = mx[i][j];
			B[i][j] = (i == j) ? 1 : 0;
			}

	for(j = 0; j < N; j++)
		{
		int rowNum = selectRow(j);
		double pivot = A[rowNum][j];

		determinant *= pivot;
		if(where[rowNum] != j) 
			{
			determinant *= (-1);

			for(i = 0; i < N; i++)
				if(where[i] == j)
					{ where[i] = where[rowNum]; break; }
			where[rowNum] = j;
			}



		if(pivot == 0) { printf("Uninvertible.\n"); exit(0); }

		for(i = 0; i < N; i++)
			if(used[i] == -1)
				{
				double value = A[i][j];
				double coefficient = value / pivot;
				subtractRow(coefficient, rowNum, i);
				}
		}



	for(i = 0; i < N; i++) used[i] = -1;

	for(j = N-1; j >= 0; j--)
		{
		int rowNum = rowsOrder[j];
		double pivot = A[rowNum][j];

		used[rowNum] = 1;

		divideThrough(rowNum, pivot);
		pivot = 1;
		for(i = 0; i < N; i++)
			if(used[i] == -1)
				{
				double value = A[i][j];
				double coefficient = value / pivot;
				subtractRow(coefficient, rowNum, i);
				}
		}
	
	matrixDouble C;
	C.resize(N);
	for(i = 0; i < N; i++) C[i].resize(N);

	for(j = N - 1; j >= 0; j--)
		{
		i = rowsOrder[j];
		for(int k = 0; k < N; k++)
			C[j][k] = B[i][k];
		}
	A.clear();
	B.clear();

	return C;
	}


double det(matrixDouble &mx)
	{
	matrixDouble inv = inverse(mx);
	return determinant;
	}

double trace(matrixDouble &mx)
	{
	int i, N;
	double ans = 0;
	N = mx.size();
	for(i = 0; i < N; i++)
		ans += mx[i][i];
	return ans;
	} 

matrixDouble eye(int N)
	{
	int i, j;
	matrixDouble ans;
	resizeMatrix(ans, N, N);
	for(i = 0; i < N; i++)
		for(j = 0; j < N; j++)
			if(i == j) ans[i][j] = 1;
			else ans[i][j] = 0;
	return ans;
	}

private:

void divideThrough(int rowInd, double d)
	{
	int N = A.size();
	int j;
	for(j = 0; j < N; j++)
		{
		A[rowInd][j] /= d;
		B[rowInd][j] /= d;
		}
	}

void print()
	{
	int i, j;
	int N = A.size();
	for(i = 0; i < N; i++)
		{
		for(j = 0; j < N; j++)
			printf("%.2lf  ", A[i][j]);
		printf("|  ");
		for(j = 0; j < N; j++)
			printf("%.2lf  ", B[i][j]);
		printf("\n");
		}
	}



int selectRow(int j)
	{
	double largestValue;
	int bestInd = -1;
	int N = A.size();
	int i;

	for(i = 0; i < N; i++)
		if(used[i] == -1)
			{
			if(bestInd == -1) { largestValue = ABS(A[i][j]); bestInd = i; }
			else if(ABS(A[i][j]) > largestValue) { largestValue = ABS(A[i][j]); bestInd = i; }
			}
	used[bestInd] = j;
	rowsOrder.push_back(bestInd);
	return bestInd;
	}

void subtractRow(double coeff, int from, int to)
	{
	int j;
	int N = A.size();

	for(j = 0; j < N; j++)
		{
		A[to][j] -= coeff*A[from][j];
		B[to][j] -= coeff*B[from][j];
		}
	}


};

