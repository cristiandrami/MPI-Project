#include<ctime>
#include<stdio.h>
#include <stdlib.h>
#include<mpi.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include<iostream>

#define numSteps 2
const double minValue=0.98;
const unsigned minNearbyLivingCells=2;
#define totalSize 300



enum states{ EMPTY = 0, ICE = 1, GAS = 2};


bool initCells()
{
    //se questa funziona mi ritorna true allora in modo random setto a viva una data cella, per inizializzare l'automa
    //mi ritorna true se il valore trovato è superiore del mio valore minimo
    //unsigned int seed=time(NULL);
    //double newValue=rand_r(&seed)/RAND_MAX;

    double newValue=((double) rand() / (RAND_MAX));

   // std::cout<<newValue<<std::endl;

    if(newValue>minValue)
        return true;

    return false;
}


bool iced(int **porzioneCorrente,int i, int j, int portionSize)
{
    if(j-1>=0 && porzioneCorrente[i][j-1]==ICE)
       return true;
    
    if(j+1<portionSize && porzioneCorrente[i][j+1]==ICE)
       return true;

    if(i+1<portionSize && porzioneCorrente[i+1][j]==ICE)
       return true;

    if(i-1>=0 && porzioneCorrente[i-1][j]==ICE)
       return true;


    return false;



}

void moveGas(int **porzioneCorrente,int i, int j, int **generazioneFutura, int portionSize)
{

    int count=0;

    //mi creo un valore pari a 0.23 e cerco di spostare il gas prima a sinistra, poi destra, su e giu, sempre se posso
    float newValue=(float)rand()/(float)RAND_MAX;

    std::cout<<newValue<<"valore";

    // se non posso perchè sforo o non è libera provo a metterlo a destra cambiando il newValue in modo da entrare nel''altro if
    while(count<=5)
    {
        if(newValue<=0.25)
        {
            if(j-1>=0 && porzioneCorrente[i][j-1]==EMPTY)
            {
                generazioneFutura[i][j-1]=GAS;
                generazioneFutura[i][j]=EMPTY;
                return;

            }
            else
            {
                newValue=0.26;
                count++;

            }
            
            

        }

        //se questo valore è tra 0.26 e 0.50 cerco di spostare il gas nella cella a destra(se questa è vuota)
        // se non posso perchè sforo o non è libera provo a metterlo a in alto cambiando il newValue in modo da entrare nel''altro if
        if(newValue>0.25 && newValue<=0.50)
        {
            if(j+1<portionSize && porzioneCorrente[i][j+1]==EMPTY)
            {
            
                generazioneFutura[i][j+1]=GAS;
                generazioneFutura[i][j]=EMPTY;
                return;

            }
            else
            {
                newValue=0.51;
                count++;

            }
            
            

        }

        //se questo valore è tra 0.51 e 0.75 cerco di spostare il gas nella cella in su(se questa è vuota)
        // se non posso perchè sforo o non è libera provo a metterlo a in basso cambiando il newValue in modo da entrare nel''altro if
        if(newValue>0.50 && newValue<=0.75)
        {
            if(i+1<portionSize && porzioneCorrente[i+1][j]==EMPTY)
            {
                generazioneFutura[i+1][j]=GAS;
                generazioneFutura[i][j]=EMPTY;
                return;

            }
            else
            {
                newValue=0.76;
                count++;

            }
            
            

        }

        //se questo valore è tra 0.76 e 1 cerco di spostare il gas nella cella in su(se questa è vuota)
        // se non posso perchè sforo o non è libera provo a metterlo a sinistra cambiando il newValue in modo da entrare nel''altro if

        if(newValue>0.75 && newValue<=1)
        {
            if(i-1>=0 && porzioneCorrente[i-1][j]==EMPTY)
            {
            
                generazioneFutura[i-1][j]=GAS;
                generazioneFutura[i][j]=EMPTY;
                return;
            } 
            else
            {
                newValue=0.1;
                count++;

            }      
            
            

        }

    }
    

    //se sono entrato nei quattro if allora mi trovo in una situazione ciclica in cui ho le celle su,sotto,destra,sinistra occupate
    //aallora la lascio dov'è
    //generazioneFutura[i][j]=porzioneCorrente[i][j];
   



}
//con questa funzione vado ad applicare proprio le regole del gioco in base ad ogni cella e ai suoi vicini
void refactCell(int **porzioneCorrente, int **generazioneFutura, int i, int j, int portionSize)
{
    switch (porzioneCorrente[i][j])
    {

    case GAS:
            //se è GAS vedo se ha vicino una cella ghiacciata, se si si ghiaccia anche lei;
            if(iced(porzioneCorrente, i, j, portionSize))
                generazioneFutura[i][j]=ICE;
            

            //ora sposto le particelle di gas;
            else
            moveGas(porzioneCorrente, i, j,generazioneFutura, portionSize);

            


        break;


    default:
        //rimane com'è
        generazioneFutura[i][j] = porzioneCorrente[i][j];
        break;
    }
}


void initMap(int** map)
{

    //con questa funzione inizializzo la mappa così:
    //-ogni bordo diventa una cella di ghiaccio
    //-scelgo la cella centrale e la faccio diventare ghiaccio
    bool ice=false;
    for(int i=0; i<totalSize; i++)
    {
        ice=false;
        
            for(int j=0; j<totalSize; j++)
            {
                ice=false;
                

                if(i==0)
                ice=true;

                if(j==0)
                ice=true;

                if(j==totalSize-1)
                ice=true;

                if(i==totalSize-1)
                ice=true;

                /*
                if(i==totalSize-1)
                    map[i][j]=ICE;
                if(j==totalSize-1)
                    map[i][j]=ICE;
                */
               if(ice)
               map[i][j]=ICE;
               else
               map[i][j]=EMPTY;

            }
    }

    map[totalSize/2][totalSize/2]=ICE;
    
    for(int i=0; i<totalSize; i++)
    {
    	for (int j=0; j<totalSize; j++)
    	{
    		
    		if(initCells() && map[i][j]==EMPTY)
    			map[i][j]=GAS;
    	}
    }
    

}

void deleteMap(int** map)
{
    for(int i=0; i<totalSize; i++)
    {
        delete[] map[i];
    }

    delete[] map;
}

void drawMap(int **map)
{
   al_clear_to_color(al_map_rgb(0, 0, 0));

    for (int i=0; i<totalSize; i++)
    {
        for (int j=0; j<totalSize; j++)
        {
            switch (map[i][j])
            {
            case EMPTY:
                al_draw_filled_rectangle(i * 3, j * 3, i * 3+3, j * 3+3, al_map_rgb(0, 0, 0));
                break;

            case ICE:
                al_draw_filled_rectangle(i * 3, j * 3, i * 3+3, j * 3+3, al_map_rgb(0,139,139));

                //std::cout<<i<<" "<<j<<std::endl;
                break;

            case GAS:
                al_draw_filled_rectangle(i * 3, j * 3, i *3+3, j * 3+3, al_map_rgb(255,255,0));
                break;
            }
        }
    }

    al_flip_display();
    al_rest(0.2);
}

int main(int argc, char** argv)
{
    int rank, numProc, portionSize;

    int continueAutoma=1;
    

   

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Request request;
    MPI_Status status;
    MPI_Request topRecvRequest;
    MPI_Request bottomSendRequest;
    MPI_Request bottomRecvRequest;
    MPI_Request topSendRequest;


    //se la mia matrice non è divisibile per il numero di processi non posso continuare
    if (totalSize%(numProc-1)!=0)
    {
        if (rank==0)
            printf("Invalid number of processes, please change the number \n");
        MPI_Finalize();
        return 0;
    }

    //divido la grandezza della matrice tra i vari processi (numero di righe)
    portionSize=totalSize/(numProc-1);


    std::cout<<"initial portion "<<portionSize<<std::endl;


    //ora  posso crearmi le sotto matrici di supporto che userò per inviare le parti ai vari processi
    // le creo di dimensione portionSize+2 in modo da poter sfruttare lo scambio di bordi
    int** portionMatrix = new int* [portionSize+2];
    for (int i=0; i <portionSize+2; i++)
    {
        portionMatrix[i] = new int[totalSize];
    }


    
    //questa mi serve per gestire le regole sulla nuova matrice (mappa)
    int **futureMatrix = new int *[totalSize];
    for (int i=0; i<totalSize; i++)
    {
        futureMatrix[i] = new int[totalSize];
    }







    if(rank==0)
    {
        int** map= new int*[totalSize];

        for(int i=0; i<totalSize; i++)
        {
            map[i]= new int[totalSize];
        }



        initMap(map);

        //qui mi setto il display in cui disegnerò la matrice
        ALLEGRO_DISPLAY *display;
        const int displayHeight = totalSize*3;
        const int displayWeight = totalSize*3;
        //qui invece mi creo un collegamento tra allegro e la tastiera con la quale potrò chiudere il programma
        ALLEGRO_KEYBOARD_STATE key_state;

        al_init();
        display = al_create_display(displayWeight, displayHeight);
        al_init_primitives_addon();

        

        if(!al_install_keyboard())
        {
            printf("Error with keyboard\n");
            MPI_Finalize();
            return -1;
        }

        while (continueAutoma==1)
        {
            
            //qui vedo se ho cliccato q sulla tastiera, se si devo chiudere il programma
            al_get_keyboard_state(&key_state);
            if (al_key_down(&key_state, ALLEGRO_KEY_Q))
            {
                continueAutoma=0;
            }

            //faccio sapere agli altri processi se devono smettere di fare operazioni
            for (int j=1; j<numProc; j++)
            {
                MPI_Send(&continueAutoma, 1, MPI_INT, j, 0, MPI_COMM_WORLD);

            }

            //utilizzo questa variabile per mandare prima il pezzo da 0 a portionSize poi portionSize fino a portionSize*2
            //e così via, gestisco gli invii
            int portionCount = 0;


            //per ogni processo
            for (int j = 1; j<numProc; j++)
            {
                //prendo tutte le righe che devo spedirgli
                for (int k = 0; k < portionSize; k++)
                {
                    //qui sto spedendo la singola riga ovvero da [k+portionCount][0] fino a [k+portionCount][totalSize]
                    //al processo j con tag j
                    MPI_Isend(&(map[k +portionCount][0]), totalSize, MPI_INT, j, 1, MPI_COMM_WORLD, &request);
                }
                //il processo successivo avrà la sua porzione
                portionCount += portionSize;
            }


            //disegno la mappa con allegro
            drawMap(map);


            std::cout<<"ho stampato la mappa\n";


            //qui ricevo da ogni processo la matrice modificata
            portionCount = 0;
            //per ogni processo
            for (int j=1; j < numProc; j++)
            {

                std::cout<<j<<std::endl;
                //prendo tutte le righe che devo ricevere
                for (int k=0; k<portionSize; k++)
                {
                    //qui ricevo la singola riga ovvero da [k+portionCount][0] fino a [k+portionCount][totalSize]
                    //dal processo j con tag j
                    MPI_Recv(&(map[k+portionCount][0]), totalSize, MPI_INT, j, 4, MPI_COMM_WORLD, &status);

                    
                }

                std::cout<<"ho ricevuto dal processo "<<j<<std::endl;

                portionCount += portionSize;
            }

            

                

        }

        al_destroy_display(display);
        deleteMap(map);
    }
        
           
           
           
           
           
           
    


    //se il rank è 1 a me serve ottenere solo il bordo inferiore da prossimo processo 
    else if(rank==1)
    {
        while (continueAutoma==1)
        {
             MPI_Recv(&continueAutoma, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

            std::cout<<"ho ricevuto dal processo 0 continue automa, sono "<<rank<<std::endl;
            

            for (int j=0; j<portionSize; j++)
            {
                MPI_Recv(&(portionMatrix[j][0]), totalSize, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            }
            std::cout<<"ho ricevuto dal processo 0 la parte di matrice, sono "<<rank<<std::endl;

            if((numProc-1)!=1)
            {
                //sto inviando l'ultima riga della mia matrice (il bordo inferiore) al prossimo processo
                MPI_Isend(&(portionMatrix[portionSize-1][0]), totalSize, MPI_INT, rank+1, 2, MPI_COMM_WORLD, &bottomSendRequest);

                //sto ricevendo la prima riga dal prossimo processo(il suo bordo superiore)
                MPI_Irecv(&(portionMatrix[portionSize][0]), totalSize, MPI_INT, rank+1, 3, MPI_COMM_WORLD, &bottomRecvRequest);

                //aspetto che lo scambio sia eseguito 
                MPI_Wait(&bottomSendRequest, &status);
                std::cout<<"ho inviat dal processo"<<rank+1<<" il bordo inferiore, sono "<<rank<<std::endl;
                MPI_Wait(&bottomRecvRequest, &status);
                std::cout<<"ho ricevuto dal processo"<<rank+1<<" il bordo inferiore, sono "<<rank<<std::endl;

                


            }

            


            //per pgni riga esclusa la prima e l'ultima che sono arrivate da altri precedente vado a modificare le celle
            for (int j=1; j<portionSize; j++)
                for (int k=0; k<totalSize;k++)
                refactCell(portionMatrix, futureMatrix, j, k, portionSize);
                    

                    std::cout<<"portion "<<portionSize<<std::endl;

            //spedisco le celle modificate al processo 0 
            for (int j=0; j<portionSize; j++)
            {
                std::cout<<portionSize;
                MPI_Send(&(futureMatrix[j][0]), totalSize, MPI_INT, 0, 4, MPI_COMM_WORLD);
            }

            std::cout<<"ho inviato al processo 0 la matrice, sono "<<rank<<std::endl;
        }
           
    
        
           
    }

    //se sono l'ultimo processo mi serve ottenere solo il bordo superiore
    else if (rank== numProc-1)
    {
        while(continueAutoma==1)
        {
            MPI_Recv(&continueAutoma, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            std::cout<<"ho ricevuto dal processo 0 continue automa, sono "<<rank<<std::endl;

            for (int j=1; j<portionSize+1; j++)
            {
                MPI_Recv(&(portionMatrix[j][0]), totalSize, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            }

            
            std::cout<<"ho ricevuto dal processo 0 la parte di matrice, sono "<<rank<<std::endl;

            //spedisco al processo precedente la mia parte superiore
            MPI_Isend(&(portionMatrix[1][0]), totalSize, MPI_INT, rank-1, 3, MPI_COMM_WORLD, &topSendRequest);
            
            //ricevo dal processo precedente la sua parte inferiore (che sarà la mia parte superiore)
            MPI_Irecv(&(portionMatrix[0][0]), totalSize, MPI_INT, rank-1, 2, MPI_COMM_WORLD, &topRecvRequest);

            MPI_Wait(&topSendRequest, &status);
            std::cout<<"ho inviato al processo "<<rank-1<<" il bordo superiore, sono "<<rank<<std::endl;
            MPI_Wait(&topRecvRequest, &status);
            std::cout<<"ho ricevuto dal processo "<<rank-1<<" il bordo superiore, sono "<<rank<<std::endl;


            
            
            //parto sempre dalla prima riga di mia proprità e arrivo questa volta proprio all'ultima 
            for (int j=1; j<portionSize+1; j++)
                for (int k = 0; k < totalSize; k++)
                    refactCell(portionMatrix, futureMatrix, j, k, portionSize);
                   


            //spedisco le celle modificate al processo 0;
            for (int j=1; j<portionSize+1; j++)
            {
                MPI_Send(&(futureMatrix[j][0]), totalSize, MPI_INT, 0, 4, MPI_COMM_WORLD);
            }

            std::cout<<"ho inviato al processo 0 la matrice, sono "<<rank<<std::endl;

        }

            

        
        
    }

    else
    {
        
        while(continueAutoma==1)
        {
            MPI_Recv(&continueAutoma, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            std::cout<<"ho ricevuto dal processo 0 continue automa, sono "<<rank<<std::endl;

            for (int j = 1; j < portionSize+1; j++)
            {
                MPI_Recv(&(portionMatrix[j][0]), totalSize, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            }
            std::cout<<"ho ricevuto dal processo 0 la parte di matrice, sono "<<rank<<std::endl;


            //invio il bordo superiore
            MPI_Isend(&(portionMatrix[1][0]), totalSize, MPI_INT, rank-1, 3, MPI_COMM_WORLD, &topSendRequest);

            //invio il bordo inferiore
            MPI_Isend(&(portionMatrix[portionSize][0]), totalSize, MPI_INT, rank+1, 2, MPI_COMM_WORLD, &bottomSendRequest);

            //ricevo il mio bordo superiore
            MPI_Irecv(&(portionMatrix[0][0]), totalSize, MPI_INT, rank-1, 2, MPI_COMM_WORLD, &topRecvRequest);

            //ricevo il mio borodo inseriore
            MPI_Irecv(&(portionMatrix[portionSize+1][0]), totalSize, MPI_INT, rank+1, 3, MPI_COMM_WORLD, &bottomRecvRequest);

            MPI_Wait(&topSendRequest, &status);
            std::cout<<"ho inviato al processo "<<rank-1<<" il bordo superiore, sono "<<rank<<std::endl;
            MPI_Wait(&bottomSendRequest, &status);
            std::cout<<"ho inviato al processo "<<rank-1<<" il bordo inferiore, sono "<<rank<<std::endl;
            MPI_Wait(&topRecvRequest, &status);
            std::cout<<"ho ricevuto dal processo "<<rank-1<<" il bordo superiore, sono "<<rank<<std::endl;
            MPI_Wait(&bottomRecvRequest, &status);
            std::cout<<"ho ricevuto al processo "<<rank-1<<" il bordo inferiore, sono "<<rank<<std::endl;


            //in questo caso mi tocca vedere dal bordo superiore(tolta la riga ricevuta) fino al bordo inferiore(tolta la riga ricevuta)
            for (int j=1; j<portionSize+1;j++)
                for (int k=0; k< portionSize; k++)
                    refactCell(portionMatrix, futureMatrix, j, k, portionSize);
                    
                    

            //invio la matrice modificata
            for (int j= 1; j< portionSize+1; j++)
            {
                MPI_Send(&(futureMatrix[j][0]), totalSize, MPI_INT, 0, 4, MPI_COMM_WORLD);
            }

            std::cout<<"ho inviato al processo 0 la matrice, sono "<<rank<<std::endl;

        }
            

        
            
        
    }

    
    for (int i=0; i< portionSize+2; i++)
    {
        delete[] portionMatrix[i];
    }
    delete[] portionMatrix;

    for (int i=0; i<totalSize; i++)
    {
        delete[] futureMatrix[i];
    }
    delete[] futureMatrix;

    MPI_Finalize();

    
}