/*
TAREA CORTA 2 - CI-0117
HECHO POR: SEBASTIAN MONTERO CASTRO B95016 & CARLOS ESPINOZA PERAZA B92786
AUTOMATA DE CONWAY GENERADO POR MEDIO DEL AUTOMATA DE REGLA 30 EN PARALELO UTILIZANDO OPENMP.
PROFESOR: ALAN CALDERON CASTRO
*/

#include <iostream>
#include <omp.h> 
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <assert.h>
using namespace std;



//Definición de las funciones
int regla30(string secuencia);
int cuentaVecinos(vector<vector <int>>& matrizPrincipal, int tam_Automatas, int fil, int col);
void sim1D(vector<int> & bits_principal,queue< vector<int> > & cola, int hilo, int cant_bits, int cant_hilos, int cant_Iter, int & done_sending, string & str_sim1D);
void sim2D(vector< vector<int> > &matrizP, queue <vector<int>> & cola, int hilo, int cant_hilos, int cant_bits, int cant_Iter, int & done_sending, string & str_sim2D);
void escribirArchivo(string nombre_archivo, string hilera);
void limpiarArchivo(string nombre_archivo);
void registrarTiempo(string textoAEscribir, double tiempoInicial);
bool vecinoValido(int tam, int f, int c);
string colaToString(queue < vector <int> > & cola);
string matrizToString(vector < vector <int> > & matriz);
template<typename T>string vectorToString(vector<T> & arreglo);
template<typename T>void print(T datos);
vector< vector <int> > & llenarMatriz(vector<vector <int>> & matriz, int filas, int columnas, int dato);



/*
FUNCION: Main del programa. Se encarga de realizar la paralelizacion por medio de los hilos y de repartir
las tareas correspondientes a cada hilo. Recibe los datos por consola en forma de argumentos y los utiliza
para procesar SIM1D(El automata de regla 30) y el SIM2D(El automata de Conway). Llama los metodos necesarios
la ejecucion apropiada del programa.
ENTRADA: Los argumentos por consola de cantidad de hilos, bits e iteraciones.
SALIDA: -
*/
int main(int argc, char* argv[]) {
	//argv[1] = cant_hilos ; argv[2] = cant_bits ; argv[3] = cant_iter;
	double wtime = omp_get_wtime();
	int cant_hilos = stoi(argv[1]);
	int cant_bits = stoi(argv[2]);
	int cant_iter = stoi(argv[3]);
	int done_sending = 0;
	string str_sim1D = "";
	string str_sim2D = "";
	vector<int> bits_principal(cant_bits, 0);
	vector< vector<int> > matrizP;
	llenarMatriz(matrizP,cant_bits,cant_bits,0);
	bits_principal[cant_bits / 2] = 1;
	queue< vector<int> > cola;

	limpiarArchivo("sim1D.txt"), limpiarArchivo("sim2D.txt"), limpiarArchivo("tiempo.txt");
	escribirArchivo("tiempo.txt", "TIEMPOS CALCULADOS CON I = " + to_string(cant_iter) + " Y CON N = " + to_string(cant_bits) + "\n--------------------------------------------\n");
	#pragma omp parallel num_threads(2) shared(cola, cant_hilos, cant_bits, cant_iter, bits_principal, done_sending, matrizP)
	{
		omp_set_nested(1);
		#pragma omp sections
		{
			#pragma omp section
			{
				# pragma omp parallel num_threads(cant_hilos/2)
				{
					int thread_id = omp_get_thread_num();
					sim1D(bits_principal, cola, thread_id, cant_bits, cant_hilos/2, cant_iter, done_sending, str_sim1D);
					#pragma omp master
					{
						registrarTiempo("La simulación de Regla 30 tardó: ", wtime);
					}
				} 
			}

			#pragma omp section
			{
				# pragma omp parallel num_threads(cant_hilos/2)
				{
					int thread_id = omp_get_thread_num();
					sim2D(matrizP,cola,thread_id,cant_hilos/2,cant_bits,cant_iter,done_sending, str_sim2D);
					#pragma omp master
					{
						registrarTiempo("La simulación de Conway tardó: ", wtime);
					}
				} 
			}
		}
	}
	registrarTiempo("Tiempo total: ", wtime);

	escribirArchivo("sim1D.txt", str_sim1D);
	escribirArchivo("sim2D.txt", str_sim2D);
	return 0;
}

/*
FUNCION: Verifica una secuencia dada por un string de 3 digitos y retorna un entero '0' o '1' dependiendo del string
ENTRADA: String secuencia que se verificara para determinar un resultado
SALIDA: Numero entero '0' o '1' dependiendo de la secuencia dada en la entrada
*/
int regla30(string secuencia){
    int codificado = 0;
    if (secuencia == "111")
		codificado = 0;
	else if (secuencia == "110")
		codificado = 0;
	else if (secuencia == "101")
		codificado = 0;
	else if (secuencia == "100")
		codificado = 1;
	else if (secuencia == "011")
		codificado = 1;
	else if (secuencia == "010")
		codificado = 1;
	else if (secuencia == "001")
		codificado = 1;
    return codificado;
}

/*
FUNCION: Simulacion del primer automata unidimensional que funciona a partir del automata de Regla 30. Procesa los datos dependiendo de
los parametros establecidos y genera el automata correspondiente, el cual es insertado en la cola cuando termina el programa. Esto, al
ser parte del programa paralela, lo hace en multiples hilos.
ENTRADA: Recibe el vector lleno de 0s y con 1 en el medio a partir del cual trabajara (bits_principal), 
la cola en la que insertara el resultado final (cola), el hilo que se esta trabajando (hilo), 
la cantidad de bits que se desean procesar (cant_bits), la cantidad de hilos con los cuales se estan trabajando (cant_hilos)
y por ultimo la cantidad de iteraciones que se desean realizar (cant_iter)
SALIDA: A pesar de no retornar nada (void), reduce los datos procesados por todos los hilos por medio del
hilo maestro y prosede a empujar los resultados correspondientes en la cola.
*/
void sim1D(vector<int> & bits_principal, queue< vector<int> > & cola, int hilo, int cant_bits, int cant_hilos, int cant_Iter, int & done_sending, string & str_sim1D){
	
	int bitsXhilo = cant_bits / cant_hilos;
	int lim_inf = hilo * bitsXhilo;
	int lim_sup = (hilo + 1) * bitsXhilo;
	#pragma omp barrier
	for(int i = 0; i < cant_Iter; i++){
		vector<int> bits_codificados;
		for(int j = lim_inf; j < lim_sup; j++){
			string secuencia = "";
			if(j == 0){
				secuencia = to_string(bits_principal[cant_bits - 1]) + to_string(bits_principal[0]) + to_string(bits_principal[1]);
			}
			else if (j == cant_bits - 1){
				secuencia = to_string(bits_principal[cant_bits - 2]) + to_string(bits_principal[cant_bits -1]) + to_string(bits_principal[0]);
			}
			else{
				secuencia = to_string(bits_principal[j - 1]) + to_string(bits_principal[j]) + to_string(bits_principal[j+1]);
			}
			bits_codificados.push_back(regla30(secuencia));
		}
		int cont = 0;
		for(int i = lim_inf; cont < bits_codificados.size(); i++){
			bits_principal[i] = bits_codificados[cont];
			cont++;
		}
		#pragma omp barrier
		#pragma omp master
		{
			cola.push(bits_principal);
			str_sim1D += vectorToString(bits_principal) + "\n";
			//escribirArchivo("sim1D.txt", vectorToString(bits_principal));
		}
		
	}
	#pragma omp critical
	done_sending++;
}

/*
FUNCION: Cuenta la cantidad de vecinos alrededor de una posicion especifica en la matriz ingresada
ENTRADA: La matriz en la que se desean analizar los vecinos, el tamaño del automata, la fila y la columna (es decir, la casilla especifica)
que se desea analizar
SALIDA: Retorna la cantidad de vecinos de esa casilla elegida
*/
int cuentaVecinos(vector<vector <int>>& matrizPrincipal, int tam_Automatas, int fil, int col){
	int cantidadVecinos = 0;
	for(int i = fil-1; i < fil+2; i++){
		for(int j = col-1; j < col+2; j++){
			if(vecinoValido(tam_Automatas,i,j)){
				cantidadVecinos += matrizPrincipal[i%tam_Automatas][j%tam_Automatas];
			}
			
		}
	}
	return cantidadVecinos - matrizPrincipal[fil][col];
}

/*
FUNCION: Automata de Conway. Este es un automata celular de dos dimensiones que trabaja en una matriz y es autodependiente. Esto
significa que depende solo de su estado inicial. Con cada iteracion avanza el automata y va generando nuevas "formas de vida" en la matriz.
ENTRADA: La matriz en la que se escribira el automata, la cola que contiene los productos del automata de Regla 30 para constantemente
alimentar la simulacion, el id del hilo que esta trabajando, la cantidad de hilos del programa, la cantidad de bits por procesar, la
cantidad de iteraciones por realizar y una referencia a un numero entero done_sending que se utiliza como auxiliar para verificar si
ya no hay mas elementos que esperar en la cola.
SALIDA: El automata de Conway completo en un archivo de texto
*/
void sim2D(vector< vector<int> > &matrizP, queue <vector<int>> & cola, int hilo, int cant_hilos, int cant_bits, int cant_Iter, int & done_sending, string & str_sim2D){
	double tiempoIteracion = omp_get_wtime();
	int cantColumnas = cant_bits / cant_hilos;
	int limite_inf = hilo * cantColumnas;
	int limite_sup = (hilo + 1) * cantColumnas;
	int cantidad_estados_1D = 0;
	int iter_realizadas = 0;
	bool colaVacia = cola.empty();
	

	#pragma omp master
	{
		
		bool sigue = true;
		while(sigue){
			if(!cola.empty()){
				matrizP[0] = cola.front();
				cola.pop();
				sigue = false;
			}
		}
		cantidad_estados_1D++;
	}
	while((iter_realizadas < cant_Iter) || (cantidad_estados_1D < cant_Iter) && (done_sending != cant_hilos)){
		#pragma omp barrier
		vector< vector<int> > matriz_l;
		llenarMatriz(matriz_l,cant_bits,limite_sup-limite_inf,0);
		for(int f = 0; f < cant_bits; f++){
			for(int c = limite_inf; c < limite_sup; c++){
				if(matrizP[f][c] == 1){
					if(cuentaVecinos(matrizP, cant_bits,f,c) < 2 || cuentaVecinos(matrizP, cant_bits, f, c) > 3){
						matriz_l[f][c - limite_inf] = 0;
					}
					else{
						matriz_l[f][c - limite_inf] = 1;
					}
				}
				else if(matrizP[f][c] == 0 && cuentaVecinos(matrizP,cant_bits, f, c) == 3){
					matriz_l[f][c - limite_inf] = 1;
				}
			}
		}

		//Reduce y append a la matriz
		for(int f = 0; f < cant_bits; f++){
			int col = 0;
			for(int c = limite_inf; c < limite_sup; c++){
				matrizP[f][c] = matriz_l[f][col];
				col++;
			}
		}
		
		#pragma omp barrier
		#pragma omp master
		{
			if(iter_realizadas >= cant_Iter-10){
				str_sim2D += matrizToString(matrizP) + "\n";
			}
			if(iter_realizadas == 0){
				registrarTiempo("Una iteracion de Conway tarda: ", tiempoIteracion);
			}
			iter_realizadas++;
			bool final = false;
			while(!final){
				if(!cola.empty()){
					matrizP[0] = cola.front();
					cola.pop();				
					final = true;
				}	

				if(done_sending == cant_hilos){
					final = true;
				}
			}
		}
		
	}
}

/*
FUNCION: Escribe, en una archivo de texto del nombre deseado, la hilera correspondiente.
ENTRADA: El nombre del archivo de texto deseado. Si el nombre archivo de texto ya existe, continua escribiendo
en este. Tambien, recibe la hilera que se desea escribir en tal archivo.
SALIDA: -
*/
void escribirArchivo(string nombre_archivo, string hilera){
	ofstream escritor;
	escritor.open(nombre_archivo,ios_base::app);
	escritor << hilera << "\n";
	escritor.close();
}

/*
FUNCION: Limpia un archivo de texto correspondiende
ENTRADA: El nombre del archivo de texto deseado.
SALIDA: -
*/
void limpiarArchivo(string nombre_archivo){
	ofstream escritor;
	escritor.open(nombre_archivo,ofstream::trunc);
	escritor.close();
}

/*
FUNCION: Retorna los elementos de un vector de Tipo T como un string
ENTRADA: El vector de tipo T a ser transformado a string
SALIDA: El string generado a partir de los elementos del vector
*/
template<typename T> string vectorToString(vector<T> & arreglo){
	string hilera = "";
	for(int i = 0; i < arreglo.size(); i++){
		hilera += to_string(arreglo[i]);
	}
	return hilera;
}

/*
FUNCION: Retorna los elementos de una cola de vectores en forma de string
ENTRADA: La cola de vectores que desea procesar
SALIDA: Los elementos de los vectores de la cola convertidos a string
*/
string colaToString(queue< vector < int> > & cola){
	string hilera = "";
	queue< vector<int> > copiaCola;
	copiaCola = cola;
	for(int i = 0; !copiaCola.empty(); i++){
		hilera += vectorToString(copiaCola.front()) + "\n";
		copiaCola.pop();
	}
	return hilera;
}

/*
FUNCION: Transforma una matriz y sus elementos a un string
ENTRADA: La matriz que desea transformar
SALIDA: El string que representa los elementos de la matriz
*/

string matrizToString(vector < vector <int> > & matriz){
	string hilera = "";
	for(int i = 0; i < matriz.size(); i++){
		hilera += vectorToString(matriz[i]) + "\n";
	}
	return hilera;
}

/*
FUNCION: Llena una matriz de filas y columnas con el elemento deseado
ENTRADA: Cantidad de filas y columnas de la matriz y el elemento con el que 
desea llenar la matriz
SALIDA: La matriz generada y llena del elemento deseado
*/
vector< vector <int> > & llenarMatriz(vector< vector<int> > & matriz, int filas, int columnas, int dato){
	for(int i = 0; i < filas; i++){
		vector<int> fila(columnas,dato);
		matriz.push_back(fila);
	}
	return matriz;
}

/*
FUNCION: Verifica si una casilla en una matriz es valida (por ejemplo, matriz[-1][1] retornaria false)
ENTRADA: El largo de la matriz (se asume que es cuadrada), la fila y la columna de la casilla a analizar
SALIDA: Si la casilla analizada se encuentra dentro de la matriz o no
*/
bool vecinoValido(int tam, int f, int c){
	bool valido = true;
	if(f < 0 || f >= tam || c < 0 || c >= tam){
		valido = false;
	}
	return valido;
}

/*
FUNCION: Escribe en un archivo de texto los tiempos correspondientes registrados.
ENTRADA: El texto que desea registrar (el tiempo) y el tiempo inicial para calcularlo.
SALIDA: -
*/
void registrarTiempo(string textoAEscribir, double tiempoInicial){
	escribirArchivo("tiempo.txt", textoAEscribir + to_string(omp_get_wtime() - tiempoInicial) + "\n");
}


/*
FUNCION: Valida que la cantidad de hilos sea mayor o igual a 2 y sea una cantidad par
ENTRADA: Cantidad de hilos
SALIDA: Retorna verdadero si la cantidad es valida y si no, el programa acaba
*/
bool validaCntHilos(int ch){
	assert(ch >= 2 && ch%2 == 0);
	return true;
}

/*
FUNCION: Imprimir datos
ENTRADA: Cualquier tipo de dato T (enplantillado)
SALIDA: El dato en pantalla
*/
template<typename T>
void print(T datos){
	cout << datos << endl;
}

/*
FUNCION: Imprime los elementos de un vector en secuencia
ENTRADA: El vector <T> por recorrer para imprimir los datos en pantalla
SALIDA: Los datos del vector en orden
*/
template<typename T> void imprimirVector(vector<T> vector){
	for(int i = 0; i < vector.size(); i++){
		cout << vector[i] << " ";
	}
	cout << endl;
}