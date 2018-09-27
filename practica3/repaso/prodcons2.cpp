#include <mpi.h>
#include <iostream>
#include <math.h>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"

/*
#define Productor    5
#define Consumidor   0
*/
#define Buffer       8
#define ITERS       	45
#define TAM          4

using namespace std;

// ---------------------------------------------------------------------

void productor(int rank)
{
   int value ;  
   
// 45 iteracciones entre 3 productores = a 15 iteraciones por productro

   for ( unsigned int i=1; i <= 15 ; i++ )
   { 
      value = i ;
      cout << "Productor " << rank << " produce valor " << value << endl << flush ;
      
      // espera bloqueado durante un intervalo de tiempo aleatorio 
      // (entre una décima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );
      
      // enviar 'value'
      MPI_Ssend( &value, 1, MPI_INT, Buffer, 0, MPI_COMM_WORLD );
   }
}
// ---------------------------------------------------------------------

void buffer()
{
   int        value[TAM] , 
              peticion , 
              pos  =  0,
              rama ;
   MPI_Status status ;
	int datosRecibidos=0;
   
   for( unsigned int i=0 ; i < ITERS*2 ; i++ ){  
		if (datosRecibidos==10){
			for(int i=1; i<=pos; i++){
				 MPI_Recv( &peticion, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status );
				cout << "Buffer envía " << value[pos-1] << " a Consumidor " << status.MPI_SOURCE << endl << flush;  
            MPI_Ssend( &value[pos-1], 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			}
				datosRecibidos = 0;		
		}

      		else{ 
			if ( pos==0 ) {     // el consumidor no puede consumir
         		rama = 0 ;      
  			}

      			else{
				if (pos==TAM){ // el productor no puede producir
         				rama = 1 ; 
				}          
      				else {             // ambas guardas son ciertas

         // leer 'status' del siguiente mensaje (esperando si no hay)
        		 		MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
         // calcular la rama en función del origen del mensaje
         	 			if ( status.MPI_TAG == 0 ){
            				rama = 0 ; 
				 	}
         	 			else{ 
            					rama = 1 ;
					 }
      				}
			}
      switch(rama)
      {
         case 0:
            MPI_Recv( &value[pos], 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status );
            cout << "Buffer recibe " << value[pos] << " de Productor " << status.MPI_SOURCE << endl << flush;  
            pos++;
	    datosRecibidos ++;
            break;
         case 1:
            //cout << "Buffer recibe de Consumidor " << status.MPI_SOURCE << " una peticion. " << endl << flush;  
            MPI_Recv( &peticion, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status );
				cout << "Buffer envía " << value[pos-1] << " a Consumidor " << status.MPI_SOURCE << endl << flush;  
            MPI_Ssend( &value[pos-1], 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            pos--;
            break;
      }     
   }
} 
}  
   
// ---------------------------------------------------------------------

void consumidor(int rank)
{
   int         value,
               peticion = 1 ; 
   MPI_Status  status ;
 
// 45 iteracciones entre 5 consumidores = 9 iteracciones recibidas por cada consumidor

   for (unsigned int i=0; i<10; i++)
   {
      cout << "Consumidor " << rank << " hace una peticion al Buffer " << endl << flush ;
      MPI_Ssend( &peticion, 1, MPI_INT, Buffer, 1/*Consumidor*/, MPI_COMM_WORLD ); 
      MPI_Recv ( &value, 1,    MPI_INT, Buffer, 0/*Productor*/, MPI_COMM_WORLD,&status );
      cout << "Consumidor " << rank << " recibe valor " << value << " de Buffer " << endl << flush ;
      
      // espera bloqueado durante un intervalo de tiempo aleatorio 
      // (entre una décima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );
      
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
   if ( size != 9 ) 
   {
      cout<< "El numero de procesos debe ser 9 "<<endl;
      return 0;
   } 
   
   // verificar el identificador de proceso (rank), y ejecutar la
   // operación apropiada a dicho identificador
   if ( rank > 4 && rank < 8) 
      productor(rank);
   else if ( rank == 8) 
      buffer();
   else 
      consumidor(rank);
   
   // al terminar el proceso, finalizar MPI
   MPI_Finalize( );
   return 0;
}

