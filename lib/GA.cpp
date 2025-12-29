#include<vector>
#include<random>
#include<algorithm>
#include"GA.h"

using namespace std;

/* ----- member function definitions ----- */
void GeneticAlgorithm::initializePopulation()
{
	population.clear();
	
	for (int p = 0; p < pop_size; ++p)
	{
    	Individual ind(n_trips, n_buses);
    	for (int b = 0; b < n_buses; ++b)
		{
        	for (int k = 0; k < n_trips; ++k)
			{
            	ind.genes[k][b] = (dist(gen_class) > 0.5) ? 1 : 0;
            	for (int k_prev = 0; k_prev < k; ++k_prev)
				{
                	if (ind.genes[k_prev][b] == 1 && (k - k_prev) < t_revisit_steps)
					{
                    	ind.genes[k][b] = 0; 
                    	break;
                    }
                }
            }
        }
        population.push_back(ind);
    }
}

Individual GeneticAlgorithm::selectParent()
{
	int tournamentSize = 5;
	int bestIdx = -1;
	double bestFit = 1e18;
	uniform_int_distribution<> idxDist(0, pop_size - 1);

	for (int i = 0; i < tournamentSize; ++i)
	{
    	int randomIdx = idxDist(gen_class);
    	if (population[randomIdx].fitness < bestFit)
		{
        	bestFit = population[randomIdx].fitness;
        	bestIdx = randomIdx;
		}
	}
	
	return population[bestIdx];
}
    
Individual GeneticAlgorithm::crossover(const Individual& p1, const Individual& p2)
{
	// p1 -> parent1, p2 -> parent2
    
	Individual offspring = p1; 
    if (dist(gen_class) < crossover_rate)
	{
    	for (int b = 0; b < n_buses; ++b)
		{
        	uniform_int_distribution<> pointDist(0, n_trips - 1);
        	int crossoverPoint = pointDist(gen_class);
            for (int k = crossoverPoint; k < n_trips; ++k)
			{
            	offspring.genes[k][b] = p2.genes[k][b];
            }
        }
    }
	
	return offspring;
}

void GeneticAlgorithm::mutate(Individual& ind)
{
	double gene_mutation_prob = mutation_rate / (double)(n_buses * n_trips);
    for (int b = 0; b < n_buses; ++b)
	{
    	for (int k = 0; k < n_trips; ++k)
		{
        	if (dist(gen_class) < gene_mutation_prob)
			{
            	ind.genes[k][b] = 1 - ind.genes[k][b]; 
			}
            for (int k_prev = 0; k_prev < k; ++k_prev)
			{
            	if (ind.genes[k_prev][b] == 1 && (k - k_prev) < t_revisit_steps)
				{
                	ind.genes[k][b] = 0; 
                	break;
                }
            }
        }
    }
}
