#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif
    void generateReduceData (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

void reduce(int arr[], int n, int gran, int th[]){
	int x = omp_get_thread_num();
	
	for(int i=0; i<n; i+=gran){
        	int start=i, end=i+gran-1;
        	if(end>n){
          		end = n-1;
        	}
	#pragma omp task
	{
	  int sum2=0;
	  for(int j=start; j<=end; j++){
	    sum2+=arr[j];
	  } 
	  th[x] += sum2;
        }
      }
}

int main (int argc, char* argv[]) {
    //forces openmp to create the threads beforehand
#pragma omp parallel
    {
        int fd = open (argv[0], O_RDONLY);
        if (fd != -1) {
            close (fd);
        }
        else {
            std::cerr<<"something is amiss"<<std::endl;
        }
    }
    
    if (argc < 3) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
        return -1;
    }
    
    int n = atoi(argv[1]);
    int * arr = new int [n];
    
    generateReduceData (arr, atoi(argv[1]));
    
    // insert reduction code here    
    int nbthreads = atoi(argv[2]);
    int sum, sum2 = 0;
    int gran = n/nbthreads;
    int th[nbthreads];

    for(int i=0;i<nbthreads;i++)
  	th[i] = 0;

    omp_set_num_threads(nbthreads);
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    #pragma omp parallel 
    {
     #pragma omp single
     {
       reduce(arr, n, gran, th);
     }
     #pragma omp critical
     {
	for(int i=0; i<nbthreads;i++){
  	  sum+=th[i];
        }
     }
    }
    
    std::cout << sum << std::endl;
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapased_seconds = end-start;
    std::cerr<<elapased_seconds.count()<<std::endl;
    
    delete[] arr;
    
    return 0;
}
