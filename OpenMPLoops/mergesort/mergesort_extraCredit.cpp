#include <omp.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData(int* arr, size_t n);
  void checkMergeSortResult(int* arr, size_t n);
#ifdef __cplusplus
}
#endif

void merge(int arr[], int left, int mid, int right, int temp[], int n){
	int p = left, q = left, r=mid+1;
	#pragma omp parallel
	{
	while(q<=mid && r<=right){
		if(arr[q] < arr[r]){
			temp[p++] = arr[q++];
		}
		else{
			temp[p++] = arr[r++];
		}
	}
	while(q<=mid){
		temp[p++] = arr[q++];
	}
	#pragma omp parallel for schedule(static)
	for(int i = left; i<=right; i++){
		arr[i] = temp[i];
	}
}
}

void mergeSort(int arr[], int n){
	int *temp;
	temp = (int*)malloc(n*sizeof(int));
	for(int i=0; i<n; i++){
		temp[i]=arr[i];
	}
	for(int i=1; i<n-1; i=2*i){
		#pragma omp parallel for schedule(static)
		for (int j=0; j<n-1; j=j+(2*i)){
			int mid = std::min(j+i-1, n-1);
			int x = std::min(j+2*i-1, n-1);
			merge(arr, j, mid, x, temp, n);
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
  
  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  
  // get arr data
  int * arr = new int [n];
  

  //insert sorting code here.
  int nbthreads = atoi(argv[2]);
  omp_set_num_threads(nbthreads);
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  generateMergeSortData (arr, n);
  mergeSort(arr, n);
  
  checkMergeSortResult(arr, n);
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapased_seconds = end-start;
  std::cerr<<elapased_seconds.count()<<std::endl;
  
  delete[] arr;

  return 0;
}
