// WAMUpdater.cpp
/* 
This is the most important class of the WAM pipeline.
It holds an image, and all the information from its
WAM calculation. It provides an interface to modify
the image whilst keeping the WAM information in
sync with it. It also offers an interface for
hypothetical queries: "I don't want to change
this image in this way yet, but if I did, what 
would the effect be?"
*/



class WAMUpdater
{

private:
// The image.
matrixDouble image;
// The results from the DWT calculation.
matrixDouble L, H, LL, LH, HL, HH;
// The variance matrices.
matrixDouble variancesH, variancesV, variancesD;
// The residuals matrices.
matrixDouble residualsH, residualsV, residualsD;
// The three sets of moments.
vecDouble momentsH, momentsV, momentsD;
// A counter of many queries have been answered so far.
int queriesUsed, queryTarget, queryStep;
// References to the two above classes.
WAMCalculation W;
DWTCalculation DWT;


public:
// Obvious access methods.
int getN() { return image.size(); } 
int getM() { return image[0].size(); }
int getQueriesUsed() { return queriesUsed; }
int getPixelValueAt(int x, int y) { return (int) image[x][y];	}

// To get the WAM vector, concatenate the 3 sets of moments.
vecDouble getWAM()
	{
	vecDouble features;
    features.insert(features.end(), momentsH.begin(), momentsH.end());
    features.insert(features.end(), momentsV.begin(), momentsV.end());
    features.insert(features.end(), momentsD.begin(), momentsD.end());
	return features;
	}

// Null constructor.
WAMUpdater() { }

/* 
Initialisation. Given a file containing an image,
do a first-time WAM calculation to initialise all
the info. 
*/
WAMUpdater(string imageFile, int qs)
    {
	// Read in.
	pgmToMatrix(imageFile, image);
	int N, M;
	N = image.size();
	M = image[0].size();

	// Initialise query counter.
	queriesUsed = 0;
	queryStep = qs;
	queryTarget = queryStep;

	// Set up sizes.
	resizeMatrix(variancesH, N, M)
	resizeMatrix(variancesV, N, M)
	resizeMatrix(variancesD, N, M)
	resizeMatrix(residualsH, N, M)
	resizeMatrix(residualsV, N, M)
	resizeMatrix(residualsD, N, M)
	momentsH.resize(9);
	momentsV.resize(9);
	momentsD.resize(9);

	// Calculate everything.
	W.calculateWAM(
	image, 
	L, H, LL, LH, HL, HH, 
	variancesH, 
	variancesV, 
	variancesD, 
	residualsH, 
	residualsV, 
	residualsD, 
	momentsH, 
	momentsV, 
	momentsD);
    }

// Check method:
// Can I perturb pixel (x, y) by v?
bool canMakeOneChange(int x, int y, int v)
	{	
	return (x >= 0 && 
	x < image.size() && 
	y >= 0 && 
	y < image[0].size() && 
	(int)image[x][y] + v >= 0 && 
	(int)image[x][y] + v <= 255);
	}
bool canMakeOneChange(Change c)	{ return canMakeOneChange(c.x, c.y, c.v); }

/*
Check method for group changes.
We're relying on no duplicate pixels 
appearing in the group change. In the rest
of our code, we arrange this to be so.
*/
bool canMakeGroupChange(vector<Change> v)
	{
	for(int i = 0; i < v.size(); i++) if(!canMakeOneChange(v[i])) return false;
	return true;
	}

// Query routine.
vecDouble QueryOneChange(Change C)
	{
	// Count the query.
	countQuery();
	// Make the change.
	makeOneChange(C);
	// Remember the features.	
	vecDouble features = getWAM();
	// Undo the change.
	undoOneChange(C);
	// Return.
	return features;
	}

// Same but for group queries.
vecDouble QueryGroupChange(vector<Change> &v)
	{
	countQuery();
	makeGroupChange(v);
	vecDouble features = getWAM();
	undoGroupChange(v);
	return features;
	}

void countQuery()
	{
	queriesUsed++;
	if(queriesUsed >= queryTarget && queryTarget != 0)
		{
		outputImage("restored"+toString(queryTarget)+".pgm");
		queryTarget += queryStep;
		}
	}

/*
The interface to change the image.
Given a singleton change, cast it 
to a group change and apply it.
*/
void makeOneChange(Change C)
	{
	vector<Change> v;
	v.push_back(C);
	makeGroupChange(v);
	}

// Routine to apply a group change.
void makeGroupChange(vector<Change> &v)
	{
	int i;
	// Firstly change the image.
	for(i = 0; i < v.size(); i++)
		{
		image[v[i].x][v[i].y] += v[i].v;
		image[v[i].x][v[i].y] = MIN(image[v[i].x][v[i].y], 255);
		image[v[i].x][v[i].y] = MAX(image[v[i].x][v[i].y], 0);
		// Recalculate the DWT stuff around each changed pixel.
		if(v[i].v != 0)
			recalculateDWT(v[i].x, v[i].y);
		}
	// Recalculate the variances and residuals around the changes v.
	recalculateVariances(v);
	recalculateResiduals(v);
	// Recalculate the moments.
	recalculateMoments();
	}

// Mechanism for undoing changes.
// Yes, I'm aware that it's
// code duplication.
void undoOneChange(Change C)
	{
	vector<Change> v;
	v.push_back(C);
	undoGroupChange(v);
	}

void undoGroupChange(vector<Change> &v)
	{ 
	int i;
	for(i = 0; i < v.size(); i++)
		{
		image[v[i].x][v[i].y] -= v[i].v;
		image[v[i].x][v[i].y] = MIN(image[v[i].x][v[i].y], 255);
		image[v[i].x][v[i].y] = MAX(image[v[i].x][v[i].y], 0);
		recalculateDWT(v[i].x, v[i].y);
		}
	recalculateVariances(v);
	recalculateResiduals(v);
	recalculateMoments();
	}

private:
// To recalculate the DWT stuff around (x, y),
// just invoke the relevant method in the DWT class.
void recalculateDWT(int x, int y)
	{	
	DWT.recalculateDWT(image, L, H, LL, LH, HL, HH, x, y);
	}

// Direct recalculation of moments.
void recalculateMoments()
	{
	W.computeMoments(residualsH, momentsH);
	W.computeMoments(residualsV, momentsV);
	W.computeMoments(residualsD, momentsD);
	}

// Recalculation of variances around group change v.
void recalculateVariances(vector<Change> &v)
	{
	int i, x, y;
	// If big group, recalculate variances using prefixedSquares.
	if(v.size() > 2) recalculateVariancesByTransform(v);
	// If small, just recalculate the dirty variances by walking over the 9x9 region around them.
	else recalculateVariancesFromDefinition(v);
	}

// OK(i, j) means (i, j) is in range.
#define OK(i, j) ( (i) >= 0 && (i) < N && (j) >= 0 && (j) < M )

void recalculateVariancesFromDefinition(vector<Change> v)
	{
	int x0, y0, x1, y1, x, y;
	int i, j, r, c;
	int N = image.size();
	int M = image[0].size();

	for(int ind = 0; ind < v.size(); ind++)
		{
		if(v[ind].v == 0) continue;
		// (x, y) is the changed pixel.
		x = v[ind].x;
		y = v[ind].y;
		// [x0..x1]x[y0..y1] are the affected variances.
	    x0 = x - 11;
	    y0 = y - 11;
	    x1 = x + 4;
	    y1 = y + 4;

		// For each dirty variance,
	    for(i = x0; i <= x1; i++)
	        for(j = y0; j <= y1; j++)
	            {
				// recalculate it.
				// Note that because the DWT filtering uses a periodicity
				// extension, we need to wrap around in both directions,
				// because the dirtiness of the DWT coefficients wraps around
				// the image. Behold, we're actually taking pains to emulate 
				// the bug in the original WAM code.
				r = (i + 2*N) % N;
				c = (j + 2*M) % M;
				variancesH[r][c] = calculateOneVarianceFromDefinition(LH, r, c);
				variancesV[r][c] = calculateOneVarianceFromDefinition(HL, r, c);
				variancesD[r][c] = calculateOneVarianceFromDefinition(HH, r, c);
				}
		}
	}

// To calculate one dirty variance,
double calculateOneVarianceFromDefinition(matrixDouble &dwt, int x, int y)    {	#ifndef WAM_fix_denominator
	double den[6];
	#endif
	double v[6] = {0, 0, 0, 0, 0};
	int d, dx, dy;
	int N = image.size();
	int M = image[0].size();
	// walk around (x, y) summing things up as necessary.
    for(dx = -4; dx <= 4; dx++)	    for(dy = -4; dy <= 4; dy++)	        if(OK(x + dx, y + dy))	            for(d = 1; d <= 4; d++)	                if(ABS(dx) <= d && ABS(dy) <= d)	                    {	                    v[d] += SQUARE(dwt[x + dx][y + dy]);
						#ifndef WAM_fix_denominator                        den[d]++;
						#endif                        }
	#ifdef WAM_fix_denominator
	// Divide by a fixed denominator.
	v[1] /= 9;
	v[2] /= 25;
	v[3] /= 49;
	v[4] /= 81;
	#else
	// Or a variable one. We used fixed.
    for(d = 1; d <= 4; d++) v[d] /= den[d];
	#endif
	// Find the smallest.
    double best, best1, best2;
	best1 = MIN(v[1], v[2]);
	best2 = MIN(v[3], v[4]);
	// Subtract a half and clamp to zero.
	best = MIN(best1, best2) - 0.5;
	if(best < 0) best = 0;
	// Return.
    return best;
    }
#undef OK

// If there are too many variances to update,
void recalculateVariancesByTransform(vector<Change> v)
	{
	int i, x, y;
	// Calculate the prefixed squares form.
	W.transform(LH);
	// Then update the dirty variances.
	for(i = 0; i < v.size(); i++)
		{
		if(v[i].v == 0) continue;
		x = v[i].x;	y = v[i].y;
		W.computeVariances(LH, variancesH, x - 11, x + 5, y - 11, y + 5);
		}
	// Same.
	W.transform(HL);
	for(i = 0; i < v.size(); i++)
		{
		if(v[i].v == 0) continue;
		x = v[i].x;	y = v[i].y;
		W.computeVariances(HL, variancesV, x - 11, x + 5, y - 11, y + 5);
		}
	// Same.
	W.transform(HH);
	for(i = 0; i < v.size(); i++)
		{
		if(v[i].v == 0) continue;
		x = v[i].x;	y = v[i].y;
		W.computeVariances(HH, variancesD, x - 11, x + 5, y - 11, y + 5);
		}
	}

// To recalculate the residuals around a group change v,
void recalculateResiduals(vector<Change> &v)
	{
	int i, x, y;
	for(i = 0; i < v.size(); i++)
		{
		if(v[i].v == 0) continue;
		// Take each x, y in v.
		x = v[i].x;
		y = v[i].y;
		// And redo the residuals in [x-11..x+4]x[y-11..y+4].
		W.computeResiduals(LH, variancesH, residualsH, x - 11, x + 5, y - 11, y + 5);
		W.computeResiduals(HL, variancesV, residualsV, x - 11, x + 5, y - 11, y + 5);
		W.computeResiduals(HH, variancesD, residualsD, x - 11, x + 5, y - 11, y + 5);
		}
	}

// Output methods.
public:
void outputImage(string fileName)
	{ matrixToPgm(image, fileName);	}

void outputWAM(string fileName)
	{ 
	vecDouble features = getWAM();
	writeVector(fileName, features);
	}

void outputAll(string fileName)
    {
    writeMatrix(fileName+".dwt.H", LH);
    writeMatrix(fileName+".dwt.V", HL);
    writeMatrix(fileName+".dwt.D", HH);

    writeMatrix(fileName+".var.H", variancesH);
    writeMatrix(fileName+".var.V", variancesV);
    writeMatrix(fileName+".var.D", variancesD);

    writeMatrix(fileName+".res.H", residualsH);
    writeMatrix(fileName+".res.V", residualsV);
    writeMatrix(fileName+".res.D", residualsD);

    writeVector(fileName+".mom.H", momentsH);
    writeVector(fileName+".mom.V", momentsV);
    writeVector(fileName+".mom.D", momentsD);
    }
};

