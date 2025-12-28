#include <cstdio>
#include <cstdlib>
#include <climits>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include "../lib/dynamics.h" 

using namespace std;

/* ----- Global Random Generator ----- */

std::random_device rd;
std::mt19937 gen(42); 
std::uniform_real_distribution<> dis(0.0, 1.0);

/* ----- GA Structures & Classes ----- */

struct Individual {
    vector<vector<int>> genes;
    double fitness;

    Individual(int N_trips, int N_buses) {
        genes.resize(N_trips, vector<int>(N_buses, 0));
        fitness = 1e18; 
    }
};

class GeneticAlgorithm {
public:
    int n_trips;
    int n_buses;
    int t_revisit_steps; 
    int pop_size;
    double crossover_rate;
    double mutation_rate;
    vector<Individual> population;

    GeneticAlgorithm(int trips, int buses, int t_steps, int p_size, double c_rate, double m_rate)
    				 : n_trips(trips), n_buses(buses), t_revisit_steps(t_steps),
					   pop_size(p_size), crossover_rate(c_rate), mutation_rate(m_rate) {}

    void initializePopulation() {
        population.clear();
        for (int p = 0; p < pop_size; ++p) {
            Individual ind(n_trips, n_buses);
            for (int b = 0; b < n_buses; ++b) {
                for (int k = 0; k < n_trips; ++k) {
                    ind.genes[k][b] = (dis(gen) > 0.5) ? 1 : 0;
                    for (int k_prev = 0; k_prev < k; ++k_prev) {
                        if (ind.genes[k_prev][b] == 1 && (k - k_prev) < t_revisit_steps) {
                            ind.genes[k][b] = 0; 
                            break;
                        }
                    }
                }
            }
            population.push_back(ind);
        }
    }

    Individual selectParent() {
        int tournamentSize = 5;
        int bestIdx = -1;
        double bestFit = 1e18;
        uniform_int_distribution<> idxDist(0, pop_size - 1);

        for (int i = 0; i < tournamentSize; ++i) {
            int randomIdx = idxDist(gen);
            if (population[randomIdx].fitness < bestFit) {
                bestFit = population[randomIdx].fitness;
                bestIdx = randomIdx;
            }
        }
        return population[bestIdx];
    }

    Individual crossover(const Individual& parent1, const Individual& parent2) {
        Individual offspring = parent1; 
        if (dis(gen) < crossover_rate) {
            for (int b = 0; b < n_buses; ++b) {
                uniform_int_distribution<> pointDist(0, n_trips - 1);
                int crossoverPoint = pointDist(gen);
                for (int k = crossoverPoint; k < n_trips; ++k) {
                    offspring.genes[k][b] = parent2.genes[k][b];
                }
            }
        }
        return offspring;
    }

    void mutate(Individual& ind) {
        double gene_mutation_prob = mutation_rate / (double)(n_buses * n_trips);
        for (int b = 0; b < n_buses; ++b) {
            for (int k = 0; k < n_trips; ++k) {
                if (dis(gen) < gene_mutation_prob) {
                    ind.genes[k][b] = 1 - ind.genes[k][b]; 
                }
                for (int k_prev = 0; k_prev < k; ++k_prev) {
                    if (ind.genes[k_prev][b] == 1 && (k - k_prev) < t_revisit_steps) {
                        ind.genes[k][b] = 0; 
                        break;
                    }
                }
            }
        }
    }
};

/* ----- main ----- */

int main(int argc, char ** argv)
{
    if (argc < 2) {
        printf("Usage: ./program [data_file]\n");
        return 1;
    }

    srand(42);       
    int algoOpt = 1; 	// 1 for GA

    /* ----- load data ----- */
    int N;				// prediction steps
    int Ns;				// number of stops
    int Nb;				// number of buses
    double Delta;		// minimum dispatching interval
    double Tr;			// round trip time
    double DT;			// dwell time
    double Hp;			// prediction horizon
    int Capb;			// bus capacity

    ifstream infile(argv[1]);
    if (!infile) {
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
    for(int i = 0; i < numLine; i++) {
        
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
    int Nit = 3000;       // number of iterations
    double Pc = 0.5;       // crossover rate
    double Pm = 0.5;       // mutation rate

    /* ----- calculate T (Revisit Interval) ----- */
    double total_round_trip_time = Tr + (Ns * DT);
    int T_revisit_steps = static_cast<int>(ceil(total_round_trip_time / Delta));
    if (T_revisit_steps < 1) T_revisit_steps = 1;

	/* === debug === */
	cout << "\nT_revisit_steps = " << T_revisit_steps << endl;
	/* === debug === */

    /* ----- Algorithm Execution ----- */
    if(algoOpt == 1) // GA
    {
        printf("Starting GA Optimization ...\n");
        
        GeneticAlgorithm ga(N, Nb, T_revisit_steps, Np, Pc, Pm);
        ga.initializePopulation();

        for (auto& ind : ga.population) {
            ind.fitness = (double)enviro.cost(1, ind.genes, DT); 
		}

		/* === debug (test chromosome cost) === */
		Individual test(N, Nb);
		char testing;
		cout << "\ntest cost of genes? Ans (y/n): ";
		cin >> testing;
		while(testing != 'n')
		{
			cout << "input genes (N = " << N << "):" << endl;
			for(int i = 0; i < N; i++)
			{
				cout << "input for trip " << i << ": ";
				for(int j = 0; j < Nb; j++)
				{
					cin >> test.genes[i][j];
				}
			}
			cout << "cost of this chromosome is " << enviro.cost(1, test.genes, DT) << endl;
			cout << "test cost of genes? Ans(y/n): ";
			cin >> testing;
		}
		/* === debug === */

        for (int it = 0; it < Nit; ++it) {
            sort(ga.population.begin(), ga.population.end(), [](const Individual& a, const Individual& b) {
                return a.fitness < b.fitness;
            });

            if (it % 100 == 0 || it == Nit - 1) {
            	printf("Iteration %d: Best Cost = %.2f\n", it, ga.population[0].fitness);
            }

            vector<Individual> newPopulation;
            newPopulation.push_back(ga.population[0]);
            newPopulation.push_back(ga.population[1]);

            while ((int)newPopulation.size() < Np) {
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
        for (int k = 0; k < N; ++k) {
            printf("Trip %2d: ", k);
            for (int b = 0; b < Nb; ++b) {
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

    return 0;
}
