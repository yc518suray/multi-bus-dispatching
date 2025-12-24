// define the dynamics and functions

#include<vector>
#include<iostream>
#include<iomanip>
#include"dynamics.h"

using namespace std;

/* ----- function declarations ----- */

void print3DArray(int *** A, int dim1, int dim2, int dim3, int d);
void initial3DArray(int *** & A, int dim1, int dim2, int dim3);
void clear3DArray(int *** & A, int dim1, int dim2, int dim3);

/* ----- function definitions ----- */

Env::Env()
{
	B = nullptr;
	V = nullptr;
	P = nullptr;
	f = nullptr;

	Capb = 0;
	Ns = 0;
	Nb = 0;
	N = 0;
	bus_index = 0;
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
		}
		delete [] B[i];
		delete [] V[i];
		delete [] P[i];
		delete [] f[i];
	}
	delete [] B;
	delete [] V;
	delete [] P;
	delete [] f;
}

void Env::initialize(int nb, int ns, int n, int capb, int *** coming)
{
	Capb = capb;
	Nb = nb;
	Ns = ns;
	N = n;

	/* ----- initialize B ----- */
	initial3DArray(B, Ns, Ns, Nb * N);

	/* ----- initialize V ----- */
	initial3DArray(V, Ns, Ns, Nb * N);

	/* ----- initialize P ----- */
	initial3DArray(P, Ns, Ns, N);

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
			print3DArray(f, Ns, Ns, N, 1);
			break;
		default:
			cout << "nothing to print." << endl;
	}
	cout << "print complete." << endl;
}

int Env::cost(int opt, int headway, vector<vector<int>> bus)
{
	// opt = 1 --> linear cost
	// opt = 2 --> nonlinear cost
	// bus	   --> bus dispatching indicator

	int Cd = 1; // weight of delay cost
	int Cv = 1; // weight of vacancy cost
	int Jd = 0; // linear/nonlinear delay cost
	int Jv = 0; // bus vacancy cost

	int dummy, boarding_k;
	for(int k = 0; k < N; k++)
	{
		for(int i = 0; i < Ns; i++)
		{
			dummy = (i == 0)? (Ns - 1): (Ns - i);
			for(int j = 0; j < dummy; j++)
			{
				if(k == 0)
				{
					// update P
					P[i][j][k] = f[i][j][k];

					for(int s = 0; s < Nb; s++)
					{
						if(bus[k][s] == 1)
						{
							// update B
							B[i][j][k + s * Nb] = ;
							
							// update V
							V[i][j][k + s * Nb] = ;
						}
					}
				}
				else
				{
					// update P
					boarding_k = 0;
					for(int s = 0; s < Nb; s++)
					{
						boarding_k += B[i][j][k - 1];
					}
					P[i][j][k] = P[i][j][k - 1] + f[i][j][k] - boarding_k;

					for(int x: bus[k])
					{
						if(x == 1)
						{
							// update B
							// update V
						}
					}
				}
			}
		}
	}

	int totalCost = Cd * Jd + Cv * Jv; 
	return totalCost;
}

void Env::clear()
{
	// clear B, V, P, but not f
	
	clear3DArray(B, Ns, Ns, Nb * N);
	clear3DArray(V, Ns, Ns, Nb * N);
	clear3DArray(P, Ns, Ns, N);
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
	// if there's only 1 bus, ambiguity arises

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
						cout << setw(4) << A[i][j][k + l * (dim3 / d)];
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
