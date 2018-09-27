#include "mpi.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
	int rank; // Identificador del proceso
	int size; // Numero de procesos.
	MPI_Status status;
	int value;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if(rank == 0){	// Si el proceso es el 0 genera el valor 100 y se lo envia al proceso1
		value=100;
		MPI_Send(&value, 1 ,MPI_INT, 1, 0 ,MPI_COMM_WORLD);
	}
	else{ 
		MPI_Recv(&value, 1 ,MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		cout << "Se ha recibido correctamente el mensaje de " << status.MPI_SOURCE << " con etiqueta " << status.MPI_TAG <<endl;
	}

	MPI_Finalize();
}
