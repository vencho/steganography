// LSBEncoder.cpp
// A class which takes care of encoding.

#include"./perms.cpp"
#include"./bitconversion.cpp"

class LSBEncoder
{
public:
LSBEncoder() { }

/*
Public routine. It embeds a given message into
an image specified by its name using a given key. 
The parameters also specify the name of the stego
image and the name of the file to be used as a 
log of the encoding.
*/
void messageIntoPgm(string message, string imageName, string newName, string logFile, int key, matrixInt &usedPixels, matrixInt &changesMade)
	{
	matrixDouble image;
	// Read the image.
	pgmToMatrix(imageName, image);
	// Put a message into it.
	messageIntoMatrix(message, image, key, usedPixels, changesMade);
	// Write the image back.
	matrixToPgm(image, newName);

	// The log contains information about
	// which pixels contain and the payload,
	// and how much each pixel has been changed:
	// -1 or +1 for payload pixels, 0 for non-payload.
	FILE *fout = fopen(logFile.c_str(), "w");
								//	FECHO(fout, "%s\n", imageName.c_str());
								//	FECHO(fout, "%s\n", newName.c_str());
	writeMatrix(fout, usedPixels);
	writeMatrix(fout, changesMade);
	fclose(fout);
	}


private:
/*
Encoding procedure. Converts  the 
message into a bit string, initialises 
the RNG with the given key and uses 
Knuth shuffle to generate a random 
permutation of the pixels of the image.
Then it traverses the pixels in the
order specified by the permutation,
and embeds each bit of the message 
into a pixel as specified by the algorithm
for LSB matching.
*/
void messageIntoMatrix(string message, matrixDouble &image, int key, matrixInt &usedPixels, matrixInt &changesMade)
	{
	int N, M;
	int i, j;
	vecBool bitMessage;
	vecInt perm;
	// Obtain a bitstring.
	bitMessage = messageToBits(message);
	N = image.size();
	M = image[0].size();


	// Prepare matrices.
	usedPixels.clear();
	changesMade.clear();
	usedPixels.resize(N);
	changesMade.resize(N);
	for(i = 0; i < N; i++) { usedPixels[i].resize(M); changesMade[i].resize(M); }
	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
			{
			usedPixels[i][j] = 0;
			changesMade[i][j] = 0;
			}

	// Initialise the RNG with key.
	srand(key);
	// Get a random permutation.
	randomPermutation(N*M, perm);

	// Do the encoding.
	for(i = 0; i < bitMessage.size(); i++)
		{
		int pixel = perm[i];
		int pixelX = pixel / M;
		int pixelY = pixel % M;
		int pixelValue = (int) image[pixelX][pixelY];
		int bit = bitMessage[i];
		int change = 0;

		usedPixels[pixelX][pixelY] = 1;

		// if LSB(pixel) = bit, leave the pixel as it is.
		// otherwise, randomly choose to increment or decrement it.
		if(LSB(pixelValue) == bit) continue;
		else
			{
			if(pixelValue == 255) change = -1;
			else if(pixelValue == 0) change = 1;
			else if(rand() % 2 == 0) change = 1;
			else change = -1;
			}
		image[pixelX][pixelY] = pixelValue + change;
		changesMade[pixelX][pixelY] = change;
   		}
	}

};


