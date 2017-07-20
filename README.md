# Prime-Generator



MPI Prime Finder

By Gaurav Dass
@dassg

I chose to design this program in a simple fashion, I chose the primality test to determine weather a number was prime. The actual counting was done externally, except that it was done in batches of 10. Each seuquence (0 - 10, 11 -100, 101 - 1000, 1001 - 10000) is broken into many sub series, which are then iterated by the number of processes The prime number count for each sub series is summed up at the end of each bracket. This breaks up the computation by having each process do part of it. The key required to get the output get printed out (at least on my system) was asking MPI the error handler to not automatically shut processes down, instead letting us (the user) do it via 

                                                            MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
                                                            
Turns out, MPI automatically kills intermediate processes if they encounter any errors. This may cause Rank 0 to potentially wait forever on whatever it is doing, to not hear from one of the processes.
                                                            
In case of a timeout a user interrupt is sent to the counter, which then stops what it is doing. A sum of the latest prime number count is made from prime counts of all the ranks. After this, all ranks send their latest traversed number to rank 0, which finds out the largest one from it. It displays this as the last traversed number along with the prime count. 

Primes counted for a timeout of 5 seconds (# Primes counted within 5 seconds):

1 core:         244510
2 cores:        260300
3 cores:        351894
4 cores:        312023
5 cores:        354745
6 cores:        217187
7 cores:        364644
8 cores:        281790
9 cores:        307374
10 cores:       295324 
11 cores:       372601 
12 cores:       240097 
13 cores:       379975 
14 cores:       275057 
15 cores:       308095 
16 cores:       317073 
17 cores:       367376 
18 cores:       229560 
19 cores:       368030 
20 cores:       228336 
