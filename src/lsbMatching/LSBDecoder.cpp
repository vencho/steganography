// LSBDecoder.cpp
// A class which performs decoding.
#include"./perms.cpp"
#include"./bitconversion.cpp"

class LSBDecoder
{

public:
LSBDecoder() { }

/*
A public routine with two versions. Given the 
name of a pgm file and a key, extract the 
message embedded into it using LSB matching. 
Optionally, a message length may be provided,
if not all pixels hold payload.
*/
string messageFromPgm(string fileName, int key, int expectedBitLength)
	{
	matrixDouble image;
	pgmToMatrix(fileName, image);
	return messageFromMatrix(image, key, expectedBitLength);
	}

string messageFromPgm(string fileName, int key)
	{
	matrixDouble image;
	pgmToMatrix(fileName, image);
	return messageFromMatrix(image, key);
	}

private:
/*
Decoding procedure.
Initialises the RNG with key, generates a
permutation of the pixels, traverses them in the order
specified by the permutation, and collects the LSBs of
the traversed pixels. Then it lumps them into groups of 8,
thus getting a byte message.

expectedLength is there to tell us when to stop
the traversal. Obviously there will be pixels into
which no information has been embedded, so there's
no sense in looking at their LSBs. If this parameter is missing,
then keep going until all pixels are traversed.
*/
string messageFromMatrix(matrixDouble &image, int key, int expectedBitLength)
	{
	int N, M;
	int i, j;
	vecBool bitString;
	vecInt perm;

	N = image.size();
	M = image[0].size();
	if(expectedBitLength > N*M) expectedBitLength = N*M;

	srand(key);
	randomPermutation(N*M, perm);

	for(i = 0; i < expectedBitLength; i++)
   		{
		int pixel = perm[i];
		int pixelX = pixel / M;
		int pixelY = pixel % M;
		int pixelValue = (int) image[pixelX][pixelY];

		bitString.push_back(LSB(pixelValue));
    	}
	return bitsToMessage(bitString);
	}

string messageFromMatrix(matrixDouble &image, int key)
	{ return messageFromMatrix(image, key, image.size()*image[0].size()); }
};

