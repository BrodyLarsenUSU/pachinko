
#include <iostream>
#include <mpi.h>
#include <Windows.h>
#include <list>
#include <random>
#define MCW MPI_COMM_WORLD
using namespace std;

//random chooses between two inputs
int leftOrRight(int a, int b)
{
    int r = rand() % 2;

    if (r == 0) {
        return a;
    }
    else {
        return b;
    }
}

int main(int argc, char** argv) {
    int rank, size;
    int ball[2];
    int pin = 0;
    int levelAt = 1;
    int bucket[8] = { 0,0,0,0,0,0,0,0 };

    bool running = true;


    int dest;
    srand(time(0));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);   

    //"drop" 100 pachinko balls
    for (int q = 0; q < 100; q++) {
        bool running = true;

        // start by dropping ball on pin 4
        if (!rank) {
            ball[pin] = 4;
            ball[levelAt] = 8;
            MPI_Send(&ball, 2, MPI_INT, ball[pin], 0, MCW);
        }
        
        while (running) {
            MPI_Recv(&ball, 2, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
            ball[levelAt] = ball[levelAt] - 1;

            // processors that dont recieve ball break out of while loop
            if (ball[levelAt] == -2) {
                running = false;
            }

            //if levelAt == 0, then ball hass reached the bottom
            else if (ball[levelAt] == 0) {
                running = false;
            }
            else {
                ball[pin] = leftOrRight(ball[pin] - 1, ball[pin] + 1); //drop either left or right
                //catch statement to prevent ball from dropping off the board
                if (ball[pin] == 0) ball[pin] = 2;
                if (ball[pin] == 8) ball[pin] = 6;
                MPI_Send(&ball, 2, MPI_INT, ball[pin], 0, MCW);
            }
        }
        // only the processor that has the ball enters this if statement
        if (rank == ball[pin]) {
            dest = leftOrRight(ball[pin]-1, ball[pin]);
            bucket[dest] = bucket[dest] + 1;

            //after putting ball in a bucket, send out messege to cause the rest
            //of the processors to break out of while loop
            for (int h = 0; h < size; h++) {

                //this do nothing if statement prevents processor from sending a messege to itself
                if (h == rank) {
                    //do nothing
                }
                else {

                    ball[levelAt] = -1;
                    MPI_Send(&ball, 2, MPI_INT, h, 0, MCW);
                }
            }
        }
        else{
        //do nothing
        }
    }

    //let user know when processor is done
    cout << "rank: "<< rank <<" is done" << endl;

    MPI_Finalize();
         
    //print out how many balls are in each bin of the bucket list
    for (int i = 0; i <= 7; i++) {
        if (bucket[i] != 0) {
            cout << rank << "-bucket bin " << i << " contains " << bucket[i] << " balls" << endl;
        }
    }
           
        
    
    return 0;
}









