import monitor.*;
import java.util.Random;
 // fumador1  -> PT; fumador2 -> PC; fumador3 -> TC;
// producto -> 0=Papel; 1=Tabaco; 3=Cerillas;

class Estanco extends AbstractMonitor
{ 
  // Creamos las variables estado
  int numero_fumadores=3;
  int producto_recogido;
  int numero_veces[]={0,0,0};

   // Creamos las variables condicion
  private Condition puede_producir= makeCondition();
  private Condition[] no_puedeMax= new Condition[numero_fumadores];
  private Condition[] puede_fumar= new Condition[numero_fumadores];

  // En el constructor inicialiamos las variables estado y condicion
  public Estanco(){ 
      for(int i=0; i<numero_fumadores; i++){
	   puede_fumar[i]=makeCondition();
	}
	for(int i=0; i<numero_fumadores; i++){
	   no_puedeMax[i]=makeCondition();
	}
	// Utilizamos esta variable para (cuando contiene -1 quiere decir que el producto ha sido recogido)
	producto_recogido=-1;
  }

  public void obtenerIngrediente(int miIngrediente) { 
	enter();
	// aqui entra el cliente con el ingrendiente que le falta, por lo que si el producto que ha sido producido
	// no es el que necesita se bloquea
	if(producto_recogido!=miIngrediente){
		puede_fumar[miIngrediente].await();
	}
	if(numero_veces[miIngrediente]>2){
		System.out.println("El fumador: "+(miIngrediente+1)+"ha fumado 3 veces y se bloquea---------------------------"+"\n");
		producto_recogido=-1;
		puede_producir.signal();
		no_puedeMax[miIngrediente].await();
	}
	// Si es su producto, lo coge guardando en la variable producto_recogido= -1, y da paso al estanquero
	System.out.println("El fumador: "+(miIngrediente+1)+"ha cogido el producto y va a fumar"+"\n");
	producto_recogido=-1;
	numero_veces[miIngrediente]++;
	puede_producir.signal();
	leave();
  }

  public void ponerIngrediente(int ingrediente){
	enter();
		if (ingrediente==0){
			System.out.println("El Estanquero ha producido Papel"+"\n");
		}
		if(ingrediente==1){
			System.out.println("El Estanquero ha producido Tabaco"+"\n");
		}
		if(ingrediente==2){
			System.out.println("El Estanquero ha producido Cerillas"+"\n");
		}
		// El estanquero producte, guardandolo en la variable producto_recogido y le da paso al fumador que necesita ese producto
		producto_recogido=ingrediente;
		puede_fumar[ingrediente].signal();
	leave();
  }

  public void esperarRecogidaIngrediente() { 
	enter();
	// En este caso mientras que el producto no se ha recogido por un fumador el estanquero se bloquea
	if(producto_recogido!=-1){
		//puede_producir.await();
	}
	leave();
  }
}

class Fumador implements Runnable {
	
   private int miIndice;
   public Thread thr;
   private Estanco estanco;
   
   public Fumador(Estanco _estanco, int p_miIndice){
	miIndice=p_miIndice;
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
	int numero_fumadores=3;
	// Creamos un objeto del monitor, y creamos las hebras de estanquero y fumadores
	Estanco estanco = new Estanco();
        Estanquero estanquero = new Estanquero(estanco);
        Fumador[] fumadores = new Fumador[numero_fumadores];
	
	// Creamos las hebras de fumadores 
	for(int i=0; i<numero_fumadores; i++){
        	fumadores[i]= new Fumador(estanco, i);
        }
	// Inicializamos el estanquero
	estanquero.thr.start();
	// Inicializamos los fumadores
	for(int i=0; i<numero_fumadores; i++){
        	fumadores[i].thr.start();
 	}
	
  }
}
