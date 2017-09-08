// RestorerIneff.cpp
// The restorer for the inefficient algorithm.

class RestorerIneff : public Restorer
{
private:
int iterations;

public:
RestorerIneff(
			string oFile, 
			string sFile, 
			string encodingLog, 
			int ITERATIONS, 
			int qstep,
			string distanceFile,
			string target) : Restorer(oFile, sFile, encodingLog, qstep, distanceFile, target)
	{
	iterations = ITERATIONS;
	}


void apply()
	{
	int N, M;
	int it;
	int i, j, v;
	Change C, bestChange;
	double dist, bestdist;

	N = W.getN();
	M = W.getM();

	vecDouble cur = W.getWAM();
	bestdist = Distance(goal, cur);
	for(it = 1; it <= iterations; it++)
		{
		ECHO("%d %.12lf\n", W.getQueriesUsed(), bestdist);

		bestChange = newChange(-1, -1, -1);
		for(i = 0; i < N; i++)
			for(j = 0; j < M; j++)
				if(!usedPixels[i][j])
					for(v = -1; v <= 1; v += 2) // v in {-1, 1}
						{
						C = newChange(i, j, v);

						if(W.canMakeOneChange(C) && ABS(changesMade[i][j] + v) <= PIXEL_CHANGE_CAP)
							{
							vecDouble w = W.QueryOneChange(C);
							dist = Distance(w, goal);
							if(dist < bestdist)
								{ bestChange = C; bestdist = dist; }
							}
						}
		if(bestChange.x == -1) break;
		else 
			{
			W.makeOneChange(bestChange);
			changesMade[bestChange.x][bestChange.y] += bestChange.v;
			}

		cur = W.getWAM();
		bestdist = Distance(goal, cur);
		}
	ECHO("%d %.12lf\n", W.getQueriesUsed(), bestdist);
	W.outputImage("restored.pgm");
	}
};



