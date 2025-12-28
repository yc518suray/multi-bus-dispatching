// declare the dynamics (boarding flow, number of waitings, etc)

#ifndef DYNAMICS_H
#define DYNAMICS_H

#include<vector>

using namespace std;

class Env
{
private:
	int Capb;		// bus capacity
	int Ns;			// number of stations
	int Nb;			// number of buses
	int N;			// number of prediciton steps
	int delta;		// minimum dispatching headway (seconds)
	double alpha = 2.0;
					// average boarding time for each passenger

	int *** B;		// boarding flow (decision variable)
	int *** V;		// number of passengers on bus
	int *** P;		// number of passengers waiting at stops
	int *** f;		// number of coming passengers
	int *** choice; // used when determining B
	int *** X;		// used to calculate perceived waiting time (quadratic)

public:
	Env();						   // constructor
	~Env();						   // destructor
	void initialize(int nb, int ns, int n, int capb, int *** coming, double Delta);
	void printEverything(int opt); // for debugging
	void clear();
	int cost(int opt, vector<vector<int>> x, double DT);
};


#endif
