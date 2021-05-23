#include<ctime>
#include<stdio.h>
#include <stdlib.h>
#include<mpi.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include<iostream>
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

    std::cout<<newValue<<std::endl;

    if(newValue>minValue)
        return true;

    return false;
}

/*
bool iced(int **porzioneCorrente,int i, int j)
{
    if(j-1>=0 && porzioneCorrente[i][j-1]==ICE)
       return true;
    
    if(j+1<portionSize && porzioneCorrente[i][j+1]==ICE)
       return true;

    if(i+1<portionSize && porzioneCorrente[i+1][j]==ICE)
       return true;

    if(i-1>=0 && porzioneCorrente[i-1][j]==ICE)
       return true;



}

void moveGas(int **porzioneCorrente,int i, int j, int **generazioneFutura)
{

    //mi creo un valore pari a 0.23 e cerco di spostare il gas prima a sinistra, poi destra, su e giu, sempre se posso
    double newValue=0.23;

    // se non posso perchè sforo o non è libera provo a metterlo a destra cambiando il newValue in modo da entrare nel''altro if

    if(newValue<=0.25)
    {
        if(j-1>=0 && porzioneCorrente[i][j-1]==EMPTY)
        {
            generazioneFutura[i][j-1]=GAS;
            return;

        }
        else
        newValue=0.26;
        

    }

    //se questo valore è tra 0.26 e 0.50 cerco di spostare il gas nella cella a destra(se questa è vuota)
    // se non posso perchè sforo o non è libera provo a metterlo a in alto cambiando il newValue in modo da entrare nel''altro if
    if(newValue>0.25 && newValue<=0.50)
    {
        if(j+1<portionSize && porzioneCorrente[i][j+1]==EMPTY)
        {
       
            generazioneFutura[i][j+1]=GAS;
            return;

        }
        else
        newValue=0.51;
        

    }

    //se questo valore è tra 0.51 e 0.75 cerco di spostare il gas nella cella in su(se questa è vuota)
    // se non posso perchè sforo o non è libera provo a metterlo a in basso cambiando il newValue in modo da entrare nel''altro if
    if(newValue>0.50 && newValue<=0.75)
    {
        if(i+1<portionSize && porzioneCorrente[i+1][j]==EMPTY)
        {
            generazioneFutura[i+1][j]=GAS;
            return;

        }
        else
        newValue=0.76;
        

    }

    //se questo valore è tra 0.76 e 1 cerco di spostare il gas nella cella in su(se questa è vuota)
    // se non posso perchè sforo o non è libera provo a metterlo a sinistra cambiando il newValue in modo da entrare nel''altro if

    if(newValue>0.75 && newValue<=1)
    {
        if(i-1>=0 && porzioneCorrente[i-1][j]==EMPTY)
        {
       
            generazioneFutura[i-1][j]=GAS;
            return;

        }       
        
        

    }

    //se sono entrato nei quattro if allora mi trovo in una situazione ciclica in cui ho le celle su,sotto,destra,sinistra occupate
    //aallora la lascio dov'è
    generazioneFutura[i][j]=porzioneCorrente[i][j];
   



}
//con questa funzione vado ad apprlicare proprio le regole del gioco in base ad ogni cella e ai suoi vicini
void refactCell(int **porzioneCorrente, int **generazioneFutura, int i, int j)
{
    switch (porzioneCorrente[i][j])
    {

    case GAS:
            //se è GAS vedo se ha vicino una cella ghiacciata, se si si ghiaccia anche lei;
            if(iced(porzioneCorrente, i, j))
                generazioneFutura[i][j]=ICE;
            

            //ora sposto le particelle di gas;
            moveGas(porzioneCorrente, i, j,generazioneFutura);

            


        break;


    default:
        //rimane com'è
        generazioneFutura[i][j] = porzioneCorrente[i][j];
        break;
    }
}
*/

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
    al_rest(0.3);
}

int main(int argc, char** argv)
{
    int rank, numProc;
    

    int done = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);



    if (totalSize%numProc!=0)
    {
        if (rank == 0)
            printf("Invalid number of processes\n");
        MPI_Finalize();
        return 0;
    }


    if(rank==0)
    {
        int** map= new int*[totalSize];

        for(int i=0; i<totalSize; i++)
        {
            map[i]= new int[totalSize];
        }



        initMap(map);

        ALLEGRO_DISPLAY *display;
        const int displayHeight = totalSize;
        const int displayWeight = totalSize;
        //const int lato = 10;
        al_init();
        display = al_create_display(displayWeight*3, displayHeight*3);
        al_init_primitives_addon();

        while(true)
        {
           drawMap(map);

           //refactCell(map);
           //break;
        }

        



        deleteMap(map);
    }

    MPI_Finalize();

}