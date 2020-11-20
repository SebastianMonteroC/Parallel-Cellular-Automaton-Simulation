/*
TAREA CORTA 2 - CI-0117
HECHO POR: SEBASTIAN MONTERO CASTRO B95016 & CARLOS ESPINOZA PERAZA B92786
AUTOMATA DE CONWAY GENERADO POR MEDIO DEL AUTOMATA DE REGLA 30 EN PARALELO UTILIZANDO OPENMP.
PROFESOR: ALAN CALDERON CASTRO
*/

#include <iostream>
#include <omp.h> 
#include <string>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable : 4996)

bool validaCntHilos(int ch);
bool validaCntTerminos(long long ct);

int main(int argc, char* argv[]) {
	int thread_count = 0, n = 0;
    double tiempo;
    cout << "--- CONJETURA DE NICOMACO ---" << endl;
	while (!validaCntHilos(thread_count) || !validaCntTerminos(n)) {
		cout << "Digite la cantidad de hilos ( >= 1 ): ";
		cin >> thread_count;
		cout << "Digite el rango de numeros ( >= 2 ): ";
		cin >> n;
    }

    tiempo = omp_get_wtime();

#  pragma omp parallel for num_threads(thread_count)
	cout << "hola" << endl;
	return 0;
}

bool validaCntHilos(int ch) {
	return ch >= 1;
}

bool validaCntTerminos(long long ct) {
	return ct >= 2;
}
