#ifndef GA_H
#define GA_H

#include<random>
#include<vector>

using namespace std;

/* ----- Individual struct ----- */
struct Individual
{
    vector<vector<int>> genes;
    double fitness;

    Individual(int N_trips, int N_buses)
	{
        genes.resize(N_trips, vector<int>(N_buses, 0));
        fitness = 1e18; // actually cost
    }
};

/* ----- Genetic Algorithm class ----- */
class GeneticAlgorithm
{
public:
	int n_trips;
    int n_buses;
    int t_revisit_steps; 
    int pop_size;
    double crossover_rate;
    double mutation_rate;
    vector<Individual> population;

	std::mt19937 gen_class;
	std::uniform_real_distribution<> dist{0.0, 1.0};
    
	GeneticAlgorithm(int trips, int buses, int t_steps,
					 int p_size, double c_rate, double m_rate, std::mt19937 g): n_trips(trips), n_buses(buses),
					   t_revisit_steps(t_steps), pop_size(p_size), crossover_rate(c_rate), mutation_rate(m_rate), gen_class(g){}

	void initializePopulation();
    Individual selectParent();
    Individual crossover(const Individual& p1, const Individual& p2);
    void mutate(Individual& ind);
};

#endif
