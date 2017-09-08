// RestorerVarianceSort.cpp
// The restorer class for our variance sort algorithm.

struct PixelWithVariance
	{
	int x, y;
	double variance;
	};
PixelWithVariance newPixelWithVariance(int x, int y, double var)
	{
	PixelWithVariance ans;
	ans.x = x;
	ans.y = y;
	ans.variance = var;
	return ans;
	}
bool operator <(PixelWithVariance p1, PixelWithVariance p2)
	{ return (p1.variance < p2.variance); }



class RestorerVarianceSort : public Restorer
{
private:
int queryLimit;
matrixDouble variance;
vector<PixelWithVariance> availablePixels;


public:
RestorerVarianceSort(
					string oFile, 
					string sFile, 
					string encodingLog, 
					int queries, 
					int qstep,
					string distanceFile,
					string target) : Restorer(oFile, sFile, encodingLog, qstep, distanceFile, target)
	{ queryLimit = queries; }


void apply()
	{
	estimateVariances();
	sortPixelsByVariance();
	performRestoration();
	W.outputImage("restored.pgm");
	}

private:
void estimateVariances()
	{
	/*
	We estimate the variance around each (i, j)
	in the spatial domain. The estimation takes
	all the pixel values in the 5x5 square centred
	at (i, j) and uses the unbiased variance 
	estimation on them.
	*/

	int N, M;
	int i, j, x, y;

	N = W.getN();
	M = W.getM();


	resizeMatrix(variance, N, M);
	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
			{
			// Coordinates of upper-left corner.
			int ulx = MAX(0, i - 2); 
			int uly = MAX(0, j - 2);

			// Coordinates of lower-right corner.
			int lrx = MIN(N - 1, i + 2); 
			int lry = MIN(M - 1, j + 2);
			
			// Number of cells in square.
			int c = (lrx - ulx + 1)*(lry - uly + 1);
			
			// The mean in the 5x5 square around (i, j).
			int sum = 0;
			for(x = ulx; x <= lrx; x++)
				for(y = uly; y <= lry; y++)
					sum += W.getPixelValueAt(x, y);
			double avg = sum / (double) c;

			// The variance in the square.
			double var = 0;
			for(x = ulx; x <= lrx; x++)
				for(y = uly; y <= lry; y++)
					{
					int pixel = W.getPixelValueAt(x, y);
					var += ((double)pixel - avg)*((double)pixel - avg);
					}
			var /= (double) (c-1);
			variance[i][j] = var;
			}
	}

void sortPixelsByVariance()
	{
	int i, j;
	int N, M;

	N = W.getN();
	M = W.getM();

	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
			if(!usedPixels[i][j])
				{
				PixelWithVariance p = newPixelWithVariance(i, j, variance[i][j]);
				availablePixels.push_back(p);
				}
	sort(availablePixels.begin(), availablePixels.end());
	reverse(availablePixels.begin(), availablePixels.end());
	}

void performRestoration()
	{
	vecDouble curWAM, hypWAM;
	double curDist, hypDist;
	int i, x, y, v;
	int wraps = 0;


	while(W.getQueriesUsed() < queryLimit)
		{
		for(i = 0; i < availablePixels.size() && W.getQueriesUsed() < queryLimit; i++)
			{
			curWAM = W.getWAM();
			curDist = Distance(curWAM, goal);
			ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());

			// Pixel under consideration.
			x = availablePixels[i].x;
			y = availablePixels[i].y;
			

			// Is it better to perturb this pixel by 1 or -1?
			// A 0 means that both perturbations increase the distance.
			// Otherwise, the function returns the direction with the
			// greater distance reduction.
			v = bestDirectionToChange(x, y);


			// If you get a 0, ignore this pixel and move on.
			if(v == 0) continue;


			// Otherwise, perturb it in that direction for as long
			// it is beneficial to do so.
			Change c = newChange(x, y, v);
			do
				{
				// Make the change.
				W.makeOneChange(c);
				changesMade[c.x][c.y] += c.v;

				// What's the WAM and the distance now?
				curWAM = W.getWAM();
				curDist = Distance(curWAM, goal);
				ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());

				// What would the WAM and the distance be
				// if I were to make another change?
				if(W.canMakeOneChange(c) && ABS(changesMade[c.x][c.y] + c.v) <= PIXEL_CHANGE_CAP)
					{
					hypWAM = W.QueryOneChange(c);
					hypDist = Distance(hypWAM, goal);
					}
				else break;
				// If a further change would increase the distance, stop.
				} while(hypDist < curDist && W.getQueriesUsed() < queryLimit);
			}
		wraps++;
		}
	}

private:
int bestDirectionToChange(int x, int y)
	{
	Change cUP = newChange(x, y, 1);
	Change cDOWN = newChange(x, y, -1);
	bool up_good, down_good;
	vecDouble hypWAMup, hypWAMdown, curWAM;
	double hypDistUp, hypDistDown, curDist;

	curWAM = W.getWAM();
	curDist = Distance(curWAM, goal);

	if(!W.canMakeOneChange(cUP) || ABS(changesMade[x][y] + 1) > PIXEL_CHANGE_CAP) 
		{ up_good = false; }
	else
		{
		hypWAMup = W.QueryOneChange(cUP);
		hypDistUp = Distance(hypWAMup, goal);
		if(hypDistUp < curDist) { up_good = true; }
		else up_good = false;
		}

	if(!W.canMakeOneChange(cDOWN) || ABS(changesMade[x][y] - 1) > PIXEL_CHANGE_CAP) 
		{ down_good = false; }
	else
		{
		hypWAMdown = W.QueryOneChange(cDOWN);
		hypDistDown = Distance(hypWAMdown, goal);
		if(hypDistDown < curDist) { down_good = true; }
		else down_good = false;
		}
	if(!down_good && !up_good) { return 0; }
	else if(down_good && !up_good) { return -1; }
	else if(!down_good && up_good) { return 1; }
	else
		{
		if(hypDistUp < hypDistDown) { return 1; }
		else { return -1; }
		}
	}
};

