// RestorerGreedy.cpp
// The restorer class for the Greedy algorithm.

class RestorerGreedy : public Restorer
{
private:
int queryLimit;

public:
RestorerGreedy(
				string oFile, 
				string sFile, 
				string encodingLog, 
				int QUERIES, 
				int qstep,
				string distanceFile,
				string target) : Restorer(oFile, sFile, encodingLog, qstep, distanceFile, target)
	{
	queryLimit = QUERIES;
	}

void apply()
	{
	// Dimensions of the image.
	int N, M;

	// Number of pixels available for restoration.
	int m;

	// Temporary variables used below.
	int x, y, pixelValue, v;
	double hypDistPlus, hypDistMinus, bestHypDist, curDist;
	vecDouble hypWAMplus, hypWAMminus, bestHypWAM, curWAM;
	int i, j;

	// Dimensions.
	N = W.getN();
	M = W.getM();

	// WAM features of the image and their distance to the goal features.
	curWAM = W.getWAM();
	curDist = Distance(curWAM, goal);

	// Create a list of the pixels available for restoration.
	vector<Pixel> availablePixels;
	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
			if(!usedPixels[i][j])
				availablePixels.push_back(newPixel(i, j));
	m = availablePixels.size();
	availablePixels.resize(m);

	// Iterate while there are queries available.
	while(W.getQueriesUsed() < queryLimit)
		for(i = 0; i < m && W.getQueriesUsed() < queryLimit; i++)
			{
			ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());
			// For each non-payload pixel (x, y),
			x = availablePixels[i].x;
			y = availablePixels[i].y;
			pixelValue = W.getPixelValueAt(x, y);

			// Calculate the WAM of the image obtained by perturbing the pixel by 1. 
			// Avoid possibility of incrementing pixel outside 8-bit range.
			if(pixelValue != 255 && ABS(changesMade[x][y] + 1) <= PIXEL_CHANGE_CAP) 
				{
				hypWAMplus = W.QueryOneChange(newChange(x, y, 1));
				hypDistPlus = Distance(hypWAMplus, goal);
				}
			else hypDistPlus = 1000000;
		
			// Same for -1.
			// Calculate the hypothetical distances of the two possibilities.
			if(pixelValue != 0 && ABS(changesMade[x][y] - 1) <= PIXEL_CHANGE_CAP) 
				{
				hypWAMminus = W.QueryOneChange(newChange(x, y, -1));
				hypDistMinus = Distance(hypWAMminus, goal);
				}
			else hypDistMinus = 1000000;
			
			// Determine which of these two changes leads to a greater distance reduction.
			if(hypDistPlus < hypDistMinus) 
				{
				bestHypDist = hypDistPlus; 
				bestHypWAM = hypWAMplus;
				v = 1;
				}
			else
				{
				bestHypDist = hypDistMinus;
				bestHypWAM = hypWAMminus;
				v = -1;
				}

			// Finally, check if this distance reduction is positive.
			// If so, perform the change.
			if(bestHypDist < curDist)
				{
				W.makeOneChange(newChange(x, y, v));
				changesMade[x][y] += v;
				curDist = bestHypDist;
				curWAM = bestHypWAM;
				}
			}
		ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());
		W.outputImage("restored.pgm");
		}

};


