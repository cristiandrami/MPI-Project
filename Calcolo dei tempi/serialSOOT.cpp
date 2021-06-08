#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <stdlib.h>
#include<time.h>

// Minimo valore per creare particelle di gas casuali
const double MIN_PROBABILITY_GAS = 0.8;

// Per la matrice 300x300
const unsigned SIZE = 300;

enum Cell_states { EMPTY=0, ICE=1, GAS=2 };

// Genera un numero casuale e lo confronta con
// la costane MIN_PROBALILITY_GAS, per decidere casualmente se una cella
// inizialmente sar� di tipo GAS oppure EMPTY
int choose_gas_or_empty() 
{
    double value = ((double)rand() / (RAND_MAX));
    if (value > MIN_PROBABILITY_GAS)
        return GAS; 
    else
        return EMPTY;
}


// Ritorna true se la cella in posizione (i,j) nella sottomatrice del
// processo che ha chiamato questa funzione � di tipo ICE
bool becomes_ice(int** portionMatrix, const int& i, const int& j, const int& portionSize) 
{
 
    if (j - 1 >= 0 && portionMatrix[i][j - 1] == ICE)
        return true;

    if (j + 1 < SIZE && portionMatrix[i][j + 1] == ICE)
        return true;
    
    if (i + 1 <= portionSize + 1 && portionMatrix[i + 1][j] == ICE)
        return true;

    if (i - 1 >= 0 && portionMatrix[i - 1][j] == ICE)
        return true;


    return false;

}



enum Directions {LEFT=0, RIGHT=1, UP=2, DOWN=3};



void move(int** portionMatrix,  int old_i, int old_j, int new_i, int new_j)
{
    portionMatrix[old_i][old_j] = EMPTY;
    portionMatrix[new_i][new_j] = GAS;
}

// Sposta una cella di GAS in modo random (il controllo sullo stato
// della cella viene effettuato prima).
// Se il gas non ha celle in cui spostarsi, rimane fermo. (per avere una prima variante di SOOT)
void move_gas(int** portionMatrix, int i,  int j,  int portionSize) 
{

    int random= rand()/RAND_MAX;
    
    std::vector<int> possible_directions;

    // UP
    if (i -1 >= 0 && portionMatrix[i-1][j] == EMPTY)
        possible_directions.push_back(UP);

    // DOWN
    if (i +1< portionSize+2 && portionMatrix[i+1][j] == EMPTY)
        possible_directions.push_back(DOWN);
    
    // Left
    if (j -1>= 1 && portionMatrix[i][j -1] == EMPTY)
        possible_directions.push_back(LEFT);

    // Right
    if (j + 1 < SIZE-1 && portionMatrix[i][j +1] == EMPTY  )
        possible_directions.push_back(RIGHT);
    
    //se il vettore è vuoto significa che la mia cella di gas non può spostarsi da nessuna parte
    if (possible_directions.empty())
        return;


    //qui sto scegliendo un numero random tra 0 e la size del vector
    //in modo random quindi mi sposterò
    //nota: srand dà problemi
    //srand(time(0));
    int rand_value = rand() % possible_directions.size();
    
    switch (possible_directions[rand_value]) 
    {
    case LEFT:
        move(portionMatrix, i, j, i, j-1);
        return;
    case RIGHT:
        move(portionMatrix, i, j, i, j+1);
        return;
    case UP:
        move(portionMatrix, i, j, i-1, j);
        return;
    case DOWN:
        move(portionMatrix, i, j, i+1, j);
        return;
    }
    
    
    
}

// Aggiorna lo stato di una cella, solo se � di tipo GAS,
// perch� le celle EMPTY e GHIACCIATE non modificano il proprio stato.
// NOTA: gli indici i,j corrispondono a una cella di tipo GAS. Il controllo
// viene effettuato prima di chiamare la funzione.
void update_automa(int** portionMatrix, const int& i, const int& j, const int& portionSize) 
{

    // Qua so che in pos. (i,j) ho una cella di tipo GAS
    if (becomes_ice(portionMatrix, i, j, portionSize)) 
    {
        portionMatrix[i][j] = ICE;
        return;
    }

    // Qua so che il GAS non diventa ICE, quindi
    // posso muoverlo
    move_gas(portionMatrix, i, j, portionSize);

}

void initialize_borders(int** map) 
{

    for (int index = 0; index < SIZE; index++) 
    {
        map[index][0] = ICE;        // Bordo Colonna Sinistra
        map[0][index] = ICE;        // Bordo Riga Superiore
        map[index][SIZE-1] = ICE;   // Bordo Colonna Destra
        map[SIZE-1][index] = ICE; // Bordo Riga Inferiore
        map[index][index]=ICE;
        
    }



}

void choose_random_gas(int** map) 
{

    for (int i = 1; i < SIZE-1; i++) 
    {
        for (int j = 1; j < SIZE-1; j++) 
        {
            map[i][j]=choose_gas_or_empty();
        }
    }

}

// Inizializza una mappa con i bordi ghiacciati
void initialize_map(int** map) 
{

    initialize_borders(map);
    choose_random_gas(map);

    map[SIZE / 2][SIZE / 2] = ICE; // Ghiaccio anche al centro
   

}


void delete_matrix(int** matrix, const int size) 
{
    for (int i = 0; i < size; i++)
        free(matrix[i]);
    free(matrix);
}


int main(int argc, char** argv)
{
    time_t start_t, end_t;
    time(&start_t);
    
    int step=0;

    while(step!=20000)
    {
        int **map = (int **)calloc(SIZE, sizeof(int*));
        for(int i = 0; i < SIZE; i++) 
        map[i] = (int *)calloc(SIZE,  sizeof(int));

        // inizializzo bordi e celle
        initialize_map(map);

        for(int i=0; i<SIZE; i++)
        {
            for(int j=0; j<SIZE; j++)
            {
                update_automa(map, i, j, SIZE);
            }
        }

        step++;
         delete_matrix(map, SIZE);

    }
    time(&end_t);
    
    

    double diff = difftime(end_t, start_t);
    
    printf("tempo %f", diff);

   


}