// Usage: ./gen_data_sim.exe [output-file-path-name] [dawn factor df]

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cmath>

using namespace std;

/* ----- function declarations ----- */

double expoDistro(double lbda, double x);
void updateStats(int * comingCount, int cnt);
void scheduleArrival(int * eventList, int idx);

/* ----- main ----- */

int main(int argc, char ** argv)
{
	srand(42);
	string outfile_name = argv[1];
	ofstream outfile(outfile_name);

	/* ----- parameter settings ----- */
	double lambda = 1.0; 	// mean of exponential distribution
	double Delta = 600.0; 	// min dispatched headway (seconds)
	int Ns = 15; 			// number of stations
	int N = 10; 			// number of prediction steps (trips)
	double Tr = 180.0 * Ns; // round trip time (seconds, without stopping)
	double DT = 180.0; 		// dwell time at each stop (seconds)
	double Hp = N * Delta; 	// prediction horizon (seconds)
	int Nb = 3; 			// number of buses
	int Capb = 100; 		// capacity of each bus

	int df = stoi(argv[2]); // dawn factor for the "dawn" time period

	/* ----- variables declarations ----- */
	int comingCount[Ns][Ns];	// number of passengers coming between trips
	int eventList[Ns][Ns]; 	 	// record scheduled event timing
	int updateCount = 0;

	double timeInc = 0.0;		// time increment since last updateStats
	double dawn = df * Delta;	// time period to "collect" passengers for
								// the 1st trip
	double time = -dawn;		// simulation clock (seconds), initialized
								// to "dawn" seconds before the 1st trip
	double T = Tr + DT * Ns;	// total time of N trips, including
								//stopping time

	/* ----- output first line (parameters) ----- */
	outfile << N << " " << Delta << " " << Tr << " " << DT << " ";
	outfile << Capb << " " << Hp << " " << Ns << " " <<  Nb << std::endl;

	/* ----- initialize the arrays ----- */
	for(int i = 0; i < Ns; i++)
	{
		for(int j = 0; j < Ns; j++)
		{
			comingCount[i][j] = 0;
			eventList[Ns][Ns] = INT_MAX;
		}
	}

	/* ----- run simulation ----- */
	while(time <= T)
	{
	}

	outfile.close();
	return 0;
}

/* ----- function definitions ----- */

double expoDistro(double lbda, double x)
{
	// however, the inter-arrival time may not be
	// exponential distributed
	
	return -lbda * log(1 - x);
}

void updateStats(int * comingCount, int cnt)
{
}

void scheduleArrival(int * eventList, int idx)
{
}
