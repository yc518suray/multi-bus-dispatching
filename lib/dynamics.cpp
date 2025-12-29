// define the dynamics and functions

#include<vector>
#include<iostream>
#include<iomanip>
#include<climits>
#include<cstdlib>
#include<cmath>
#include"dynamics.h"

using namespace std;

static int debug_time = -1;	// debug timing
static int debug_count = 0;		// count timing of debugging
static bool lesszero = true;

/* ----- function declarations ----- */

void print3DArray(int *** A, int dim1, int dim2, int dim3, int d);
void initial3DArray(int *** & A, int dim1, int dim2, int dim3);
void clear3DArray(int *** & A, int dim1, int dim2, int dim3);
double uniform_rand();
int minimum(int a, int b, int c);

/* ----- function definitions ----- */

Env::Env()
{
	B = nullptr;
	V = nullptr;
	P = nullptr;
	f = nullptr;
	X = nullptr;
	choice = nullptr;

	Capb = 0;
	Ns = 0;
	Nb = 0;
	N = 0;
}

Env::~Env()
{
	for(int i = 0; i < Ns; i++)
	{
		for(int j = 0; j < Ns; j++)
		{
			delete [] B[i][j];
			delete [] V[i][j];
			delete [] P[i][j];
			delete [] f[i][j];
			delete [] X[i][j];
		}
		for(int j = 0; j < Nb; j++)
		{
			delete [] choice[i][j];
		}
		delete [] B[i];
		delete [] V[i];
		delete [] P[i];
		delete [] f[i];
		delete [] X[i];
		delete [] choice[i];
	}
	delete [] B;
	delete [] V;
	delete [] P;
	delete [] f;
	delete [] X;
	delete [] choice;
}

void Env::initialize(int nb, int ns, int n, int capb, int *** coming, double Delta)
{
	Capb = capb;
	Nb = nb;
	Ns = ns;
	N = n;
	delta = (int)Delta;

	/* ----- initialize B ----- */
	initial3DArray(B, Ns, Ns, Nb * N);

	/* ----- initialize V ----- */
	initial3DArray(V, Ns, Ns, Nb * N);

	/* ----- initialize P ----- */
	initial3DArray(P, Ns, Ns, N);

	/* ----- initialize X ----- */
	initial3DArray(X, Ns, Ns, N * (N - 1) / 2);

	/* ----- initialize choice ----- */
	initial3DArray(choice, Ns, Nb, 3);

	/* ----- initialize f ----- */
	f = new int ** [Ns];
	for(int i = 0; i < Ns; i++)
	{
		f[i] = new int * [Ns];
		for(int j = 0; j < Ns; j++)
		{
			f[i][j] = new int [N];
			for(int k = 0; k < N; k++) f[i][j][k] = coming[k][i][j];
		}
	}
}

void Env::printEverything(int opt)
{
	switch(opt)
	{
		case 1:
			cout << "print out B below:" << endl;
			print3DArray(B, Ns, Ns, N * Nb, Nb);
			break;
		case 2:
			cout << "print out V below:" << endl;
			print3DArray(V, Ns, Ns, N * Nb, Nb);
			break;
		case 3:
			cout << "print out P below:" << endl;
			print3DArray(P, Ns, Ns, N, 1);
			break;
		case 4:
			cout << "print out f below:" << endl;
			cout << "note: \'k\' should be replaced with \'k - 1\' below" << endl;
			print3DArray(f, Ns, Ns, N, 1);
			break;
		default:
			cout << "Nothing to print." << endl;
	}
	cout << "Print complete." << endl;
}

int Env::cost(int opt, vector<vector<int>> bus, double DT)
{
	// opt = 1 --> linear cost
	// opt = 2 --> nonlinear cost
	// bus	   --> bus dispatching indicator
	// DT	   --> dwell time at each stop (sec), constant
	// note: the bus door opening-closing time toc is assumed 0

	int Cd = 1; // weight of delay cost
	int Cv = 1; // weight of vacancy cost
	int Jd = 0; // linear/nonlinear delay cost
	int Jv = 0; // bus vacancy cost

	/* === debug === */
	//if(debug_count == debug_time)
	//{
	//	cout << "this dispatching vector: " << endl;
	//	for(int i = 0; i < N; i++)
	//	{
	//		cout << "trip " << i << ": ";
	//		for(int j = 0; j < Nb; j++)
	//		{
	//			cout << bus[i][j] << " ";
	//		}
	//		cout << endl;
	//	}
	//	cout << endl;
	//}
	/* === debug === */

	int dummy, bidx, boarding;
	for(int k = 0; k < N; k++)
	{
		/* --- for each trip --- */
		for(int i = 0; i < Ns; i++)
		{
			/* --- for each source --- */
			dummy = (i == 0)? (Ns - 1): (Ns - i);
			for(int j = 0; j < dummy; j++)
			{
				/* --- for each destination --- */
				/* --- update V --- */
				if(i > 0)
				{
					for(int s = 0; s < Nb; s++)
					{
						bidx = s + k * Nb;
						V[i][j][bidx] = V[i - 1][j + 1][bidx] + B[i - 1][j + 1][bidx];
						Jv += (Capb - V[i][j][bidx]); // equation (13)

						/* === debug === */
						if(debug_count == debug_time && V[i][j][bidx] < 0 && lesszero)
						{
							cout << "\nV < 0 at i, j, k = " << i << ", " << j << ", " << k
								 << ", bus = " << s << "\n\n";
							lesszero = false;
						}
						/* === debug === */
					}
				}
				/* --- update P --- */
				if(k == 0)
				{
					P[i][j][k] = f[i][j][k]; // no waiting at k - 1 = 0 trip
					if(opt == 1) Jd += P[i][j][k]; // equation (10)
				}
				else
				{
					boarding = 0;
					for(int s = 0; s < Nb; s++)
					{
						boarding += B[i][j][s + (k - 1) * Nb];
					}
					P[i][j][k] = P[i][j][k - 1] + f[i][j][k] - boarding;
					if(opt == 1) Jd += P[i][j][k]; // equation (10)
					
					/* === debug === */
					if(debug_count == debug_time && P[i][j][k] < 0 && lesszero)
					{
						cout << "\nP < 0 at i, j, k = " << i << ", " << j << ", " << k << "\n\n";
						lesszero = false;
					}
					/* === debug === */
				}
			}
			/* --- update B --- */
			int onbus[Nb] = {0}; // for use in equations (27)
			//double remains;		 // for use in equations (26) ~ (27)
			//double remains2;	 // for use in equations (28)
			//double rand_portion; // for use in equations (26) ~ (28)
			for(int j = 0; j < dummy; j++)
			{
				/* --- equations (26) --- */
				int num_bus = 0, count_bus = 0;
				for(int s = 0; s < Nb; s++)
				{
					if(bus[k][s] == 1) num_bus++;
				}
				//remains = 1.0;
				for(int s = 0; s < Nb; s++)
				{
					count_bus++;
					if(bus[k][s] == 1)
					{
						bidx = s + k * Nb;
						/* === debug, old method === */
						//if(count_bus < num_bus)
						//{
						//	rand_portion = uniform_rand();
						//}
						//else
						//{
						//	rand_portion = 1.0;
						//}
						//choice[j][s][0] = floor(P[i][j][k] * remains * rand_portion);
						//remains = remains * (1 - rand_portion);
						/* === debug === */

						choice[j][s][0] = floor(P[i][j][k] / static_cast<double>(num_bus));
						onbus[s] += V[i][j][bidx];
					}
				}
			}
			double MBF = DT / alpha; // maximum boarding flow limited by DT
			for(int s = 0; s < Nb; s++)
			{
				//remains = 1.0;
				//remains2 = 1.0;

				if(bus[k][s] == 1)
				{
					for(int j = 0; j < dummy; j++)
					{
						/* --- equations (27) --- */
						/* === debug, old method === */
						//if(j < dummy - 1)
						//{
						//	rand_portion = uniform_rand();
						//}
						//else
						//{
						//	rand_portion = 1.0;
						//}
						//choice[j][s][1] = floor((Capb - onbus[s]) * remains * rand_portion);
						//remains = remains * (1 - rand_portion);
						/* === debug === */

						choice[j][s][1] = floor((Capb - onbus[s]) / static_cast<double>(dummy));

						/* --- equations (28) --- */
						/* === debug, old method === */
						//if(j < dummy - 1)
						//{
						//	rand_portion = uniform_rand();
						//}
						//else
						//{
						//	rand_portion = 1.0;
						//}
						//choice[j][s][2] = floor(MBF * remains2 * rand_portion);
						//remains2 = remains2 * (1 - rand_portion);
						/* === debug === */

						choice[j][s][2] = floor(MBF / static_cast<double>(dummy));
					}
				}
			}

			/* === debug === */
			//if(debug_count == debug_time)
			//{
			//	cout << "\n\nk = " << k << ", i = " << i << endl;
			//	for(int j = 0; j < dummy; j++)
			//	{
			//		cout << "j = " << j << ", ";
			//		cout << "choice 0: ";
			//		for(int s = 0; s < Nb; s++) cout << choice[j][s][0] << " ";
			//		cout << "choice 1: ";
			//		for(int s = 0; s < Nb; s++) cout << choice[j][s][1] << " ";
			//		cout << "choice 2: ";
			//		for(int s = 0; s < Nb; s++) cout << choice[j][s][2] << " ";
			//		cout << endl;
			//	}
			//}
			/* === debug === */

			/* --- equation (29) --- */
			for(int j = 0; j < dummy; j++)
			{
				for(int s = 0; s < Nb; s++)
				{
					if(bus[k][s] == 1)
					{
						bidx = s + k * Nb;
						B[i][j][bidx] = minimum(choice[j][s][0], choice[j][s][1], choice[j][s][2]);
						
						/* === debug === */
						if(debug_count == debug_time && B[i][j][bidx] < 0 && lesszero)
						{
							cout << "\nB < 0 at i, j, k = " << i << ", " << j << ", " << k
								 << ", bus = " << s << "\n\n";
							lesszero = false;
						}
						/* === debug === */
					}
				}
			}
			clear3DArray(choice, Ns, Nb, 3);
		}
	}
	
	/* === debug === */
	int what = 0;
	if(debug_count == debug_time)
	{
		cout << "\nenter option to printEverything (1 ~ 4, enter -1 to leave debug): ";
		while(cin >> what)
		{
			if(what == -1)
			{
				debug_count++;
				break;
			}
			else
			{
				this -> printEverything(what);
			}
			cout << "\nenter option to printEverything (1 ~ 4, enter -1 to leave debug): ";
		}
	}
	else debug_count++;
	/* === debug === */

	if(opt == 1) Jd *= delta;
	if(opt == 2)
	{
		/* --- calculate perceived waiting time --- */
		for(int i = 0; i < Ns; i++)
		{
			dummy = (i == 0)? (Ns - 1): (Ns - i);
			for(int j = 0; j < dummy; j++)
			{
				/* --- equation (11) --- */
				int a, b, c, tmp, jump_idx;
				for(int k = 0; k < N - 1; k++)
				{
					a = 0; b = 0; c = 0; tmp = 0;
					for(int s = 0; s < Nb; s++)
					{
						bidx = s + k * Nb;
						tmp = tmp + B[i][j][bidx];
						if(k > 0) a -= B[i][j][bidx];
					}
					for(int r = k + 1; r < N; r++)
					{
						for(int s = 0; s < Nb; s++)
						{
							bidx = s + r * Nb;
							tmp = tmp + B[i][j][bidx];
						}
						if(k > 0)
						{
							a += P[i][j][k - 1];
							c += f[i][j][k];
						}
						b = (tmp > a)? (tmp - a): 0;
						jump_idx = r - k - 1 + N * k - k * (k - 1) / 2;
						X[i][j][jump_idx] = (c > b)? (c - b): 0;
						if(r > k + 1)
						{
							Jd += (r - k - 1) * (r - k - 1) *\
								  (X[i][j][jump_idx] - X[i][j][jump_idx - 1]);
						}
					}
				}
			}
		}
	}

	int totalCost = Cd * Jd + Cv * Jv;
	this -> clear();

	if(debug_count == debug_time + 1)
	{
		cout << "Jd = " << Jd << ", Jv = " << Jv << endl;
	}

	return totalCost;
}

void Env::clear()
{
	// clear B, V, P, but not f
	
	clear3DArray(B, Ns, Ns, Nb * N);
	clear3DArray(V, Ns, Ns, Nb * N);
	clear3DArray(P, Ns, Ns, N);
	clear3DArray(X, Ns, Ns, N * (N - 1) / 2);
	clear3DArray(choice, Ns, Nb, 3);
}

/* ----- non-member functions ----- */

void initial3DArray(int *** & A, int dim1, int dim2, int dim3)
{
	A = new int ** [dim1];
	for(int i = 0; i < dim1; i++)
	{
		A[i] = new int * [dim2];
		for(int j = 0; j < dim2; j++)
		{
			A[i][j] = new int [dim3];
			for(int k = 0; k < dim3; k++) A[i][j][k] = 0;
		}
	}
}

void clear3DArray(int *** & A, int dim1, int dim2, int dim3)
{
	for(int i = 0; i < dim1; i++)
	{
		for(int j = 0; j < dim2; j++)
		{
			int * tmp = A[i][j];
			for(int k = 0; k < dim3; k++) tmp[k] = 0;
		}
	}
}

void print3DArray(int *** A, int dim1, int dim2, int dim3, int d)
{
	// d = total number of buses

	for(int i = 0; i < dim1; i++)
	{
		int x = (i == 0)? (dim2 - 1): (dim2 - i);
		for(int j = 0; j < x; j++)
		{
			int y = (i == 0)? (i + j + 2): (i + j + 1) % dim2 + 1;
			for(int k = 0; k < (dim3 / d); k++)
			{
				cout << "i =" 	<< setw(4) << i + 1;
				cout << ", j =" << setw(4) << y;
				cout << ", k =" << setw(4) << k + 1;
				cout << ", value =";
				if(d != 1)
				{
					for(int l = 0; l < d; l++)
					{
						cout << setw(4) << A[i][j][l + k * d];
						if(l < d - 1) cout << ",";
					}
				}
				else
				{
					cout << setw(4) << A[i][j][k];
				}
				cout << endl;
			}
		}
	}
}

double uniform_rand()
{
	return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

int minimum(int a, int b, int c)
{
	int x = INT_MAX;
	if(a < x) x = a;
	if(b < x) x = b;
	if(c < x) x = c;
	
	return x;

}
