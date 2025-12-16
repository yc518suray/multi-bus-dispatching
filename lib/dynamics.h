// declare the dynamics (boarding flow, number of waitings, etc)

#ifndef DYNAMICS_H
#define DYNAMICS_H

using namespace std;

class Env
{
private:
	int Capb;		// bus capacity
	int Ns;			// number of stations
	int Nb;			// number of buses
	int N;			// number of prediciton steps
	int bus_index;	// used to index data for each bus

	int *** B;		// boarding flow (decision variable)
  //int ** A;		// alighting flow
	int *** V;		// number of passengers on bus
	int *** P;		// number of passengers waiting at stops
	int *** f;		// number of coming passengers

public:
	Env();						   // constructor
	~Env();						   // destructor
	void initialize(int nb, int ns, int n, int capb, int *** coming);
	void printEverything(int opt); // for debugging
	int cost(int opt);
	void clear();
};


#endif
