#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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
    map[SIZE / 3][SIZE / 3] = ICE; // Ghiaccio anche al centro

}


void delete_matrix(int** matrix, const int size) 
{
    for (int i = 0; i < size; i++)
        free(matrix[i]);
    free(matrix);
}


const unsigned DIM = 3; // dimensione blocchi da disegnare

void draw_map(int** map) 
{
    int count=0;

    const ALLEGRO_COLOR BLACK = al_map_rgb(0, 0, 0);
    const ALLEGRO_COLOR CYAN = al_map_rgb(0, 139, 139);
    const ALLEGRO_COLOR YELLOW = al_map_rgb(255, 255, 0);

    al_clear_to_color(al_map_rgb(0,0,0)); // provare a toglierlo

    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            
            
            switch (map[i][j]) {

            case EMPTY:
                al_draw_filled_rectangle(j*DIM, i*DIM, j* DIM + DIM, i * DIM + DIM, BLACK);
                break;

            case ICE:
                al_draw_filled_rectangle(j*DIM, i*DIM, j* DIM + DIM, i * DIM + DIM, CYAN);
                break;

            case GAS:
                al_draw_filled_rectangle(j*DIM, i* DIM, j* DIM + DIM, i * DIM + DIM, YELLOW);
                break;

            }

        }
            
    }
    al_flip_display();
    const double REST = 0.2;
    al_rest(REST);

}

int main(int argc, char** argv) 
{

    int step=0;

    int rank; // rank del processo corrente
    int num_procs; // numero processi totale
    int portionSize; // numero righe sotto matrice gestita dal processo corrente

    // 1 se il programma continua la sua esecuzione
    // 0 se terminiamo il programma ('q')
    int continue_automa= 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Se non posso dividere la matrice in modo tale che
    // ogni processo abbia matrici di stesse dimensioni,
    // termino il programma.
    // NOTA: Il processo 0 si occupa SOLO di disegnare (per questo il -1)
    if (SIZE % (num_procs - 1) != 0) 
    {
        if (rank == 0)
            printf("Numero processi %i non valido. Riprovare.\n", num_procs);

        MPI_Finalize();
        return -1;
    }

    const int CONTINUE_AUTOMA_TAG = 0;
    const int OLD_MAP_SEND_RCV_TAG = 1;
    const int NEW_MAP_SEND_RECV_TAG = 4;
    const int TOP_BORDER_SEND_RECV = 3;
    const int BOTTOM_BORDER_SEND_RECV = 2;

    MPI_Status status;
    MPI_Request request;

    // Receive
    MPI_Request bottomRecvRequest;
    MPI_Request topRecvRequest;

    // Send
    MPI_Request bottomSendRequest;
    MPI_Request topSendRequest;

    

    // Calcolo il numero di righe di sotto_matrici che
    // ogni processo avr� (tranne il processo 0)
    portionSize = SIZE / (num_procs-1);

    // Creo un nuovo tipo di dato, formato da 
    // una riga contigua di SIZE-colonne di interi
    MPI_Datatype rowDataType;
    MPI_Type_contiguous(SIZE, MPI_INT, &rowDataType);
    MPI_Type_commit(&rowDataType);


    /********* PROCESSO 0 *********/
    // Se sono il processo 0, creo la mappa e la disegno,
    // e gestisco le comunicazioni tra i processi
    if (rank == 0) 
    {
        double start=MPI_Wtime();

        int **map = (int **)calloc(SIZE, sizeof(int*));
        for(int i = 0; i < SIZE; i++) 
            map[i] = (int *)calloc(SIZE,  sizeof(int));

        // inizializzo bordi e celle
        initialize_map(map);


        while (step!= 20000) 
        {

            



            // spedisco a tutti i processi contnue_automa per informarli su cosa devono fare, se continuare o meno l'esecuzione
            MPI_Bcast(&continue_automa,1,MPI_INT, 0, MPI_COMM_WORLD);
               



            /********* SPEDIZIONE *********/
            // Il processo 0 spedisce le righe ai vari processi,
            // usando una variabile rank_portion, per calcolare
            // i giusti valori da spedire ad ogni processo ed aggiornare la mappa
            int rank_portion = 0;
            for (int rank1 = 1; rank1<num_procs; rank1++) 
            {
                // spedisco tutte le righe di un processo al processo stesso
                for (int i = 0; i < portionSize; ++i) 
                {
                    // singola linea da spedire
                    MPI_Isend(map[i + rank_portion], 1, rowDataType, rank1, OLD_MAP_SEND_RCV_TAG, MPI_COMM_WORLD, &request);
                    //MPI_Send(map[i + rank_portion], 1, rowDataType, rank, OLD_MAP_SEND_RCV_TAG, MPI_COMM_WORLD);
                }
                rank_portion += portionSize; // per il prossimo processo
            }

            //draw_map(map);




            /********* RICEZIONE *********/
            // Il processo 0 riceve la mappa aggiornata
            rank_portion = 0;
            for (int rank1 = 1; rank1 < num_procs; rank1++) 
            {
                for (int i = 0; i < portionSize; i++) 
                {
                    MPI_Recv(map[i + rank_portion], 1, rowDataType, rank1, NEW_MAP_SEND_RECV_TAG, MPI_COMM_WORLD, &status);
                    
                }
                rank_portion += portionSize; // per il prossimo processo
            }
            step++;

        } // while (continue_automa)
        double end=MPI_Wtime();
        printf("time %f ", end-start);


       
        delete_matrix(map, SIZE);

    } // if rank == 0

    // Qua dentro entrano tutti i processi tranne 0.
    // Prima di compiere le operazioni comuni, controllo che
    // i processi 1 e num_procs-1 effetutino i dovuti scambi di righe
    else 
    {

        // sottomatrice della matrice della mappa,
        // che ogni processo deve gestire
        int **portionMatrix = (int **)calloc((portionSize+2), sizeof(int*));

        for(int i = 0; i < portionSize+2; i++) 
        portionMatrix[i] = (int *)calloc(SIZE, sizeof(int));

        //2 righe bordo sup e inf
        int* sup= (int*) calloc(SIZE, sizeof(int*));  //la riga 1 del processo dopo di me
        int* inf= (int*) calloc(SIZE, sizeof(int*)); //l'ultima riga del processo prima di me
            

        while (step!= 20000) 
        {

            // Ricevo la variabile continue_automa da parte del processo 0
            MPI_Bcast(&continue_automa,1,MPI_INT, 0, MPI_COMM_WORLD);

            // Ricevo le righe della vecchia matrice (da 1 a n-2, perch� 0 ed n-1
            // devo riceverle da altri processi)
            for (int i = 1; i < portionSize + 1; i++)
            {
                MPI_Recv(portionMatrix[i], 1, rowDataType, 0, OLD_MAP_SEND_RCV_TAG, MPI_COMM_WORLD, &status);
            }
          
            
               

            int bottom_des; // destinatario bordo inferiore
            int top_des; // destinatario bordo superiore

            // Controlliamo se il rango � 1 oppure n-1,
            // per gesitre le spedizioni di bordi che
            // non dovrebbero essere spediti
            if (rank == 1) 
            {
                //se sono il processo 1 inivio la prima riga (ICE) all'ultimo processo
                // e l'ultima riga al processo 2
                
               bottom_des = rank+1; 
               top_des = num_procs-1; 
                
 
            } 
            else if (rank == num_procs-1) 
            {
                //se sono il processo num_procs-1 inivio la prima riga al processo  (num_procs-1)-1
                // e l'ultima riga al processo 1 (ICE)

                bottom_des = 1;
                top_des = rank-1;
            } 
            else 
            {
                //altrimenti  invio inivio la prima riga al processo rank-1
                // e l'ultima riga al processo rank+1
                bottom_des = rank+1;
                top_des = rank-1;
            }

          

            //NOTA BENE: IL PROCESSO 1 MANDA LA SUA PRIMA RIGA AL PROCESSO N-1 E IL PROCESSO N-1 MANDA LA SUA ULTIMA RIGA AL PROCESSO 1
            // CIO' FUNZIONA CON QUESTO APPROCCIO PERCHÈ OGNI PROCESSO HA UNA SOTTO MATRICE DELLA RISPETTIVA portionSize CON L'AGGIUNTA DI 2 RIGHE
            // INOLTRE IN QUESTO CASO LA PRIMA RIGA DEL PRIMO PROCESSO E L'ULTIMA DEL PROCESSO N-1 SONO ENTRAMBE ICE E NON HO NESSUN PROBLEMA

            // invio la mia prima riga, che sarà il bordo inferiore del processo precedente
            MPI_Isend(portionMatrix[1], 1, rowDataType, top_des, TOP_BORDER_SEND_RECV, MPI_COMM_WORLD, &topSendRequest);

            // invio la mia ultima riga, che sarà il bordo superiore del prossimo processo
            MPI_Isend(portionMatrix[portionSize], 1, rowDataType, bottom_des, BOTTOM_BORDER_SEND_RECV, MPI_COMM_WORLD, &bottomSendRequest);

            // ricevo il mio bordo superiore ovvero la l'ultima riga della sottomatrice del processo precedente
            MPI_Irecv(portionMatrix[0], 1, rowDataType, top_des, BOTTOM_BORDER_SEND_RECV, MPI_COMM_WORLD, &topSendRequest);

            // ricevo il mio bordo inferiore ovvero la prima riga della sottomatrice del processo successivo
            MPI_Irecv(portionMatrix[portionSize + 1], 1, rowDataType, bottom_des, TOP_BORDER_SEND_RECV, MPI_COMM_WORLD, &bottomSendRequest);

            MPI_Wait(&topSendRequest, &status);
            MPI_Wait(&bottomSendRequest, &status);

            topSendRequest=MPI_REQUEST_NULL;
            bottomSendRequest=MPI_REQUEST_NULL;
            //MPI_Wait(&topRecvRequest, &status);
            //  MPI_Wait(&bottomRecvRequest, &status);

            // aggiorno lo stato delle celle che non fanno parte dei bordi sup,inf
            for (int i=1; i<portionSize+1; i++)
            {
                for (int j=0; j<SIZE; j++)
                {
                    if (portionMatrix[i][j] == GAS)
                    {
                        update_automa(portionMatrix, i, j, portionSize);
                    }
                        

                }
                    
            }

          // invio la mia prima riga, che sarà il bordo inferiore del processo precedente
            MPI_Isend(portionMatrix[0], 1, rowDataType, top_des, TOP_BORDER_SEND_RECV, MPI_COMM_WORLD, &topSendRequest);

            // invio la mia ultima riga, che sarà il bordo superiore del prossimo processo
            MPI_Isend(portionMatrix[portionSize+1], 1, rowDataType, bottom_des, BOTTOM_BORDER_SEND_RECV, MPI_COMM_WORLD, &bottomSendRequest);

            // ricevo il mio bordo superiore ovvero la l'ultima riga della sottomatrice del processo precedente
            MPI_Irecv(sup, 1, rowDataType, top_des, BOTTOM_BORDER_SEND_RECV, MPI_COMM_WORLD, &topSendRequest);

            // ricevo il mio bordo inferiore ovvero la prima riga della sottomatrice del processo successivo
            MPI_Irecv(inf, 1, rowDataType, bottom_des, TOP_BORDER_SEND_RECV, MPI_COMM_WORLD, &bottomSendRequest);


            MPI_Wait(&topSendRequest, &status);
            MPI_Wait(&bottomSendRequest, &status);
            //MPI_Wait(&topRecvRequest, &status);
            //MPI_Wait(&bottomRecvRequest, &status);

            topSendRequest=MPI_REQUEST_NULL;
            bottomSendRequest=MPI_REQUEST_NULL;

            
  
            for(int j=0; j<SIZE; j++)
            {
               
                //std::cout<<inf[j]<<" ";
                if(portionMatrix[1][j]==EMPTY)
                portionMatrix[1][j]=sup[j];
                else if(portionMatrix[1][j]!=ICE)
                portionMatrix[1][j]=EMPTY;
               
                if(portionMatrix[portionSize][j]==EMPTY )
                portionMatrix[portionSize][j]=inf[j];
                else if(portionMatrix[portionSize][j]!=ICE)
                portionMatrix[portionSize][j]=EMPTY;
                
            }
           
            // invio la matrice modificata
            for (int i=1; i<portionSize+1; i++)
            {
                MPI_Send(portionMatrix[i], 1, rowDataType, 0, NEW_MAP_SEND_RECV_TAG, MPI_COMM_WORLD);
            }
            step++;
                
            

        }
        free(sup);
        free(inf);

        delete_matrix(portionMatrix, portionSize+2);

    }

    MPI_Type_free(&rowDataType);
    MPI_Finalize();

} // main