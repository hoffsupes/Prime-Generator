//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
// Simple Prime Finder
// By Gaurav Dass
// @dassg
/////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////
#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>

int killsignal = 0;

int isprime(unsigned int n)            // a simple prime detector
{                                       // using something like the seive of erastho would require a creation of a whole binary list till N, something I dont think should be done when counting to 4294967291
    int i = 0;

    for (i = 2; i <= sqrt(n); i++ )
    {
        if (n%i == 0)
        {
        return 0;
        }

    }

    return 1;
}

void tring_tring(int bye)               // SIGNAL HANDLER
{
    if (bye == SIGUSR1)
    {
    killsignal = 1;
    }
}

int main(int argc, char *argv[])        // main
{
MPI_Init(&argc,&argv);                  // MPI_Init initializing the MPI
int pross, rank,prime_count = 0,count = 0;
unsigned int end = 4294967291;          // LARGEST UNSIGNED 32 BIT PRIME

MPI_Comm_size(MPI_COMM_WORLD,&pross);       // obtain number of processes
MPI_Comm_rank(MPI_COMM_WORLD,&rank);        // obtain total ranks
MPI_Status status;                          // status: used for MPI_Recv
signal(SIGUSR1,tring_tring);                // signal for user interrupt
MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);         // this was KEY in getting avoiding a condition wherein MPI_Recv would wait forever on process 0
                                                               // while other another process would exit, this would make it seem like the program was never exiting even // though MPI finalize was being called
if(rank == 0)                                                  // Error Handling in MPI is very important otherwise MPI just kills any intermediate process where there is      
                                                               // an error
{
    printf("\n       N      Primes \n");
}

unsigned int pix;
int ival = 2 + rank;                        // INITIALIZE STARTING POINT FOR COUNTING PRIMES
for(pix = 10;pix <= end; pix*=10)           // counting in ODD arithmetic sequences with endpoints of multiples of 10
{  
    MPI_Bcast(&pix,1,MPI_INT,0,MPI_COMM_WORLD);         // MPI Broadcast to all processes current count number
    unsigned int i;
    
    for(i = ival; i < pix; i+= pross )   // Complete sequences are of forms 0:10, 11:100, 101: 1000 etc. Each seqeunce is broken into multiple sequences 
    {                                                  // which are then divided amongst slave processes and the master for counting
        count += isprime(i);

        if(killsignal)                                  // SIGUSR1 OBTAINTED
        {
        break;
        }
    }
  
    ival  = i;                                                              // ival set to new value (THIS ENSURES THAT THE COUNT STARTS FROM PREVIOUSLY STOPPED VALUE FOR A 
                                                                            // GIVEN RANK, saves computation)
    MPI_Reduce(&count,&prime_count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);     // reduce ADDS up all of the prime counts from the sub-arithmetic sequences from the slave                      
                                                                            // processes
    MPI_Barrier(MPI_COMM_WORLD);                                            // MAKE SURE ALL PROCESSES ENTER THIS PLACE TOGETHER
                                                                            // YOU'LL NOTE THAT SINCE THIS IS WHERE THE PROGRAM EXITS, HENCE I HAVE MADE SURE THAT THIS
        if (rank == 0 && killsignal == 0)                                   // PLACE IS EXPLICITLY SYNCHRONIZED BY DUAL BARRIERS
        {
        printf("\n %8d  %10d",pix,prime_count);                             // displaying in multiples of 10
        }

    if(killsignal)                                                          // on obtaining the killsignal, the final prime count is reduced once more above 
    {
       
        if(rank != 0)                                                       // while printing the the primes found by ALL the slave ranks are passed to rank 0
        { 
            int errco = MPI_Send(&i,1,MPI_INT,0,1,MPI_COMM_WORLD);                      // killsignal obtained, stopping all operations and sends LAST FOUND PRIME TO rank 0
            
              if (errco != MPI_SUCCESS)                                     // handling error, for some reason printf never gives any output in my machine
            {

            char err_stri[BUFSIZ];
            int errlen, err_cla;

            MPI_Error_class(errco, &err_cla);
            MPI_Error_string(err_cla, err_stri, &errlen);
            printf("\n MPI_ERROR CLASS for rank %d %s\n", rank, err_stri);
            MPI_Error_string(errco, err_stri, &errlen);
            printf("\n MPI_ERROR CODE for rank %d: %s\n", rank, err_stri);
            }
            
        }
        else
        {
            int u = 0, pol;
            for(u = 1; u < pross; u++)
            {
              int errco = MPI_Recv(&pol,1,MPI_INT,u,1,MPI_COMM_WORLD,&status);          // receive all the LAST TRAVERSED NUMBER from all the other ranks
            
              if (errco != MPI_SUCCESS)  // handling error, for some reason printf never gives any output in my machine
            {

            char err_stri[BUFSIZ];
            int errlen, err_cla;

            MPI_Error_class(errco, &err_cla);
            MPI_Error_string(err_cla, err_stri, &errlen);
            printf("\n MPI_ERROR CLASS for rank %d %s\n", rank, err_stri);
            MPI_Error_string(errco, err_stri, &errlen);
            printf("\n MPI_ERROR CODE for rank %d: %s\n", rank, err_stri);
            }
              
              
              if (i < pol || i == pix)                                                  // DETERMINE THE LARGEST COUNTED NUMBER, this WOULD BE THE NUMBER WHICH WOULD BE THE LATEST ONE
              {
                 i = pol;                                                   // SINCE variable i is used, it literally checks against its own value for any signs of minimality
              }
            
            }

        }
        MPI_Barrier(MPI_COMM_WORLD);                                    // MAKE SURE ALL PROCESSES EXIT TOGETHER
        if(rank == 0) { printf("\n %8d  %10d\n",i,prime_count);}          // USE ONLY rank 0 for printing the FINAL PRIME COUNT WHICH WAS ACCUMULATED ONE LAST TIME and the 
                                                                        // LATEST NUMBER TRAVERSED as determined from information obtained from all the ranks before
        MPI_Finalize();                                                     // MPI_Finalize needs to be called or else MPI flushes your screen up a verbose error message
       return 0;                                                            // Exit with code(0) because non-zero exit codes can be problematic (sometimes)
    }
    
        
}

MPI_Finalize();                                         // If you can wait till the program counts till 4294967291 then it'll exit here

return 0;
}
