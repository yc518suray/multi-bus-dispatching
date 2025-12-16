// declare the dynamics (boarding flow, number of waitings, etc)
#ifndef DYNAMICS_H
#define DYNAMICS_H

using namespace std;

class Env
{
private:
	int Capb;		// bus capacity
	int bus_index;	// used to index data for each bus

	int *** B;		// boarding flow (decision variable)
  //int ** A;		// alighting flow
	int *** V;		// number of passengers on bus
	int *** P;		// number of passengers waiting at stops
	int *** f;		// number of coming passengers

public:
	void initialize(int Nb, int Ns, int N, int capb, int *** coming);// initialize everything
	void printEverything(int opt);									 // for debugging
	int cost(int opt);												 // calculate linear/nonlinear cost
};


#endif
