// RestorerQP.cpp
// The restorer for the quadratic programming algorithm.

class RestorerQP : public Restorer
{
private:
int queryLimit;
int COUNT;

vector<Change> changes;

matrixDouble A;
matrixDouble V;
vecDouble b;

public:
RestorerQP(
			string oFile, 
			string sFile, 
			string encodingLog, 
			int queries, 
			int qstep,
			string distanceFile,
			string target) : Restorer(oFile, sFile, encodingLog, qstep, distanceFile, target)
	{
	queryLimit = queries;
	COUNT = 2000;
	}

void apply()
	{
	clock();
	srand(time(0));

	vecDouble curWAM = W.getWAM();
	double curDist = Distance(curWAM, goal);
	ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());


	while(W.getQueriesUsed() < queryLimit)
		{
		if(W.getQueriesUsed() >= 10000) COUNT = 1000;
		if(W.getQueriesUsed() >= 30000) COUNT = 500;
		if(W.getQueriesUsed() >= 40000) COUNT = 50;

		pickChanges();
		calculateModel();
		writeMatlabInput();
		signalMatlab();
		idleUntilMatlabIsReady();
		performChanges();
		}

	W.outputImage("restored.pgm");
	}

void pickChanges()
	{
	int i, x, y, v;

	random_shuffle(availablePixels.begin(), availablePixels.end());
	changes.clear();
	for(i = 0; i < availablePixels.size() && changes.size() < COUNT && changes.size() + W.getQueriesUsed() < queryLimit; i++)
		{
		x = availablePixels[i].x;
		y = availablePixels[i].y;
		if(ABS(changesMade[x][y]) == PIXEL_CHANGE_CAP) continue;
		Change c = newChange(x, y, -1);
		changes.push_back(c);
		c = newChange(x, y, 1);
		changes.push_back(c);
		}
	sort(changes.begin(), changes.end());
	}

void calculateModel()
	{
	int i, j;
	vecDouble f, v, delta;

	int n = changes.size();

	A.clear();
	V.clear();
	b.clear();


	f = W.getWAM();

	V.resize(27);
	for(i = 0; i < 27; i++) V[i].resize(n);

	for(i = 0; i < n; i++)
		{
		Change c = changes[i];
		v = W.QueryOneChange(c);
		delta = v - f;
		for(j = 0; j < 27; j++)
			V[j][i] = delta[j];
		}

	// A = V'*covInv*V;
	matrixDouble Vprimed = transpose(V);
	matrixDouble a = Vprimed*covInv;
	A = a*V;

	// b = 2*V'*covInv*(f - means);
	vecDouble c = f - means;
	for(i = 0; i < 27; i++) c[i] *= 2;
	b = a*c;
	}

void writeMatlabInput()
	{
	int i, j;
	int n = changes.size();

	// Describe the quadratic objective.
	// Note that Matlab's input is 1/2x'Hx, so
	// I need to double my A before putting it in.
	FILE *fout = fopen("H.dat", "wb");
	for(i = 0; i < n; i++)
		{
		for(j = 0; j < n; j++)
			FECHO(fout, "%.12lf ", 2*A[i][j]);
		FECHO(fout, "\n");
		}
	fclose(fout);


	// Describe the linear objective.
	// Matlab's f is my b.
	fout = fopen("f.dat", "wb");
	for(i = 0; i < n; i++)
		FECHO(fout, "%.12lf ", b[i]);
		FECHO(fout, "\n");
	fclose(fout);

	// Linear constraint matrix.
	// A constraint for each i, i + 1.
/*
	fout = fopen("A.dat", "wb");
	for(i = 0; i < n; i += 2)
		{
		for(j = 0; j < n; j++)
			if(j == i || j == i + 1)
				FECHO(fout, "1 ");
			else
				FECHO(fout, "0 ");
		FECHO(fout, "\n");
		}
	fclose(fout);
*/

	// Right-hand side of the linear constraints.
/*
	fout = fopen("b.dat", "wb");
	for(i = 0; i < n; i += 2)
			FECHO(fout, "1 ");
	FECHO(fout, "\n");
	fclose(fout);
*/
	}

void signalMatlab()
	{
	FILE *fout;
	fout = fopen("signalToMatlab", "wb");
	FECHO(fout, "112233\n"); 
	fclose(fout);
	}

void idleUntilMatlabIsReady()
	{
	int x;
	FILE *fin;
	while(1)
		{
		fin = fopen("signalFromMatlab", "rb");
		if(fin != NULL)
			{
			fscanf(fin, "%d", &x);
			if(x == 998877) break;
			}
		}
	fclose(fin);
	remove("signalFromMatlab");
	}

void performChanges()
	{
	int n = changes.size();
	int i;

	vector<Change> gc;
	double x;

	FILE *fin = fopen("matlabOutput", "rb");
	for(i = 0; i < n; i++)
		{
		fscanf(fin, "%lf", &x);
		if(x >= 0.5) { gc.push_back(changes[i]); }
		}
	fclose(fin);

	W.makeGroupChange(gc);
	vecDouble curWAM = W.getWAM();
	double curDist = Distance(curWAM, goal);

	for(i = 0; i < gc.size(); i++)
		{
		int pixelx = gc[i].x;
		int pixely = gc[i].y;
		int changev = gc[i].v;
		changesMade[pixelx][pixely] += changev;
		}
	ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());
	}
};

