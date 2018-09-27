#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

using namespace std;

void Filosofo( int id, int nprocesos);
void Tenedor ( int id, int nprocesos);
void Camarero();

// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int rank, size;
   
   srand(time(0));
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   if( size!=11)
   {
      if( rank == 0) 
         cout<<"El numero de procesos debe ser 11" << endl << flush ;
      MPI_Finalize( ); 
      return 0; 
   }
   
   if ((rank%2) == 0) {
	if(rank==10){
		Camarero();
	}
	else{
    		Filosofo(rank,size-1); // Los pares son Filosofos 
	}
   }
   else {
    	Tenedor(rank,size);  // Los impares son Tenedores 
   }
   
   MPI_Finalize( );
   return 0;
}  
// ---------------------------------------------------------------------

void Filosofo( int id, int nprocesos )
{
   int izq = (id+1) % nprocesos;
   int der = ((id+nprocesos)-1) % nprocesos;   
   
   while(1)
   {
	
	// Piensa (espera bloqueada aleatorio del proceso)
      cout << "Filosofo " << id << " PENSANDO" << endl << flush;
      
      // espera bloqueado durante un intervalo de tiempo aleatorio 
      // (entre una décima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );

	

      // Solicita sentarse al camarero
      cout << "Filosofo "<<id<< " solicita al camarero sentarse en la mesa..." << endl << flush;
      MPI_Ssend( &izq, 1, MPI_INT, 10, 2, MPI_COMM_WORLD );
      // ...
      // Solicita tenedor izquierdo
      cout << "Filosofo "<<id<< " solicita tenedor izq ..." << izq << endl << flush;
      MPI_Ssend( &izq, 1, MPI_INT, izq, 0, MPI_COMM_WORLD );
      // ...

      // Solicita tenedor derecho
      cout <<"Filosofo "<<id<< " coge tenedor der ..." << der << endl << flush;
      MPI_Ssend( &der, 1, MPI_INT, der, 0, MPI_COMM_WORLD );
      // ...
  
      cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
      sleep((rand() % 3)+1);  //comiendo
      
      // Suelta el tenedor izquierdo
      cout <<"Filosofo "<<id<< " suelta tenedor izq ..." << izq << endl << flush;
      MPI_Ssend( &izq, 1, MPI_INT, izq, 1, MPI_COMM_WORLD );
      // ...
      
      // Suelta el tenedor derecho
      cout <<"Filosofo "<<id<< " suelta tenedor der ..." << der << endl << flush;
      MPI_Ssend( &der, 1, MPI_INT, der, 1, MPI_COMM_WORLD );
      // ...
      

	// Pide permiso para levantarse
    cout << "Filosofo "<<id<< " solicita al camarero levantarse de la mesa..." << endl << flush;
    MPI_Send( &izq, 1, MPI_INT, 10, 3, MPI_COMM_WORLD );
      
 }
}
// ---------------------------------------------------------------------

void Tenedor(int id, int nprocesos)
{
  int buf; 
  MPI_Status status; 
  int Filo;
  
  while( true )
  {
    // ...
    
    // Recibe la peticion del filosofo ...
    MPI_Recv( &Filo, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status );
    // ...
    
    cout << "Ten. " << id << " recibe petic. de " << Filo << endl << flush;
    
    // Espera a que el filosofo suelte el tenedor...
    MPI_Recv( &Filo, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD, &status );
    // ...
    cout << "Ten. " << id << " recibe liberac. de " << Filo << endl << flush; 
  }
}
// ---------------------------------------------------------------------

void Camarero()
{
   int contador=0;
   MPI_Status status;
   int aux, rama;
   while(true)
   {  
      if ( contador==0 )      // el filosofo puede sentarse
         rama = 0 ;        
      else if (contador==4) // el filosofo no se puede sentar y debe levantarse un filosofo
         rama = 1 ;           
      else               // ambas guardas son ciertas
      {
         // leer 'status' del siguiente mensaje (esperando si no hay)
         MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
         
         // calcular la rama en función del origen del mensaje
         if ( status.MPI_TAG == 2 ) 
            rama = 0 ; 
         else 
            rama = 1 ;
      }
      switch(rama)
      {
         case 0:
            MPI_Recv( &aux, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status );
	    cout << "El camarero recibe la petición de sentarse el Fisolofo: " <<status.MPI_SOURCE<< endl << flush;
            contador++;
            break;
         case 1:
            MPI_Recv( &aux, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status );
	    cout << "El camarero recibe la petición de levantarse el Fisolofo: " <<status.MPI_SOURCE<< endl << flush;
            contador--;
            break;
      }     
   }
}























