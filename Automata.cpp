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
using namespace std;


bool validaCntHilos(int ch);

template<typename T>
void print(T datos){
	cout << datos << endl;
}

//Definición de las funciones
int regla30(string secuencia);
int cuentaVecinos(vector<vector <int>>& matrizPrincipal, int tam_Automatas, int fil, int col);
void sim1D(vector<int> & bits_principal,queue< vector<int> > & cola, int hilo, int cant_bits, int cant_hilos, int cant_Iter, int & done_sending);
void sim2D(vector< vector<int> > &matrizP, queue <vector<int>> & cola, int hilo, int cant_hilos, int cant_bits, int cant_Iter, int & done_sending);
void escribirArchivo(string nombre_archivo, string hilera);
void limpiarArchivo(string nombre_archivo);
void registrarTiempo(string textoAEscribir, double tiempoInicial);
string colaToString(queue < vector <int> > & cola);
string matrizToString(vector < vector <int> > & matriz);
bool vecinoValido(int tam, int f, int c);
template<typename T>string vectorToString(vector<T> & arreglo);
vector< vector <int> > & llenarMatriz(vector<vector <int>> & matriz, int filas, int columnas, int dato);


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

/*
FUNCION:
ENTRADA:
SALIDA:
*/
int main() {
	double wtime = omp_get_wtime();
	int tam_auto = 2;
	int cant_hilos = 4;
	int cant_bits = 100;
	int cant_iter = 100;
	int done_sending = 0;
	vector<int> bits_principal(cant_bits, 0);
	vector< vector<int> > matrizP;
	llenarMatriz(matrizP,cant_bits,cant_bits,0);
	bits_principal[cant_bits / 2] = 1;
	queue< vector<int> > cola;

	limpiarArchivo("sim1D.txt"), limpiarArchivo("sim2D.txt"), limpiarArchivo("tiempo.txt");

	/*
	-FALTA INGRESAR DATOS POR CONSOLA
	-FALTA TERMINAR LA VERIFICACION DE RECIBIR DE LA COLA DE SIM2D
	POR MEDIO DE DONE_SENDING Y TRY_RECEIVE(?)
	*/
	escribirArchivo("tiempo.txt", "TIEMPOS CALCULADOS CON I = " + to_string(cant_iter) + " Y CON N = " + to_string(cant_bits) + "\n--------------------------------------------\n");
	#pragma omp parallel num_threads(2) shared(cola, cant_hilos, cant_bits, tam_auto, cant_iter, bits_principal, done_sending, matrizP)
	{
		omp_set_nested(1);
		#pragma omp sections
		{
			#pragma omp section
			{
				# pragma omp parallel num_threads(cant_hilos/2)
				{
					int thread_id = omp_get_thread_num();
					sim1D(bits_principal, cola, thread_id, cant_bits, cant_hilos/2, cant_iter, done_sending);
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
					sim2D(matrizP,cola,thread_id,cant_hilos/2,cant_bits,cant_iter,done_sending);
					#pragma omp master
					{
						registrarTiempo("La simulación de Conway tardó: ", wtime);
					}
				} 
			}
		}
	}
	registrarTiempo("Tiempo total: ", wtime);
	return 0;
}

/*
FUNCION: Verifica que la cantidad de hilos sea mayor que 1
ENTRADA: Cantidad de hilos
SALIDA: Si la cantidad de hilos es mayor que 1
*/
bool validaCntHilos(int ch) {
	return ch >= 1;
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
void sim1D(vector<int> & bits_principal, queue< vector<int> > & cola, int hilo, int cant_bits, int cant_hilos, int cant_Iter, int & done_sending){
	
	int bitsXhilo = cant_bits / cant_hilos;
	int lim_inf = hilo * bitsXhilo;
	int lim_sup = (hilo + 1) * bitsXhilo;

	//print("Para el hilo " + to_string(hilo) + " == [" + to_string(lim_inf) + "," + to_string(lim_sup) + "]");
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
			//print("Para la iteracion " + to_string(j) + " se tiene la secuencia " + secuencia + " = " + to_string(regla30(secuencia)));
		}
		//imprimirVector(bits_codificados);
		int cont = 0;
		for(int i = lim_inf; cont < bits_codificados.size(); i++){
			bits_principal[i] = bits_codificados[cont];
			cont++;
		}
		#pragma omp barrier
		#pragma omp master
		{
			cola.push(bits_principal);
			escribirArchivo("sim1D.txt", vectorToString(bits_principal));
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
FUNCION: 
ENTRADA:
SALIDA:
*/
void sim2D(vector< vector<int> > &matrizP, queue <vector<int>> & cola, int hilo, int cant_hilos, int cant_bits, int cant_Iter, int & done_sending){
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

		//Intento de Reduce y append
		for(int f = 0; f < cant_bits; f++){
			int col = 0;
			for(int c = limite_inf; c < limite_sup; c++){
				matrizP[f][c] = matriz_l[f][col];
				col++;
			}
		}
		
		//print("");
		#pragma omp barrier
	

		#pragma omp master
		{
			if(iter_realizadas >= cant_Iter-10){
				escribirArchivo("sim2D.txt",matrizToString(matrizP));
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
FUNCION:
ENTRADA: 
SALIDA: 
*/
// vector<int> & recibirCola(queue< vector < int> > & cola, int done_sending){
// 	bool elementoRecibido = false;
	
// }

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

bool vecinoValido(int tam, int f, int c){
	bool valido = true;
	if(f < 0 || f >= tam || c < 0 || c >= tam){
		valido = false;
	}
	return valido;
}

void registrarTiempo(string textoAEscribir, double tiempoInicial){
	escribirArchivo("tiempo.txt", textoAEscribir + to_string(omp_get_wtime() - tiempoInicial) + "\n");
}