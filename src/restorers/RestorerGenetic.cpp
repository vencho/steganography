// RestorerGenetic.cpp
// The restorer for our Genetic algorithm

struct PopulationIndividual
	{
	GroupChange vc;
	double distanceReduction;
	};

bool operator <(PopulationIndividual a, PopulationIndividual b)
	{
	if(a.distanceReduction < b.distanceReduction) return true;
	else return false;
	}

PopulationIndividual newIndividual(GroupChange gc, double distRed)
	{
	PopulationIndividual ans;
	ans.vc = gc;
	ans.distanceReduction = distRed;
	return ans;
	}

typedef PopulationIndividual Individual;
typedef priority_queue<PopulationIndividual> Population;








GroupChange mergeTwoGroupChanges(GroupChange &u, GroupChange &v)
	{
	GroupChange ans;
	ans.clear();

	sort(u.begin(), u.end());
	sort(v.begin(), v.end());
	int i;

	vector<Change> :: iterator a, b;
	a = u.begin();
	b = v.begin();
	while(1)
		{
		#define ADVANCE(t) { ans.push_back(*(t)); (t)++; }
		if(a == u.end() && b == v.end()) break;
		else if(a == u.end() && b != v.end()) ADVANCE(b)
		else if(a != u.end() && b == v.end()) ADVANCE(a)
		else
			{
			pair<int, int> pixela, pixelb;
			pixela = make_pair((*a).x, (*a).y);
			pixelb = make_pair((*b).x, (*b).y);
			if(pixela < pixelb) ADVANCE(a)
			else if(pixela > pixelb) ADVANCE(b)
			else
				{
				ans.push_back(newChange((*a).x, (*a).y, (*a).v + (*b).v));
				a++;
				b++;
				}
			}
		#undef ADVANCE
		}
	return ans;
	}




class RestorerGenetic : public Restorer
{
private:
// The population for the genetic algorithm.
Population P;

// Parameters of the genetic algorithm.
int initialSize;

// What it says on the tin.
int queryLimit;

public:
// Constructor.
RestorerGenetic(string oFile, 
				string sFile, 
				string encodingLogFile, 
				int initSize, 
				int queries, 
				int qStep, 
				string distanceFile,
				string target) : Restorer(oFile, sFile, encodingLogFile, qStep, distanceFile, target)
	{
	// Set state space of the genetic algorithm restorer.
	initialSize = initSize;
	queryLimit = queries;
	}

void apply()
	{
	initialiseGeneticAlgorithm();
	iterate();
	dwindle();
	W.outputImage("restored.pgm");
	}

private:
void initialiseGeneticAlgorithm()
	{
	int i, j;
	int N, M;
	N = W.getN(); M = W.getM();


	vecDouble curWAM;
	double curDist;
	curWAM = W.getWAM();
	curDist = Distance(curWAM, goal);
	ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());

	for(i = 0; i < initialSize; i++)
		migrateIn();
	}

void iterate()
	{
	vecDouble curWAM;
	double curDist;

	curWAM = W.getWAM();
	curDist = Distance(curWAM, goal);
	ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());

	while(1)
		{
		if(W.getQueriesUsed() >= queryLimit) break;
		migrateIn();
		if(W.getQueriesUsed() >= queryLimit) break;
		migrateIn();
		if(W.getQueriesUsed() >= queryLimit) break;

		migrateOut();
		curWAM = W.getWAM();
		curDist = Distance(curWAM, goal);
		ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());

		if(P.size() == 2*initialSize + initialSize / 10) merge();
		}
	}

void migrateIn()
	{
	vector<Change> groupChange;
	vecDouble hypWAM, curWAM;
	double curDist, hypDist, distRed;

	curWAM = W.getWAM();
	curDist = Distance(curWAM, goal);
	while(1)
		{
		if(W.getQueriesUsed() >= queryLimit) return;

		do
			{ groupChange = randomNodupGroupChange(1, -1, 1); }
		while(groupChange[0].v == 0);

		hypWAM = W.QueryGroupChange(groupChange);

		hypDist = Distance(hypWAM, goal);
		distRed = curDist - hypDist;
		if(distRed > 0.000001) break;
		}
	P.push(newIndividual(groupChange, distRed));
	}

void migrateOut()
	{
	int i;
	int x, y, v;
	Individual best;

	// Extract the top element from P.
	best = P.top();
	P.pop();

	// Check that applying it does not violate
	// the pixel change cap.
	for(i = 0; i < best.vc.size(); i++)
		{
		x = best.vc[i].x;
		y = best.vc[i].y;
		v = best.vc[i].v;
		// If it does, break early.
		if(ABS(changesMade[x][y] + v) > PIXEL_CHANGE_CAP) break; 
		// If it does not, finish normally.
		}
	// If haven't finished normally, then applying best
	// violates pixel change cap. Discard it.
	if(i != best.vc.size()) return;

	// Check that applying best will not take a pixel outside [0..255].
	// Also, that the distance reduction label is positive.
	if(W.canMakeGroupChange(best.vc) && best.distanceReduction > 0)
		{
		// If so, make the change.
		W.makeGroupChange(best.vc);
		// Record that pixels have been touched.
		for(i = 0; i < best.vc.size(); i++)
			changesMade[best.vc[i].x][best.vc[i].y] += best.vc[i].v;
		}

	}

void merge()
	{
	Individual first, second;
	Individual merged;
	vecDouble hypWAM, curWAM;
	double curDist, hypDist, distRed;
	int i;

	vector<Individual> mergeResults;
	for(i = 0; i < initialSize; i++)
		{
		first = P.top();
		P.pop();
		second = P.top();
		P.pop();

		if(W.getQueriesUsed() < queryLimit)
			{
			merged.vc = mergeTwoGroupChanges(first.vc, second.vc);

			hypWAM = W.QueryGroupChange(merged.vc);
			curWAM = W.getWAM();
			curDist = Distance(curWAM, goal);
			hypDist = Distance(hypWAM, goal);
			merged.distanceReduction = curDist - hypDist;
			mergeResults.push_back(merged);
			}
		else
			{
			mergeResults.push_back(first);
			mergeResults.push_back(second);
			}
		}

	while(!P.empty()) P.pop();

	for(i = 0; i < mergeResults.size(); i++)
		P.push(mergeResults[i]);
	}

void dwindle()
	{
	while(!P.empty())
		migrateOut();

	vecDouble curWAM = W.getWAM();
	double curDist = Distance(curWAM, goal);
	ECHO("%.12lf %d\n", curDist, W.getQueriesUsed());
	}


};

