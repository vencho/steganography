#include"../header.cpp"
#include"../pgmReader/pgmReader.cpp"
#include"../lsbMatching/LSBDecoder.cpp"



// doDecode stego.pgm KEY [byteLength]

int main(int argc, char *argv[])
{
matrixDouble image;
string fileName;
string message;
int key;



if(argc != 4 && argc != 5)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("doDecode stegoImage KEY outputFile [messageByteLength]\n\n");
	ECHO("Performs lsb decoding with the given key on stegoImage. Writes the output to outputFile. ");
	ECHO("If a length is specified, it reads only so many bytes. If not, it walks over the whole image.\n\n");
	exit(0);
	}


fileName = string(argv[1]);
key = atoi(argv[2]);


LSBDecoder D;
if(argc == 4)
	message = D.messageFromPgm(fileName, key);
else
	message = D.messageFromPgm(fileName, key, atoi(argv[4])*8);

FILE *fout = fopen(argv[3], "w");
FECHO(fout, "%s", message.c_str());
fclose(fout);
/*
for(int i = 0; i < message.length(); i++)
	ECHO("%d ", (int)message[i]);
ECHO("\n\n");
*/
return 0;
}

