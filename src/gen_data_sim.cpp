// Usage: ./gen_data_sim.exe [output-file-path-name]
// Note: in this model, the round trip is fixed, i.e. traffic uncertainty
// and bus overtake. It also assumes equal distance between adjacent stops.

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cmath>
#include<climits>

using namespace std;

/* ----- function declarations ----- */

void updateStats(ofstream & out, int ** cc, int * tc, int d, int L);
void scheduleArrival(double & t, double mu, double ** elist, int ** cc, double & tinc, int L);
void findEarliest(double ** elist, int * index, int L);
double expoDistro(double lbda, double x);

/* ----- main ----- */

int main(int argc, char ** argv)
{
	srand(42);
	string outfile_name = argv[1];
	ofstream outfile(outfile_name);

	/* ----- parameter settings ----- */
	double lambda = 120.0; 	// mean of exponential distribution (seconds)
	double Delta = 600.0; 	// min dispatched headway (seconds)
	const int Ns = 15; 		// number of stations
	int N = 10; 			// number of prediction steps (trips)
	double Tr = 180.0 * Ns; // round trip time (seconds, without stopping)
	double DT = 180.0; 		// dwell time at each stop (seconds)
	double Hp = N * Delta; 	// prediction horizon (seconds)
	int Nb = 3; 			// number of buses
	int Capb = 100; 		// capacity of each bus

	int df = 3;				// dawn factor for the "dawn" time period

	/* ----- variables declarations ----- */
	int ** comingCount;			// number of passengers coming between trips
	comingCount = new int * [Ns];
	for(int i = 0; i < Ns; i++) comingCount[i] = new int [Ns];
	
	double ** eventList;		// record scheduled event timing
	eventList = new double * [Ns];
	for(int i = 0; i < Ns; i++) eventList[i] = new double [Ns];
	
	int tripCount[Ns] = {0};
	int tripCountCurr = 0;

	double timeInc = 0.0;		// time increment since last updateStats
	double dawn = df * Delta;	// time period to "collect" passengers for
								// the 1st trip
	double time = -dawn;		// simulation clock (seconds), initialized
								// to "dawn" seconds before the 1st trip
	double T = N * (Tr + DT * Ns);
								// total time of N trips, including
								// stopping time
	double randNum = 0.0;

	/* ----- output first line (parameters) ----- */
	outfile << N << " " << Delta << " " << Tr << " " << DT << " ";
	outfile << Capb << " " << Hp << " " << Ns << " " <<  Nb << std::endl;

	/* ----- initialize the arrays ----- */
	for(int i = 0; i < Ns; i++)
	{
		for(int j = 0; j < Ns; j++)
		{
			comingCount[i][j] = 0;
		}
	}

	for(int i = 0; i < Ns; i++)
	{
		if(i == 0)
		{
			// destinations: station 2 ~ Ns
			// schedule the first arriving passengers

			for(int j = 0; j < Ns - 1; j++)
			{
				randNum = static_cast<double>(rand()) / INT_MAX;
				eventList[i][j] = time + expoDistro(lambda, randNum);
			}
			eventList[i][Ns - 1] = static_cast<double>(INT_MAX);
		}
		else
		{
			// destinations: station (i + 1) ~ Ns, 1
			// schedule the first arriving passengers

			for(int j = 0; j < Ns - i + 1; j++)
			{
				randNum = static_cast<double>(rand()) / INT_MAX;
				eventList[i][j] = time + expoDistro(lambda, randNum);
			}
			for(int j = Ns - i + 1; j < Ns; j++)
			{
				eventList[i][j] = static_cast<double>(INT_MAX);
			}
		}
	}
	
	/* ----- run simulation ----- */
	while(time <= T)
	{
		if(timeInc >= (Tr / Ns) && time >= 0)
		{
			updateStats(outfile, comingCount, tripCount, tripCountCurr, Ns);
			tripCount[tripCountCurr]++;
			tripCountCurr = (tripCountCurr + 1) % Ns;
			timeInc = 0.0;
		}
		scheduleArrival(time, lambda, eventList, comingCount, timeInc, Ns);
	}
	/* ----- simulation finished ----- */

	cout << "Simulation finished." << endl;

	outfile.close();
	return 0;
}

/* ----- function definitions ----- */

double expoDistro(double lbda, double x)
{
	// However, the inter-arrival time may not be
	// exponential distributed ?
	
	return -lbda * log(x);
}

void updateStats(ofstream & out, int ** cc, int * tc, int d, int L)
{
	// L = Ns
	// cc = comingCount
	// tc = tripCount

	if(d == 0)
	{
		for(int i = 0; i < L - 1; i++)
		{
			out << tc[d] << " " << (d + 1) << " ";
			out << (i + 2) << " " << cc[d][i] << endl;
			cc[d][i] = 0;
		}
	}
	else
	{
		int tmp = 0;
		for(int i = 0; i < L - d; i++)
		{
			if(i == L - d - 1)
			{
				tmp = 1;
			}
			else
			{
				tmp = i + d + 2;
			}
			out << tc[d] << " " << (d + 1) << " ";
			out << tmp << " " << cc[d][i] << endl;
			cc[d][i] = 0;
		}
	}
}

void scheduleArrival(double & t, double mu, double ** elist, int ** cc, double & tinc, int L)
{
	// cc = comingCount
	// L = Ns

	/* --- find earliest event --- */
	int idx[2] = {-1};
	findEarliest(elist, idx, L);
	++cc[idx[0]][idx[1]];
	
	/* --- update time & increment --- */
	tinc += (elist[idx[0]][idx[1]] - t);
	t = elist[idx[0]][idx[1]];
	
	/* --- schedule next arrival --- */
	double rnd = static_cast<double>(rand()) / INT_MAX;
	elist[idx[0]][idx[1]] = t + expoDistro(mu, rnd);
}

void findEarliest(double ** elist, int * index, int L)
{
	// L = Ns

	int idx_1 = -1;
	int idx_2 = -1;
	double min = static_cast<double>(INT_MAX);
	for(int i = 0; i < L; i++)
	{
		for(int j = 0; j < L - i; j++)
		{
			if(elist[i][j] < min)
			{
				idx_1 = i;
				idx_2 = j;
				min = elist[i][j];
			}
		}
	}

	index[0] = idx_1;
	index[1] = idx_2;
}
