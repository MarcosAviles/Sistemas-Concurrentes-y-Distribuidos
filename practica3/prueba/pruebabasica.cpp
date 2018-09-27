#include "mpi.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
	int rank; // Identificador del proceso
	int size; // Numero de procesos.

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	cout << "Hola desde proceso " <<rank << " de " << size <<endl;

	MPI_Finalize();
	
}
