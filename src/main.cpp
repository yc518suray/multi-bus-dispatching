// Usage: ./main [input data file]
//
// main program of genetic algorithm

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cmath>

#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include "../lib/dynamics.h" 
#include "../lib/GA.h"

using namespace std;

/* ----- Global Random Generator ----- */

//std::random_device rd;
std::mt19937 gen(42); 
std::uniform_real_distribution<> dis(0.0, 1.0);

/* ----- main ----- */

int main(int argc, char ** argv)
{
	/* ----- runtime timing start ----- */
	chrono::steady_clock::time_point ts = chrono::steady_clock::now();

    if (argc < 2)
	{
        printf("Usage: ./main [input data file]\n");
        return 1;
    }

    /* ----- load data ----- */
    int N;				// prediction steps
    int Ns;				// number of stops
    int Nb;				// number of buses
    double Delta;		// minimum dispatching interval
    double Tr;			// round trip time
    double DT;			// dwell time
    double Hp;			// prediction horizon
    int Capb;			// bus capacity

    int algoOpt = 1; 	// 1 for GA
    
	ifstream infile(argv[1]);
    if (!infile)
	{
        printf("Error opening file.\n");
        return 1;
    }
    
    infile >> N >> Delta >> Tr >> DT >> Capb >> Hp >> Ns >> Nb;

    int *** f;
    f = new int ** [N];
    for(int i = 0; i < N; i++)
	{
        f[i] = new int * [Ns];
        for(int j = 0; j < Ns; j++)
		{
            f[i][j] = new int [Ns];
            for(int k = 0; k < Ns; k++) f[i][j][k] = 0;
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
    enviro.initialize(Nb, Ns, N, Capb, f, Delta);

    /* ----- GA Parameters ----- */
    int Np 	= 50;          // population size
    int Nit = 1000;		   // number of iterations
    double Pc = 0.5;	   // crossover rate
    double Pm = 0.5;       // mutation rate

    /* ----- calculate T (Revisit Interval) ----- */
    double total_round_trip_time = Tr + (Ns * DT);
    int T_revisit_steps = static_cast<int>(ceil(total_round_trip_time / Delta));
    if (T_revisit_steps < 1) T_revisit_steps = 1;

	/* === debug === */
	printf("\nT_revisit_steps = %d\n", T_revisit_steps);
	/* === debug === */


    /* ----- Algorithm Execution ----- */
    if(algoOpt == 1) // GA
    {
        printf("Starting GA Optimization ...\n");
        
        GeneticAlgorithm ga(N, Nb, T_revisit_steps, Np, Pc, Pm, gen);
        ga.initializePopulation();

		/* --- calculation initial fitness --- */
        for (auto& ind : ga.population)
		{
            ind.fitness = (double)enviro.cost(1, ind.genes, DT); 
		}

		/* --- iterations --- */
        for (int it = 0; it < Nit; ++it)
		{
            sort(ga.population.begin(), ga.population.end(), [](const Individual& a, const Individual& b){ return a.fitness < b.fitness; });

            if (it % 100 == 0 || it == Nit - 1)
			{
            	printf("Iteration %d: Best Cost = %.2f\n", it, ga.population[0].fitness);
            }

            vector<Individual> newPopulation;
            newPopulation.push_back(ga.population[0]);
            newPopulation.push_back(ga.population[1]);

            while ((int)newPopulation.size() < Np)
			{
                Individual p1 = ga.selectParent();
                Individual p2 = ga.selectParent();
                Individual child = ga.crossover(p1, p2);
                ga.mutate(child);
                
                child.fitness = (double)enviro.cost(1, child.genes, DT); 
                
                newPopulation.push_back(child);
            }
            ga.population = newPopulation;
        }

        Individual bestSol = ga.population[0];
        printf("\nOptimal Solution Found:\n");
        printf("Final Cost: %.2f\n", bestSol.fitness);
        
        printf("Dispatching Schedule (X):\n");
        for (int k = 0; k < N; ++k)
		{
            printf("Trip %2d: ", k + 1);
            for (int b = 0; b < Nb; ++b)
			{
                printf("%d ", bestSol.genes[k][b]);
            }
            printf("\n");
        }
    }
    else if(algoOpt == 2) // HS
    {
        printf("Harmony Search is not implemented yet.\n");
    }
    else
    {
        printf("wrong algorithm option\n");
        exit(1);
    }

	/* ----- runtime timing end ----- */
	chrono::steady_clock::time_point te = chrono::steady_clock::now();
	printf("time elapsed: %ld ms\n",
		   chrono::duration_cast<chrono::milliseconds>(te - ts).count());

    return 0;
}
