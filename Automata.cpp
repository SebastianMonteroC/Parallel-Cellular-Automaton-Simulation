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
using namespace std;


bool validaCntHilos(int ch);

template<typename T>
void print(T datos){
	cout << datos << endl;
}

//Definición de las funciones
int regla30(string secuencia);
void sim1D(vector<int> & bits_principal,queue< vector<int> > & cola, int hilo, int cant_bits, int cant_hilos, int cant_Iter);
int cuentaVecinos(vector<vector <int>>& matrizPrincipal, int tam_Automatas, int fil, int col);
void sim2d(queue <vector<int>> & cola, int hilo, int cant_hilos, int tam_Automatas, int cant_Iter);
template<typename T>
void imprimirVector(vector<T> vector){
	for(int i = 0; i < vector.size(); i++){
		cout << vector[i] << " ";
	}
	cout << endl;
}

int main() {
	int num_hilos = 4;
	int cant_bits = 50;
	int tam_auto = 2;
	int cant_iter = 30;
	vector<int> bits_principal(cant_bits, 0);
	bits_principal[cant_bits / 2] = 1;
	
	queue< vector<int> > cola;
	

	#pragma omp parallel num_threads(num_hilos) shared(cola, num_hilos, cant_bits, tam_auto, cant_iter, bits_principal)
	{
		int thread_id = omp_get_thread_num();
		
		sim1D(bits_principal, cola, thread_id, cant_bits, num_hilos, cant_iter);
		
	}
	while(!cola.empty()){
		imprimirVector(cola.front());
		cola.pop();
	}
	// for(int i = 0; i < cola.size(); i++){
	// 	vector<int> vAux = cola.front();
	// 	imprimirVector(vAux);
	// }

	
}

bool validaCntHilos(int ch) {
	return ch >= 1;
}

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

void sim1D(vector<int> & bits_principal, queue< vector<int> > & cola, int hilo, int cant_bits, int cant_hilos, int cant_Iter){
	
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
			}else if (j == cant_bits - 1){
				secuencia = to_string(bits_principal[cant_bits - 2]) + to_string(bits_principal[cant_bits -1]) + to_string(bits_principal[0]);
			}else{
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
		
		cola.push(bits_principal);
	}

	
}

int cuentaVecinos(vector<vector <int>>& matrizPrincipal, int tam_Automatas, int fil, int col){
	int cantidadVecinos = 0;
	for(int i = fil-1; i < fil+2; i++){
		for(int j = col-1; j < col+2; j++){
			cantidadVecinos += matrizPrincipal[i%tam_Automatas][j%tam_Automatas];
		}
	}
	return cantidadVecinos - matrizPrincipal[fil][col];
}

void sim2D(queue <vector<int>> & cola, int hilo, int cant_hilos, int tam_Automatas, int cant_Iter){
	int cantColumnas = tam_Automatas / cant_hilos;
	int limite_inf = hilo * cantColumnas;
	int limite_sup = (hilo + 1) * cantColumnas;
	vector<vector <int>> matriz;
	int cantidad_estados_1D = 0;
	bool colaVacia = cola.empty();
	int iter_realizadas = 0;
	vector<int> fila0;

}