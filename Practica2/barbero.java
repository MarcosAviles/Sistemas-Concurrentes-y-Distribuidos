import monitor.*;
import java.util.Random;

class Barberia extends AbstractMonitor{
	
	// Creamos las variables condicion
	private Condition sem_barbero= makeCondition();
	private Condition sem_salaespera= makeCondition();
	private Condition sem_silla= makeCondition();

	// Creamos una variable estado
	int numero_clientes=-1;
	
	// invcado por los clientes para cortarse el pelo
	public void cortarPelo (int indice){
		enter();
			// Hemos inicializado la variable numero_cliente=-1 por el motivo de que el primer cliente que entre
			// sea el cliente 0, así podemos cumplir con la condición que el primer cliente que entre a la barberia
			// pase directamente a la silla y sino se bloquea
			System.out.println("El cliente: "+indice+" entra a la barberia");
			numero_clientes++;
			// si hay gente en la barberia se bloquea el cliente
			if(numero_clientes>0){
				System.out.println("El cliente: "+indice+" se espera en la sala de espera");
				sem_salaespera.await();
			}

			// sino pasa a la silla y despierta al barbero
			else{
				System.out.println("El cliente: "+indice+" despierta al barbero");
				sem_barbero.signal();
			}
			// Bloqueamos una vez que ya esta el cliente en la silla
			System.out.println("El cliente: "+indice+" se sienta en la silla");
			sem_silla.await();
			System.out.println("El cliente: "+indice+" ha terminado de pelarse y se va");
			
		leave();	

	}

	// invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
	public void siguienteCliente (){
		enter();
			// Si no hay clientes el barbero se bloquea ( tiene que ser -1 ya que como hemos inializado la variable
			// numero de clientes a -1, cuando salgan todos los clientes de la barberia esta variable contendrá -1
			if(numero_clientes<0){
				System.out.println("No hay nadie en la barberia ni en la silla y el barbero se duerme");
				sem_barbero.await();
				
			}
			// Si no se cumple lo anterior damos paso al siguiente cliente de la sala de espera
			else{
				sem_salaespera.signal();
								
			}
		leave();
			
	}
	// invocado por el barbero para indicar que ha terminado de cortar el pelo
	public void finCliente (){
		enter();
			// Una vez que termina un cliente de pelarse lo eliminamos de la barberia y damos paso
			// al siguiente cliente, que esta esperando que este libre la silla
			numero_clientes--;
			sem_silla.signal();
		leave();
	}
}


class Cliente implements Runnable{ 

	public Thread thr;
	Barberia barberia;
	int indice;
	
	public Cliente(Barberia _barberia, int indice_hebra){
		barberia=_barberia;
		thr=new Thread(this,"Cliente ");
		indice=indice_hebra;
	}

	public void run (){ 
		try{
			while (true) {
				barberia.cortarPelo (indice); // el cliente espera (si procede) y se corta el pelo
				aux.dormir_max( 2000 ); // el cliente está fuera de la barberia un tiempo
			}
		}
		catch(Exception e){
            		System.err.println("Excepcion en cliente: " + e);
        	}
	}
}

class Barbero implements Runnable{ 
	
	public Thread thr;
	Barberia barberia;

	public Barbero(Barberia _barberia){
		barberia=_barberia;
		thr=new Thread(this,"Barbero ");
	}
	public void run (){ 
		try{
			while (true) {
				barberia.siguienteCliente ();
				aux.dormir_max( 2500 ); // el barbero está cortando el pelo
				barberia.finCliente ();
			}
		}
		catch(Exception e){
            		System.err.println("Excepcion en barbero: " + e);
        	}
	}
}

class aux{
	static Random genAlea = new Random() ;
	static void dormir_max( int milisecsMax ){ 
		try{ 
			Thread.sleep( genAlea.nextInt( milisecsMax ) ) ;
		}
		catch( InterruptedException e ){ 
			System.err.println("sleep interumpido en ’aux.dormir_max()’");
		}
	}
}

class MainBarbero { 
  public static void main( String[] args ){
	int n_clientes=3;
	// creamos las instancias y objetos 
	Barberia barberia = new Barberia();
        Barbero barbero = new Barbero(barberia);
        Cliente[] clientes = new Cliente[n_clientes];

	// Ponemos en marcha el barbero
	barbero.thr.start();
	
	// Inicializamos los clientes
	for(int i=0; i<n_clientes; i++){
  		clientes[i]= new Cliente(barberia,i);
       }
	// ponemos en marcha a los clientes
	for(int i=0; i<n_clientes; i++){
  		clientes[i].thr.start();
       }
	
  }
}
