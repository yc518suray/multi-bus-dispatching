#include<cstdio>
#include<string>

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

	/* ----- Opt 1: Genetic Algorithm ----- */

	/* ----- Opt 2: Harmony Search ----- */

	/* ----- Results ----- */

	return 0;
}

/* ----- function definitions ----- */

string getLine(ifstream & infile)
{
}
