/*
TAREA CORTA 2 - CI-0117
HECHO POR: SEBASTIAN MONTERO CASTRO B95016 & CARLOS ESPINOZA PERAZA B92786
AUTOMATA DE CONWAY GENERADO POR MEDIO DEL AUTOMATA DE REGLA 30 EN PARALELO UTILIZANDO OPENMP.
PROFESOR: ALAN CALDERON CASTRO
*/

#include <iostream>
//#include <omp.h> 
#include <string>
#include <vector>
#include <queue>
using namespace std;


bool validaCntHilos(int ch);

//Definición de las funciones
int regla30(string secuencia);
void sim1D(queue < vector<int> > cola, int hilo, int tam_Automatas, int tam_Equipos, int cant_Iter);
int cuentaVecinos(vector<int>& vectorPrincipal, int tam_Automatas, int fil, int col);
void sim2d(int hilo, int tam_Automatas, int cant_Iter);

int main() {
    cout << regla30("000") << endl;
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

void sim1D(queue< vector<int> > cola, int hilo, int cant_bits, int cant_hilos, int cant_Iter){
	vector<int> bits_principal(cant_bits, 0);
	bits_principal[cant_bits / 2] = 1;
	int bitsXhilo = cant_bits / cant_hilos;
	int lim_inf = hilo * bitsXhilo;
	int lim_sup = (hilo + 1) * bitsXhilo;

#pragma parallel for
	for(int i = 0; i < cant_Iter; i++){
		vector<int> bits_codificados;
		for(int j = lim_inf; j < lim_sup; j++){
			string secuencia = "";
			if(j == 0){
				secuencia = to_string(bits_principal[cant_bits - 1]) + to_string(bits_principal[0]) + to_string(bits_principal[1]);
			}else if (j == cant_bits - 1){
				secuencia = to_string(bits_principal[cant_bits - 2]) + to_string(bits_principal[cant_bits -1]) + to_string(bits_principal[0]);
			}else{
				secuencia = to_string(bits_principal[cant_bits - 1]) + to_string(j) + to_string(bits_principal[j+1]);
			}
			bits_codificados.push_back(regla30(secuencia));
		}
		#pragma critical
		for(int i = 0; i < bits_codificados.size(); i++){
			bits_principal.push_back(bits_codificados[i]);
		}
		#pragma omp barrier
		if(hilo == 0){
			cola.push(bits_principal);
		}
	}
}

