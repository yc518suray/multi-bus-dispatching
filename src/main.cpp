#include<cstdio>
#include<string>
#include<fstream>

using namespace std;

/* ----- declarations ----- */

int cost();

/* ----- main ----- */

int main(int argc, char ** argv)
{
	int opt_data = stoi(argv[2]);
	int algoOpt = 1; // opt for GA

	/* ----- load data ----- */

	int N;					// prediction steps
	int Ns;					// number of stops
	int Nb;					// number of buses
	double Delta;			// minimum dispatching interval
	double Tr;				// round trip time (without stopping)
	double DT;				// dwell time at each stop
	double Hp;				// prediction horizon
	int Capb;				// bus capacity

	string infileName;
	sprintf(infileName, "input-%d.dat", opt_data);
	ifstream infile(infileName);

	infile >> N >> Delta >> Tr >> DT >> Capb >> Hp >> Ns >> Nb;

	int f[N][Ns][Ns] = {{INT_MAX}};// number of psngers coming between trips
	int numLine = ;				   // expected number of lines to input
	int x, y, z;
	for(int i = 0; i < numLine; i++)
	{
		infile >> x >> y >> z;
		if(z == 1)
		{
			z = Ns - y + 1;
		}
		else
		{
			z = z - y;
		}
		infile >> f[x][y - 1][z - 1];
	}
	
	infile.close();

	/* ----- parameters (GA) ----- */
	
	int Np = 50;		// population size
	int Nit = 1000;		// number of iteration
	int chromosome[Np][Ns][Ns] = {{0}};

	/* ----- parameters (HS) ----- */
	
	/* ----- pending ------ */

	/* ----- Opt 1: Genetic Algorithm ----- */
	if(algoOpt == 1)
	{
	}
	/* ----- Opt 2: Harmony Search ----- */
	else if(algoOpt == 2)
	{
		/* ----- pending ----- */
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

int cost()
{
	/* ----- fitness = 1 / cost ------ */
}
