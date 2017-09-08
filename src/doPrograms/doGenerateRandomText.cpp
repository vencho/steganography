#include"../header.cpp"



int main(int argc, char *argv[])
{
int i;

srand(time(0));

if(argc != 3)
	{
	ECHO("Wrong number of arguments. Usage:\n");
	ECHO("doGenerateRandomText byteLength fileName\n\n");
	ECHO("Generates a random text of the specified length, and puts it into fileName. ");
	ECHO("Characters are drawn randomly from [-128..128), ie signed char. RNG is initialised with current time.\n");
	exit(0);
	}


FILE *fout = fopen(argv[2], "wb");
int length = atoi(argv[1]);
for(i = 0; i < length; i++)
	{
	int letter = (rand() % 256)-128; // [0..255] -128 = [-128..127]
	FECHO(fout, "%c", (char)letter);
	}
fclose(fout);

return 0;
}

