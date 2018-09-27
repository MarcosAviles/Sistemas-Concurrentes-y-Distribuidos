#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>      // incluye "time(....)"
#include <unistd.h>    // incluye "usleep(...)"
#include <stdlib.h>    // incluye "rand(...)" y "srand"

using namespace std;

sem_t semaforos[3], puede_producir;
unsigned interacciones=5;

 // fumador1  -> PT; fumador2 -> PC; fumador3 -> TC;
// ----------------------------------------------------------------------------
// función que simula la acción de fumar  como un retardo aleatorio de la hebra

void fumar() 
{ 
   //  inicializa la semilla aleatoria  (solo la primera vez)
   static bool primera_vez = true ;
   if ( primera_vez )
   {   primera_vez = false ;
      srand( time(NULL) );    
   }
   
   // calcular un numero aleatorio de milisegundos (entre 1/10 y 2 segundos)
   const unsigned miliseg = 100U + (rand() % 1900U) ; 

   // retraso bloqueado durante 'miliseg' milisegundos
   usleep( 1000U*miliseg ); 
	
}
// ----------------------------------------------------------------------------
// En la función producir vamos a tomar los valores (1=Papel, 2=Tabaco, 3=Cerrilas) que es llamada
// por el productor(estanquero) en cada interacción
unsigned producir()
{
	const unsigned suministro = rand()%3+1;  // Producimos un numero aleatorio del 0 al 2 y le sumamos uno

// Aqui deducimos dependiendo del numero producido aleatorio que tipo de producto se va a producir y devolvemos el numero producido
	if (suministro==1){
		cout<<"El Estanquero ha producido Papel"<< endl <<flush;
	}
	if(suministro==2){
		cout<<"El Estanquero ha producido Tabaco"<< endl <<flush;
	}
	else{
		cout<<"El Estanquero ha producido Cerillas"<< endl <<flush;
	}
	return suministro;
}

// Esta funcion productor es la que se van a encargar de ejecutar la hebra estanquero que llama a la 
// producir, desbloquea al fumador correspondiente, y se bloquea el productor(estanquero)
void * productor( void * )
{   
  for( unsigned i = 0 ; i < interacciones ; i++ )
  { 
	// Este semáforo bloquearia al productor, pero el semaforo esta inicializado a 1 por lo que en
	// la primera interración se ejecuta directamente, en posteriores interacciones sera bloqueado
	sem_wait(&puede_producir);
    	int dato = producir();
	if(dato==1){
		sem_post(&semaforos[2]);
		 
	}
	if(dato==2){
		sem_post(&semaforos[1]);
		
	}
	else{
		sem_post(&semaforos[0]);
	}
  }
	
  
  return NULL ;
}

// Esta es la funcion del consumidor(fumador) que ejecutara cada hebra de fumador

void * consumidor( void * ih_indice){
	// Primero hacemos un casting del indice de la hebra recibido por parametro, así ya sabremos
	// a que fumador pertenece
	unsigned long ih = (unsigned long) ih_indice ;

	// Realizamos un bucle infinito
	while(true){	
		// Cuando se lance la hebra del fumador, llegará hasta aqui ya que se bloqueara por el
		// semaforo inicializado en 0, esperando a que el estanquero produzca un producto y 
		// desbloquee la hebra correspondiente
		sem_wait(&semaforos[ih]);
		cout<<"El fumador: " << ih+1 << "se va a liar el cigarrillo y va a fumar"<< endl;
		// La llamada a esta funcion hace que se duerma el proceso durante un tiempo
		fumar();
		cout<<"El fumador: " << ih+1 << " ha terminado de fumar"<< endl;
		// Desbloqueamos al productor para que pueda producir
		sem_post(&puede_producir);
	}
	return NULL;
}


// ----------------------------------------------------------------------------

int main()
{
	// Declaracion de un vector de hebras para los fumadores
	pthread_t hebras[3];
	// Declaracion de la hebra para el estanquero
	pthread_t estanquero;

	// Inicializamos los semáforos	
	sem_init (&puede_producir, 0, 1);
	for(int i=0; i<3; i++){
		sem_init (&semaforos[i], 0, 0);
	}

	// Lanzamos las hebras
	pthread_create(&estanquero,NULL,productor,NULL); // // lanzamos la hebra del estanquero
	for (int i=0; i<3; i++){
		pthread_create(&hebras[i],NULL,consumidor,(void *)i); // lanzamos la hebras de los fumadores
	}
	

	// Realizamos la espera de la hebra de estanquero para su finalizacion
	// no realizamos lo mismo para los fumadores ya que como he realizado un bucle infinito en la 
	// funcion del consumidor, vamos ha cancelar las hebras de los fumadores cuando el estanquero
	// finalize y no tener que realizar un ctrl+c para matar el programa
	pthread_join( estanquero, NULL ) ;

	// Aqui cancelamos las hebras de los fumadores cuando el estanquero ya ha finalizado
	for (int i=0; i<3; i++){
		pthread_cancel(hebras[i]);
	}
	

	// Eliminamos los semáforos de memoria para liberar memoria
	sem_destroy (&puede_producir);
	for (int i=0; i<3; i++){
		sem_destroy (&semaforos[i]);
	}


  return 0 ;
}
