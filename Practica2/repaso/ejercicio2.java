 import monitor.*; // we import the package monitor
 import java.util.Random ; // we import the packae random to generate aleatory numbers
 
 
 class Bareto extends AbstractMonitor
{ 
	// Declaration of variables
	
	private int copa1 = 0 ;// vale 0 si no hay copas del tipo 1
	private int copa2 = 0 ;//  " "   " """  " " " " " " " " " 0
	
	// Declaration of conditions
	private Condition sem_servir = makeCondition();
	private Condition[] sem_copa = new Condition[2]; // one for each smoker
	
	
	public Bareto(){
		for(int i = 0 ; i < 2 ; i++){
			sem_copa[i] = makeCondition();
		}
	}
			
	
	// invocado por cada fumador, indicando su ingrediente o numero
	public void pedir( int tipo_copa, int cod_alumno ){ 
		enter();
			// si la copa es del tipo 1
			if(tipo_copa == 1){ /// 1 es el primer tipo de copa
				if(copa1 == 0 ){ // no quedan copas de este tipo
				System.out.println("\033[32m" + "Alumno"+cod_alumno+" esperando copa del tipo" + tipo_copa + "\033[0m");
				sem_copa[0].await(); // este alumno tiene que esperar que pongan mas copas de su tipo
				 }
			 sem_servir.signal();  // El camarero pondra 3 copas de cada tipo
			System.out.println("\033[32m" + "Alumno"+cod_alumno+" consumiento copa del tipo" + tipo_copa + "\033[0m");
			copa1--; // quitamos una bebida
			}
			// si la copa es del tipo 2
			else if(tipo_copa == 2){ /// 1 es el primer tipo de copa
				if(copa2 == 0 ){ // no quedan copas de este tipo
				System.out.println("\033[32m" + "Alumno"+cod_alumno+" esperando copa del tipo" + tipo_copa + "\033[0m");
				sem_copa[1].await(); // este alumno tiene que esperar que pongan mas copas de su tipo
				 }
			 sem_servir.signal();  // El camarero pondra 3 copas de cada tipo
			System.out.println("\033[32m" + "Alumno"+cod_alumno+" consumiento copa del tipo" + tipo_copa + "\033[0m");
			copa2--; // quitamos una bebida
			}
		leave();
	
	
	}
	
	// invocado por el estanquero, indicando el ingrediente que pone
	public void servir(){
		enter();
			
			if( copa1 > 0 && copa2 >0 ){ // Duerme porque hay bebidas de los dos tipos en la barra
				System.out.println("\033[31m" + "Camarero duermiendo, no tiene trabajo. \033[0m");
				sem_servir.await();
			}
			// Cuando sale es porque algun alumno lo ha despertado, asi que produce tres copas de cada tipo
			copa1+=3;
			copa2+=3;
			System.out.println("\033[31m" + "Camarero sirviendo tres copillas de cada tipo. \033[0m");
			// Activamos la cola que esta esperando, aunque podiamos activar las dos indiferentemente.
			while(!sem_copa[0].isEmpty() || !sem_copa[0].isEmpty() ){
				sem_copa[0].signal();
				sem_copa[1].signal();
			}
		leave();
	}
}	
	
	



class Camarero implements Runnable{

	private Bareto bareto;
	public Thread thr ;
	
	public Camarero(  Bareto bareto ){ 
		
		this.bareto = bareto;
		thr = new Thread(this, "Camarero"); // to create the thread
	}
	
	public void run(){
		while ( true ){ 
			try{	// true becouse we have this programe works indefinitely
				bareto.servir();
				aux.dormir_max( 2000 ); // to send it to sleep
			}
		
			catch(Exception e){
		        System.err.println("Excepcion en main1: " + e);
		    }
		 }
	}

}


class Alumno implements Runnable{
	private static int numero_alumnos = 0;
	private int cod_alumno ;
	private Bareto bareto;
	public Thread thr ;
	
	public Alumno(Bareto bareto){
		cod_alumno = numero_alumnos;
		numero_alumnos++;	
		this.bareto = bareto;
		thr = new Thread(this, "Alumno"+cod_alumno);
	}
	
	public void run(){
		
		int tipo_copa=0;
		while (true){
			try{
		
				tipo_copa = (int)( Math.floor(Math.random()*2)); // To generate aleatory number between 0 - 1 both include); 			
				bareto.pedir(tipo_copa, cod_alumno); // pide la copa
				aux.dormir_max( 2000 ); // retardo
			}
			catch(Exception e){
		        System.err.println("Excepcion en main2: " + e);
		    }
		}
    }
}

class aux
{
  static Random genAlea = new Random() ;

  static void dormir_max( int milisecsMax )
  { 
    try
    { 
      Thread.sleep( genAlea.nextInt( milisecsMax ) ) ;
    } 
    catch( InterruptedException e )
    { 
      System.err.println("sleep interumpido en 'aux.dormir_max()'");
    }
  }
}

class ejercicio2{
	public static void main(String[] args)
    {
	if ( args.length != 1 ) 
        {  
     		  System.err.println("Uso: numero_de_alumnos");
     			 return ;
    	}
	int numero_alumnos = Integer.parseInt(args[0]);
	Bareto bareto = new Bareto();
	Camarero camarero = new Camarero(bareto);
	Alumno[] alumno = new Alumno[numero_alumnos];
	
	for(int i = 0 ; i < numero_alumnos ; i++){
		alumno[i] = new Alumno(bareto);
	}

	//We turn on the threads
	camarero.thr.start();
	for(int i = 0 ; i < numero_alumnos ; i++){
		alumno[i].thr.start();
	}
	


		
   } 
}

