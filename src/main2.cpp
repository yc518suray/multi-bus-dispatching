#include<cstdio>
#include<string>
#include<fstream>

using namespace std;

/* ----- declarations ----- */

string getLine(ifstream & infile);

int main(int argc, char ** argv)
{
	int opt_data = stoi(argv[2]);

	/* ----- parameters (GA) ----- */

	/* ----- parameters (HS) ----- */

	/* ----- load data ----- */
	
	string infileName;
	sprintf(infileName, "input-%d.dat", opt_data);

	ifstream infile(infileName);

	infile.close();

	/* ----- Opt 1: Genetic Algorithm ----- */
	if(algoOpt == 1)
	{
	}
	/* ----- Opt 2: Harmony Search ----- */
	else if(algoOpt == 2)
	{
	}
	else
	{
		printf("wrong algorithm option\n");
		exit(1);
	}

	/* ----- Results ----- */

	return 0;
}

/* ----- function definitions ----- */

string getLine(ifstream & infile)
{
}
