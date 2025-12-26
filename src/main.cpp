// usage: ./main [input data file]

#include<cstdio>
#include<cstdlib>
#include<climits>
#include<string>
#include<fstream>
#include"../lib/dynamics.h"

using namespace std;

/* ----- declarations ----- */

/* ----- main ----- */

int main(int argc, char ** argv)
{
	srand(42);		 // random seed
	int algoOpt = 1; // opt for GA

	/* ----- load data ----- */

	int N;					// prediction steps
	int Ns;					// number of stops
	int Nb;					// number of buses
	double Delta;			// minimum dispatching interval
	double Tr;				// round trip time (without stopping)
	double DT = 180.0;		// dwell time at each stop
	double Hp;				// prediction horizon
	int Capb;				// bus capacity

	ifstream infile(argv[1]);
	infile >> N >> Delta >> Tr >> DT >> Capb >> Hp >> Ns >> Nb;

	int *** f;				// number of psngers coming between trips
	f = new int ** [N];
	for(int i = 0; i < N; i++)
	{
		f[i] = new int * [Ns];
		for(int j = 0; j < Ns; j++)
		{
			f[i][j] = new int [Ns];
			for(int k = 0; k < Ns; k++) f[i][j][k] = INT_MAX;
		}
	}

	int numLine = N * (Ns * Ns + Ns - 2) / 2;
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

	/* ----- establish environment (dynamics) ----- */
	
	Env enviro;
	enviro.initialize(Nb, Ns, N, Capb, f);

	/* ----- establish dispatching indicator ----- */

	vector<vector<int>> X;
	X.resize(N, vector<int>(Nb));

	/* ----- parameters (GA) ----- */
	
	int Np = 50;				   	 // population size
	int Nit = 1000;				   	 // number of iteration
	int chromosome[Np][Ns][Ns] = {{{-1}}};

	/* ----- mutation parameters ----- */

	/* ----- crossover parameters ----- */

	/* ----- parameters (HS) ----- */
	
	/* ----- !pending! ------ */

	/* ----- Opt 1: Genetic Algorithm ----- */
	if(algoOpt == 1)
	{
	}
	/* ----- Opt 2: Harmony Search ----- */
	else if(algoOpt == 2)
	{
		/* ----- !pending! ----- */
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
