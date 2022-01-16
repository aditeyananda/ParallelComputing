#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>


#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

int* PrefixSum(int* arr, int* pr, int n, int nbthreads);

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
  generatePrefixSumData (arr, n);

  int * pr = new int [n+1];

  //insert prefix sum code here
  int nbthreads = atoi(argv[2]);
  omp_set_num_threads(nbthreads);
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  PrefixSum(arr, pr, n, nbthreads);
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapased_seconds = end-start;
  std::cerr<<elapased_seconds.count()<<std::endl;
  
  
  checkPrefixSumResult(pr, n);

  delete[] arr;

  return 0;
}

int* PrefixSum(int* arr, int* pr, int n, int nbthreads){
	int* block = new int[nbthreads];
	pr[0]=0;
	#pragma omp parallel
	{
		int threadNum = omp_get_thread_num();
		#pragma omp single
		{
			block = new int[nbthreads+1];
			block[0]=0;
		}
		
		int sum=0;
		#pragma omp for schedule(static)
			for(int i=0; i<n; i++){
				sum = sum +arr[i];
				pr[i+1] = sum;
			}
			
		block[threadNum+1] = sum;
		#pragma omp barrier
		int x = 0;
		for(int i=0; i<(threadNum+1); i++){
			x = x+block[i];
		}
		
		#pragma omp for schedule(static)
		for(int i=0; i<n; i++){
			pr[i+1] = pr[i+1] + x;
		}
	}
	
	delete[] block;
	return pr;
}

