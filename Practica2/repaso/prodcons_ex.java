
import monitor.*; // we import the package monitor

class MonitorBuffer extends AbstractMonitor
{
	// Declaration of variables
  	 private int      tam_v1 = 0    , 
			  tam_v2 = 0 ,
                          cont_v1    = 0    , 
			  cont_v2    = 0    ;
 	 private double[] bufferV1   = null ;
	 private double[] bufferV2   = null ;
	
	// Declaration of conditions
	private Condition consumiendo = makeCondition();
	private Condition produciendo = makeCondition();
	
	// Constructor
	public MonitorBuffer(int tam_v1, int tam_v2){
		this.tam_v1= tam_v1;
		bufferV1 = new double[tam_v1];
		this.tam_v2= tam_v2;
		bufferV2 = new double[tam_v2];
	}
	
	// We have to declarate the methods
	public  void depositar( double valor, int numero_productor ) throws InterruptedException
  	{ 
  	enter();
		/// Los productores pares producen en v1 y los productores impares producen en v2
		if(numero_productor%2 == 0 ){
			if( cont_v1 == tam_v1 ) 
			    	produciendo.await(); /// Esta lleno el buffer_v1  
           		if( cont_v1 < tam_v1){
				bufferV1[cont_v1] = valor ; 
				cont_v1++ ;
				consumiendo.signal(); 
			}
			
		}
		else{
			if( cont_v2 == tam_v2 ) 
			   	produciendo.await(); /// Esta lleno el buffer_v2    
         		
			if(cont_v2 < tam_v2){
				bufferV2[cont_v2] = valor ; 
				cont_v2++ ;
				consumiendo.signal();
			}
		
			
		}      
    	leave(); 
  	}
  	
  	public double extraer() throws InterruptedException
 	{ 
   	 
   	 enter();
		double valor  ;
		if( cont_v1 == 0 && cont_v2 == 0 ) 
		  	consumiendo.await() ; /// Tiene que esperarse que se produca un producto
		// primero va a consultar el buffer_v2 y despues el buffer_v1    
		if(cont_v2 != 0){
			cont_v2--;
			valor = bufferV2[cont_v2] ;
			 
		}
		else{ 
			cont_v1--;
			valor = bufferV1[cont_v1];	  
		}
		produciendo.signal();
    	leave();
	return valor;
    	
  	}
}



// ****************************************************************************

class Ejercicio1_Productor implements Runnable 
{ 
  private MonitorBuffer bb  ;
  private int    veces , 
                 numP  ;
  public  Thread thr   ;
  
  public Ejercicio1_Productor( MonitorBuffer pbb, int pveces, int pnumP ) 
  { 
    bb    = pbb;    // monitor que usa
    veces = pveces; // iteraciones que realizara
    numP  = pnumP ; // numero de productor
    thr   = new Thread(this,"productor "+numP);   // su correspondiente hebra asociada
  }

  public void run() 
  { 
    try
    { 
      double item = 100*numP ;  // es el numero que produce
      
      for( int i=0 ; i < veces ; i++ )
      { 
        System.out.println("\033[32m"+thr.getName() + ", produciendo " + item + "\033[0m"); // mensaje por pantalla
        bb.depositar( item++ , numP );    // llama a la funcion depositar del monitor
      }
    }
    catch( Exception e ) 
    { 
      System.err.println("Excepcion en main: " + e);
    }
  }
}



// ****************************************************************************

class Ejercicio1_Consumidor implements Runnable 
{ 
  private MonitorBuffer  bb    ;  // monitor que usa el consumidor
  private int     veces ,         // numero de veces que consumira
                  numC  ;         // numero de consumidor que es
  public  Thread  thr   ;         // su correspondiente hebra asociada
  
  public Ejercicio1_Consumidor( MonitorBuffer pbb, int pveces, int pnumC ) 
  { 
    bb    = pbb;
    veces = pveces;
    numC  = pnumC ;
    thr   = new Thread(this,"consumidor "+numC);
  }

  public void run() 
  { 
    try 
    { 
      for( int i=0 ; i<veces ; i++ )
      { 
        double item = bb.extraer (); // ""al consumidor no le interesa de donde sale su producto solo quiere consumir""
        System.out.println("\033[31m"+thr.getName() + ", consumiendo " + item + "\033[0m");
      } 
    }
    catch( Exception e ) 
    { 
      System.err.println("Excepcion en main: " + e);
    }
  }
}



// ****************************************************************************

class prodcons_ex 
{ 
  public static void main( String[] args ) 
  { 
    if ( args.length != 6 ) 
    {  
      System.err.println("Uso: ncons nprod tambufv1 tambufv2 niterp niterc");
      return ;
    }

    // leer parametros, crear vectores y buffer intermedio
      Ejercicio1_Consumidor[] cons      = new Ejercicio1_Consumidor[Integer.parseInt(args[0])] ;
      Ejercicio1_Productor[]  prod      = new Ejercicio1_Productor[Integer.parseInt(args[1])] ;
      MonitorBuffer buffer  		    = new MonitorBuffer(Integer.parseInt(args[2]),Integer.parseInt(args[3])); // añadimos otro buffer tenemos v1 y v2
      int          iter_cons 		    = Integer.parseInt(args[4]);
      int          iter_prod 		    = Integer.parseInt(args[5]);
	  
    if ( cons.length*iter_cons != prod.length*iter_prod )
	  { 
      System.err.println("no coinciden número de items a producir con a cosumir");
      return ;
    }    

	  // Make thread
	  for(int i = 0; i < cons.length; i++) 
	    cons[i] = new Ejercicio1_Consumidor(buffer,iter_cons,i) ;
	  for(int i = 0; i < prod.length; i++)
		prod[i] = new Ejercicio1_Productor(buffer,iter_prod,i);
	  // poner en marcha las hebras
	  for(int i = 0; i < prod.length; i++) 
      prod[i].thr.start();
	  for(int i = 0; i < cons.length; i++) 
      cons[i].thr.start();
  }
  
}

