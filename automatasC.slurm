#!/bin/bash
#SBATCH --job-name=automatasCelulares
#SBATCH --output=resultadosAutomatas
#SBATCH --partition=nu
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=4
#SBATCH --time=00:00:05
g++ -fopenmp -std=c++11 Automata.cpp -o test
./test

