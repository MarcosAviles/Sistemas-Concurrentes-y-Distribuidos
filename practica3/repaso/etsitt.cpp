#include <mpi.h>
#include <iostream>
#include <math.h>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"


#define Controlador    12
#define Mostrador      2
#define Alumno   3

using namespace std;

// ---------------------------------------------------------------------

void alumno(int rank)
{
   int value ;  
   MPI_Status status ;
   while(true)
   { 
      value = i+1;
      cout << "El alumno "<< rank << "pide solicitud al controlador" << endl << flush ;
      MPI_Ssend( &value, 1, MPI_INT, Controlador, 0, MPI_COMM_WORLD );
      cout << "El alumno "<< rank << "recibe solicitud aceptada del controlador" << endl << flush ;
      MPI_Recv( &value, 1, MPI_INT, Controlador, 1, MPI_COMM_WORLD, &status);
      cout << "El alumno "<< rank << "envia solicitud al mostrador" << endl << flush ;
      MPI_Ssend( &value, 1, MPI_INT, Mostrador, 2, MPI_COMM_WORLD );
      cout << "El alumno "<< rank << "recibe solicitud del mostrador" << endl << flush ;
      MPI_Recv( &value, 1, MPI_INT, Mostrador, 5, MPI_COMM_WORLD, &status);
      cout << "El alumno "<< rank << "envia al controlador que ha terminado su tramite" << endl << flush ;
      MPI_Ssend( &value, 1, MPI_INT, Controlador, 4, MPI_COMM_WORLD );
      usleep( 1000U * (100U+(rand()%900U)) );
   }
}
// ---------------------------------------------------------------------

void controlador(){ 
     int value;
     int pos=0;
     MPI_Status status ;
     bool vector[3]=;
     while(true){
	MPI_Recv( &value, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
	if(pos<3){
		MPI_Ssend( &value, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD );
		pos++;
	}
	else{
		
	}
     }
}
   
// ---------------------------------------------------------------------

void mostrador(int rank)
{

   MPI_Status  status ;
   int value;
   while(true){
      cout << "El mostrador " << rank<< " recibe del alumno" << endl << flush ;
      MPI_Recv ( &value, 1, MPI_INT, alumno, 2, MPI_COMM_WORLD,&status ); 
      usleep( 1000U * (100U+(rand()%900U)) );
      cout << "El mostrador " << rank<< " envia finalizacion del tramite " << endl << flush ;
      MPI_Ssend( &value, 1, MPI_INT, status.MPI_SOURCE, 5, MPI_COMM_WORLD );
      
   }
}
// ---------------------------------------------------------------------

int main(int argc, char *argv[]) 
{
   int rank,size; 
   
   // inicializar MPI, leer identif. de proceso y número de procesos
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   // inicializa la semilla aleatoria:
   srand ( time(NULL) );
   
   // comprobar el número de procesos con el que el programa 
   // ha sido puesto en marcha (debe ser 3)
   if ( size != 12 ) 
   {
      cout<< "El numero de procesos debe ser 12 "<<endl;
      return 0;
   } 
   
   // verificar el identificador de proceso (rank), y ejecutar la
   // operación apropiada a dicho identificador
   if ( rank < 3  ) 
      mostrador(rank);
   else if ( rank == 12 ) 
      controlador();
   else 
      alumno(rank);
   
   // al terminar el proceso, finalizar MPI
   MPI_Finalize( );
   return 0;
}

