#include"../header.cpp"
#include"../utils/matrixIO.cpp"
#include"../pgmReader/pgmReader.cpp"
#include"../lsbMatching/LSBEncoder.cpp"



using namespace std;



int main(int argc, char *argv[])
{
matrixInt usedPixels, changesMade;

if(argc != 6 && argc != 7)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("doEncode originalImage stegoImage encodingLog KEY messageFile [messageByteLength]\n\n");	
	exit(0);
	}

// Read in the message.
string message;
FILE *fin = fopen(argv[5], "rb");
while(1)
	{
	char ch;
	if(fscanf(fin, "%c", &ch) != 1) break;
	message.append(1, ch);
	}
fclose(fin);

// If a length was specified,
// truncate the message.
if(argc == 7) 
	{
	int length = atoi(argv[6]);
	if(length < message.length()) message = message.substr(0, length);
	}

// ECHO("Message truncated. message.length = %d\n", message.length());
// Invoke the LSB encoder.
LSBEncoder E;
E.messageIntoPgm(message, string(argv[1]), string(argv[2]), string(argv[3]), atoi(argv[4]), usedPixels, changesMade);

// ECHO("Message embedded\n");
// Output to stdout.
/*
for(int i = 0; i < message.length(); i++)
	ECHO("%d ", (int) message[i]);
ECHO("\n\n");
*/

return 0;
}

