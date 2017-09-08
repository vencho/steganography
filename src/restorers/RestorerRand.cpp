// RestorerRand.cpp
// The restorer for the Random algorithm.

class RestorerRand : public Restorer
{
private:
int LOTS, CHANGES;
int queryLimit;

public:
RestorerRand(
			string oFile, 
			string sFile, 
			string encodingLog, 
			int lots, 
			int changes, 
			int queries, 
			int qstep,
			string distanceFile,
			string target) : Restorer(oFile, sFile, encodingLog, qstep, distanceFile, target)
	{
	LOTS = lots;
	CHANGES = changes;
	queryLimit = queries;
	}

void apply()
	{
	int i, j;
	int iterationsWithoutReduction = 0;
	int N = W.getN();
	int M = W.getM();

	double hypDist, bestDist;
	vector<Change> groupChange, bestGroupChange;
	vecDouble curWAM, hypWAM;

	while(W.getQueriesUsed() < queryLimit)
		{
		curWAM = W.getWAM();
		bestDist = Distance(goal, curWAM);
		bestGroupChange.clear();

		ECHO("%.12lf %d\n", bestDist, W.getQueriesUsed());

		if(iterationsWithoutReduction == 100)
			{
			iterationsWithoutReduction = 0;
			CHANGES = MAX(1, (CHANGES*3)/4);
			}

		for(i = 0; i < LOTS; i++)         // Randomly choose LOTS groups of pixels to change.
			{
			// Each group has CHANGES pixels in it. Each is perturbed by
			// 1, 0 or -1. Effectively, the group has upto CHANGES perturbations,
			// each of value 1 or -1.
			groupChange = randomNodupGroupChange(CHANGES, -1, 1);

			// For large values of CHANGES it is virtually 
			// impossible for the group change to contain all zeroes.
			// However, for a small value of CHANGES, it is worth
			// checking whether the group change is void. If so, reselect.

			if(CHANGES <= 10)
				{
				for(j = 0; j < groupChange.size(); j++)
					if(groupChange[j].v != 0)
						break;
				if(j == groupChange.size())
					{ i--; continue; }
				}

			// Change is not void. Query the oracle for its effects.
			// If better than best, record it.
			hypWAM = W.QueryGroupChange(groupChange);
			hypDist = Distance(goal, hypWAM);
			if(hypDist < bestDist)
				{
				bestDist = hypDist;
				bestGroupChange = groupChange;
				}

			}

		if(bestGroupChange.empty())
			{ iterationsWithoutReduction++; }
		else
			{
			iterationsWithoutReduction = 0;
			W.makeGroupChange(bestGroupChange);
			for(i = 0; i < bestGroupChange.size(); i++)
				changesMade[bestGroupChange[i].x][bestGroupChange[i].y] += bestGroupChange[i].v;
			}
		}

	curWAM = W.getWAM();
	bestDist = Distance(goal, curWAM);
	ECHO("%.12lf %d\n", bestDist, W.getQueriesUsed());
	W.outputImage("restored.pgm");
	}

};


