import monitor.*;
import java.util.Random;
 // fumador1  -> PT; fumador2 -> PC; fumador3 -> TC;
// producto -> 0=Papel; 1=Tabaco; 3=Cerillas;

class Estanco extends AbstractMonitor
{ 
  int numero_fumadores=3;
  boolean producto_recogido;
  private Condition puede_producir= makeCondition();
  private Condition[] puede_fumar= new Condition[numero_fumadores];

  public Estanco(){ 
      for(int i=0; i<numero_fumadores; i++){
	   puede_fumar[i]=makeCondition();
	}
	producto_recogido=true;
  }

  public void obtenerIngrediente(int miIngrediente) { 
	enter();
	if(producto_recogido==true){
		puede_fumar[miIngrediente].await();
	}
	System.out.println("El fumador: "+(miIngrediente+1)+"ha cogido el producto y va a fumar"+"\n");
	producto_recogido=true;
	puede_producir.signal();
	leave();
  }

  public void ponerIngrediente(int ingrediente){
	enter();
		sem_fumador[ingrendiente].signal();
		producto_recogido=false;
	leave();
  }

  public void esperarRecogidaIngrediente() { 
	enter();
	if(producto_recogido==false){
		puede_producir.await();
	}
	leave();
  }
}

class Fumador implements Runnable {
	
   private int miIndice;
   public Thread thr;
   private Estanco estanco;
   
   public Fumador(Estanco _estanco, int p_miIndice){
	miIndice=p_miIndice%3;
	estanco=_estanco;
	thr=new Thread(this,"fumador " + p_miIndice);
   }
   
   public void run(){
	try{
		while(true){
			estanco.obtenerIngrediente(miIndice);
			aux.dormir_max(2000);	
			System.out.println("El fumador: "+(miIndice+1)+" ha terminado de fumar"+"\n");
		}
	}
	catch(Exception e){
            System.err.println("Excepcion en fumador: " + e);
        }	
   }
	
}



class Estanquero implements Runnable {   
	Estanco estanco;
	public Thread thr;

	public Estanquero(Estanco _estanco){
		estanco=_estanco;
		thr= new Thread(this,"estanquero");
	}
	
	public void run(){
		try{
			int ingrediente;
			while(true){
				ingrediente=(int) (Math.random() * 3.0);
				estanco.ponerIngrediente(ingrediente);
				estanco.esperarRecogidaIngrediente();
			}
		}
		catch(Exception e){
            		System.err.println("Excepcion en estanquero: " + e);
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

class MainFumadores { 
  public static void main( String[] args ){
	int numero_fumadores =((int) (Math.random() * 3.0)+1)*6;
	
	// Creamos un objeto del monitor, y creamos las hebras de estanquero y fumadores
	Estanco estanco1 = new Estanco();
	Estanco estanco2 = new Estanco();
        Estanquero estanquero = new Estanquero(estanco);
        Fumador[] fumadores = new Fumador[numero_fumadores];
	
	// Creamos las hebras de fumadores 
	for(int i=0; i<numero_fumadores; i++){
		if((i/3)%2==0){
        		fumadores[i]= new Fumador(estanco1, i);
		}
		else{
			fumadores[i]=new Fumador(estanco2, i);
		}
        }
	// Inicializamos el estanquero
	estanquero.thr.start();
	// Inicializamos los fumadores
	for(int i=0; i<numero_fumadores; i++){
        	fumadores[i].thr.start();
 	}
	
  }
}
