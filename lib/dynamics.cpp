// define the dynamics and functions

#include<vector>
#include<set>
#include<iostream>
#include<iomanip>
#include"dynamics.h"

using namespace std;

/* ----- function declarations ----- */

void print3DArray(int *** A, int dim1, int dim2, int dim3, int d);

/* ----- function definitions ----- */

void Env::initialize(int Nb, int Ns, int N, int capb, int *** coming)
{
	Capb = capb;

	/* ----- initialize B ----- */
	B = new int ** [Ns];
	for(int i = 0; i < Ns; i++)
	{
		B[i] = new int * [Ns];
		for(int j = 0; j < Ns; j++)
		{
			B[i][j] = new int [Nb * N];
			for(int k = 0; k < Nb * N; k++) B[i][j][k] = 0;
		}
	}

	/* ----- initialize V ----- */
	V = new int ** [Ns];
	for(int i = 0; i < Ns; i++)
	{
		V[i] = new int * [Ns];
		for(int j = 0; j < Ns; j++)
		{
			V[i][j] = new int [Nb * N];
			for(int k = 0; k < Nb * N; k++) V[i][j][k] = 0;
		}
	}

	/* ----- initialize P ----- */
	P = new int ** [Ns];
	for(int i = 0; i < Ns; i++)
	{
		P[i] = new int * [Ns];
		for(int j = 0; j < Ns; j++)
		{
			P[i][j] = new int [N];
			for(int k = 0; k < N; k++) P[i][j][k] = 0;
		}
	}

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
	char option;
	switch(opt)
	{
		case 1:
			cout << "print out B below:" << endl;
			print3DArray(B, Ns, Ns, N, Nb);
			break;
		case 2:
			option = 'V';
			cout << "print out V below:" << endl;
			print3DArray(V, Ns, Ns, N, Nb);
			break;
		case 3:
			option = 'P';
			cout << "print out P below:" << endl;
			print3DArray(P, Ns, Ns, N, 0);
			break;
		case 4:
			option = 'f';
			cout << "print out f below:" << endl;
			print3DArray(f, Ns, Ns, N, 0);
			break;
	}
	cout << "print complete." << endl;
}

void Env::cost(int opt)
{
}

void print3DArray(int *** A, int dim1, int dim2, int dim3, int d)
{
	for(int i = 0; i < dim1; i++)
	{
		for(int j = 0; j < dim2; j++)
		{
			for(int k = 0; k < (dim3 / d); k++)
			{
				cout << setw(4) << i << setw(4) << j << setw(4) << k;
				if(d != 0)
				{
					for(int l = 0; l < d; l++)
					{
						cout << setw(4) << A[i][j][l + k * d];
						if(l < d - 1) cout << ",";
					}
				}
				else
				{
				}
			}
		}
	}
}
