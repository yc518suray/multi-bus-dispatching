// Usage: ./gen_data_sim.exe [output-file-path-name]
// 
// Note1: in this model, the round trip is fixed, i.e. no traffic uncertainty
// and no bus overtake. It also assumes equal distance between adjacent stops.
//
// Note2: Passengers coming during dwell time are NOT counted for the
// present trip (but for the next trip).

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cmath>
#include<climits>

using namespace std;

/* ----- function declarations ----- */

void updateStats(ofstream & out, int ** cc, int * tc, int d, int L);
void scheduleArrival(double & t, double mu, double ** el, int ** cc, int L);
void findEarliest(double ** elist, int * index, int L);
double expoDistro(double lbda, double x);
int findEarliestCheck(double * ckpt, int L);

/* ----- main ----- */

int main(int argc, char ** argv)
{
	srand(42);
	string out_name = argv[1];
	ofstream out(out_name);

	/* ----- parameter settings ----- */
	double lambda = 120.0; 	// mean of exponential distribution (seconds)
	double Delta = 600.0; 	// min dispatched headway (seconds)
	const int Ns = 25; 		// number of stations
	int N = 12; 			// number of prediction steps (trips)
	double Tr = 180.0 * Ns; // round trip time (seconds, without stopping)
	double DT = 180.0; 		// dwell time at each stop (seconds)
	double Hp = N * Delta; 	// prediction horizon (seconds)
	int Nb = 5; 			// number of buses
	int Capb = 100; 		// capacity of each bus

	int df = 1;				// dawn factor for the "dawn" time period
	int Ntr = static_cast<int>((Tr + DT * Ns) / Delta);
							// number of trips on route at the same time

	/* ----- variables declarations ----- */
	int ** comingCount;			// number of passengers coming between trips
	comingCount = new int * [Ns];
	for(int i = 0; i < Ns; i++) comingCount[i] = new int [Ns];
	
	double ** eventList;		// record scheduled event timing
	eventList = new double * [Ns];
	for(int i = 0; i < Ns; i++) eventList[i] = new double [Ns];
	double checkPoint[Ntr];		// time increment that indicates 
								// to update stats for some stop

	int tripCount[Ns] = {0};
	int tripCurr[Ntr] = {0};

	const double double_max = static_cast<double>(INT_MAX);
	double dawn = df * Delta;	// time period to "collect" passengers for
								// the 1st trip
	double time = -dawn;		// simulation clock (seconds), initialized
								// to "dawn" seconds before the 1st trip
	bool stop = false;			// indicate if the simulation should stop
	double randNum = 0.0;

	/* ----- output first line (parameters) ----- */
	out << N << " " << Delta << " " << Tr << " " << DT << " ";
	out << Capb << " " << Hp << " " << Ns << " " <<  Nb << std::endl;

	/* ----- initialize the arrays ----- */
	for(int i = 0; i < Ns; i++)
	{
		for(int j = 0; j < Ns; j++)
		{
			comingCount[i][j] = 0;
		}
	}

	for(int i = 0; i < Ntr; i++)
	{
		checkPoint[i] = i * Delta;
	}

	for(int i = 0; i < Ns; i++)
	{
		if(i == 0)
		{
			// destinations: station 2 ~ Ns
			// schedule the first arriving passengers

			for(int j = 0; j < Ns - 1; j++)
			{
				randNum = static_cast<double>(rand()) / RAND_MAX;
				eventList[i][j] = time + expoDistro(lambda, randNum);
			}
			eventList[i][Ns - 1] = double_max;
		}
		else
		{
			// destinations: station (i + 2) ~ Ns, 1
			// schedule the first arriving passengers

			for(int j = 0; j < Ns - i; j++)
			{
				randNum = static_cast<double>(rand()) / RAND_MAX;
				eventList[i][j] = time + expoDistro(lambda, randNum);
			}
			for(int j = Ns - i; j < Ns; j++)
			{
				eventList[i][j] = double_max;
			}
		}
	}

	/* ----- start simulation ----- */
	int udIdx = findEarliestCheck(checkPoint, Ntr);
	while(!stop)
	{
		if(time >= checkPoint[udIdx])
		{
			/* --- update stats at check point --- */
			if(tripCount[tripCurr[udIdx]] < N)
			{
				updateStats(out, comingCount, tripCount, tripCurr[udIdx], Ns);
			}
			tripCount[tripCurr[udIdx]]++;
			tripCurr[udIdx] = (tripCurr[udIdx] + 1) % Ns;

			/* --- schedule next check point --- */
			if(tripCurr[udIdx] == 0)
			{
				checkPoint[udIdx] = checkPoint[(udIdx - 1 + Ntr) % Ntr] + Delta;
			}
			else
			{
				checkPoint[udIdx] += (Tr / Ns + DT);
			}

			/* --- remove stops from event scheduling --- */
			if(tripCount[Ns - 1] == N)
			{
				stop = true;
			}
			else if(tripCount[tripCurr[udIdx]] == N)
			{
				for(int i = 0; i < Ns; i++)
				{
					eventList[tripCurr[udIdx]][i] = double_max;
				}
			}
			else;
			
			/* --- find next check point --- */
			udIdx = findEarliestCheck(checkPoint, Ntr);
		}
		scheduleArrival(time, lambda, eventList, comingCount, Ns);
	}
	/* ----- simulation finished ----- */

	cout << "Simulation complete." << endl;

	out.close();
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
		for(int i = 0; i < L - d; i++)
		{
			out << tc[d] << " " << (d + 1) << " ";
			out << (i + d + 2) % L << " " << cc[d][i] << endl;
			cc[d][i] = 0;
		}
	}
}

void scheduleArrival(double & t, double mu, double ** el, int ** cc, int L)
{
	// L = Ns
	// cc = comingCount

	/* --- find earliest event --- */
	int idx[2] = {-1, -1};
	findEarliest(el, idx, L);
	++cc[idx[0]][idx[1]];
	
	/* --- update time --- */
	t = el[idx[0]][idx[1]];
	
	/* --- schedule next arrival --- */
	double rnd = static_cast<double>(rand()) / RAND_MAX;
	el[idx[0]][idx[1]] = t + expoDistro(mu, rnd);
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

int findEarliestCheck(double * ckpt, int L)
{
	// L = Ntr

	int min_idx = -1;
	double min = static_cast<double>(INT_MAX);
	for(int i = 0; i < L; i++)
	{
		if(ckpt[i] < min)
		{
			min = ckpt[i];
			min_idx = i;
		}
	}

	return min_idx;
}
