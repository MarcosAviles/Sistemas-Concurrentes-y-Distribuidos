#include "mpi.h"
#include <iostream>

using namespace std;

void Productor(){
	int valor=100;
	MPI_Send(&valor,1,MPI_INT,1,0,MPI_COMM_WORLD);
	cout << "Se ha enviado el mensaje " <<endl;
	cout << "---------------------------------------------------------"<<endl;
}

void Consumidor(){
	MPI_Status status;
	int valor;

	MPI_Recv(&valor,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
	cout << "Se ha recibido el valor " << valor << " de " << status.MPI_SOURCE << " con etiqueta " << status.MPI_TAG <<endl;


}

int main(int argc, char *argv[]){
	int rank; // Identificador del proceso
	int size; // Numero de procesos.
	MPI_Status status;
	int value;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	if(rank==0)
		Productor();
	else if(rank==1)
		Consumidor();
	
	MPI_Finalize();
}
