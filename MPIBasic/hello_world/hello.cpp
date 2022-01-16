#include <mpi.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char*argv[]) {
    MPI_Init(NULL, NULL);

    int processes, pnum;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    int len;
    
    MPI_Comm_size(MPI_COMM_WORLD, &processes);  
    MPI_Comm_rank(MPI_COMM_WORLD, &pnum);    
    MPI_Get_processor_name(proc_name, &len);

    printf("I am Process %d out of %d. I am running on machine %s.\n", pnum, processes, proc_name);
    MPI_Finalize();

}
