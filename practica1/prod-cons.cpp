#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>

using namespace std ;

// ---------------------------------------------------------------------
// constantes 
const unsigned 
  num_items  = 40 ,
  tam_vector = 10 ;              

// ---------------------------------------------------------------------
// Variables globales
int vector[tam_vector];
int primera_ocupada=0, primera_libre=0;
sem_t puede_escribir, puede_leer;

unsigned producir_dato()
{
  static int contador = 1 ;
  cout << "producido: " << contador << endl << flush ;
  return contador++ ;
}
// ---------------------------------------------------------------------

void consumir_dato( int dato )
{
    cout << "dato recibido: " << dato << endl ;
}
// ---------------------------------------------------------------------

void * productor( void * )
{   
  for( unsigned i = 0 ; i < num_items ; i++ )
  { 
    	int dato = producir_dato() ;
	// Bloqueamos al productor pero como esta inicializado el semaforo en 1, la primera interaccion
	// la ejecutará y las siguientes será bloqueado
	sem_wait(&puede_escribir);
	
	vector[primera_libre]=dato; // Almacenamos el dato producido en el vector [posicion libre]

	// desbloqueamos la hebra de consumidor
	sem_post(&puede_leer);
	primera_libre=(primera_libre+1)%tam_vector; // incrementamos la variable sin pasar del tamanio del vector
    	

  }
  return NULL ;
}
// ---------------------------------------------------------------------

void * consumidor( void * )
{   
  for( unsigned i = 0 ; i < num_items ; i++ )
  {   
    int dato ;
	
	sem_wait(&puede_leer);
	dato=vector[primera_ocupada]; //Recuperamos el dato del vector en la posicion indicada
	primera_ocupada=(primera_ocupada+1)%tam_vector; // Incrementamos la variable primera_ocupada para que tampoco se pase del tamanio del vector
	
	// desbloqueamos al productor
	sem_post(&puede_escribir);
	consumir_dato( dato ) ;
	
  }
  return NULL ;
}
//----------------------------------------------------------------------

int main()
{
//--------------------------------------------------------------
// Declaracion de hebras
	pthread_t hebra1, hebra2;
//--------------------------------------------------------------
// Inicializamos los semáforos
	sem_init (&puede_escribir, 0, tam_vector); // puede escribir lo inicializamos en el tamanio
	// del vector para que pueda producir hasta tantos datos como tamanio hayamos creado el vector
	sem_init (&puede_leer, 0, 0); // al contrario que el anterior puede leer lo inicializamos en 0 para que no pueda leer 

//----------------------------------------------------------------------

	pthread_create(&hebra1,NULL,productor,NULL); // lanzamos la primera hebra
	pthread_create(&hebra2,NULL,consumidor,NULL); // lanzamos la segunda hebra

	pthread_join( hebra1, NULL ) ; // realizamos la espera de la primera hebra
	pthread_join( hebra2, NULL ) ; // realizamos la espera de la segunda hebra
	
	sem_destroy (&puede_escribir); // eliminamos memoria para los semaforos
	sem_destroy (&puede_leer); // eliminamos los semaforos para limpiar memoria
   return 0; 
}
