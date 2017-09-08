// Restorer.cpp
// A superclass for our restorers.
// It promises the apply() routine, which is implemented by subclasses.

class Restorer
{
protected:
WAMUpdater W;
vecDouble goal;
matrixInt usedPixels;
matrixInt changesMade;


string originalFile;
string stegoFile;

// A list of all the non-payload pixels available for restoration.
vector<Pixel> availablePixels;

public:

virtual void apply() = 0;

Restorer(string oFile, string sFile, string encodingLogFile, int queryStep, string distanceFile, string target)
	{
	char x[128];
	int i, j;
	FILE *fin = fopen(encodingLogFile.c_str(), "rb");

	originalFile = oFile;
	stegoFile = sFile;

	loadMatrix(fin, usedPixels);
	loadMatrix(fin, changesMade);

	W = WAMUpdater(stegoFile, queryStep);
	fclose(fin);

	prepareDistance(distanceFile);
	setGoal(target);

	int N, M; N = W.getN(); M = W.getM();
	/*
	Find all the non-payload pixels, and put them into a vector.
	This will allow quick generation of valid group changes.
	*/
	for(i = 0; i < N; i++) 
		for(j = 0; j < M; j++) 
			if(!usedPixels[i][j]) 
				availablePixels.push_back(newPixel(i, j));
	availablePixels.resize(availablePixels.size());
	}


/*
Routine to set the goal. The choice is between
aiming for the original features, and aiming for
the mean features. We are going to use the mean
features.
*/
void setGoal(string instruction)
	{
	if(instruction == "mean")
		{ goal = means; }
	else if(instruction == "original")
		{
		WAMUpdater F = WAMUpdater(originalFile, 0);
		goal = F.getWAM();
		}
	else
		{ exit(0); }
	}

// RANGE: [a..b)
int randomNumberInRange(int a, int b)
	{ return a + (rand() % (b-a)); }

protected:
// Returns a combination of k elements,
// drawn from [0..m), where m is the number
// of available pixels.
vecInt randomSample(int k)
{
int i, ind;
bool hasDups;
int m = availablePixels.size();

vecInt combination;
combination.resize(k);

do
	{
	for(i = 0; i < k; i++)
		{
		ind = randomNumberInRange(0, m);
		combination[i] = ind;
		}
	sort(combination.begin(), combination.end());
	hasDups = false;
	for(i = 1; i < k && !hasDups; i++)
		if(combination[i] == combination[i-1]) hasDups = true;
	} while(hasDups);


return combination;
}


vector<Change> randomNodupGroupChange(int size, int vmin, int vmax)
{
int i, ind, v, x, y;
bool hasDups;
vector<Change> groupChange;
vecInt indices;

groupChange.resize(size);
indices.resize(size);

vmax += 1;

do
	{
	indices = randomSample(size);
	for(i = 0; i < size; i++)
		{
		ind = indices[i];
		x = availablePixels[ind].x;
		y = availablePixels[ind].y;
		do 
			{ v = randomNumberInRange(vmin, vmax); } 
		while(ABS(changesMade[x][y] + v) > PIXEL_CHANGE_CAP);

		groupChange[i] = newChange(x, y, v);
		}
	} while(!W.canMakeGroupChange(groupChange));

return groupChange;
}

};


