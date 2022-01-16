#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

double total_sum;
void method(float (*f)(float, int), float a, float b,float n, int intensity, int rank, int processes){
	int beg = n*rank/processes;
	int end = n*(rank+1)/processes;
	double sum = 0.0;
	
	if(rank == processes-1)
		end = n;
	for(int i=beg; i<end; i++){
		sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	}
	MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
}

int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  MPI_Init(&argc, &argv);
  MPI_Barrier(MPI_COMM_WORLD);
  int processes;
  MPI_Comm_size(MPI_COMM_WORLD, &processes);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
 
  int id = atoi(argv[1]);
  float a = atoi(argv[2]);
  float b = atoi(argv[3]);
  float n = atoi(argv[4]);
  int intensity = atoi(argv[5]);
  float sum = 0.0;  
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  switch(id){
  	case 1 : method(f1,a,b,n,intensity, rank, processes);
  		break;
  	case 2 : method(f2,a,b,n,intensity, rank, processes);
  		break;
  	case 3 : method(f3,a,b,n,intensity, rank, processes);
  		break;
  	case 4 : method(f4,a,b,n,intensity, rank, processes);
  		break;
  	default: return 0;
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();

  if(rank==0){
    std::cout << total_sum << std::endl;
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapased_seconds = end-start;
    std::cerr<<elapased_seconds.count()<<std::endl;
 
  }
  return 0;
}
