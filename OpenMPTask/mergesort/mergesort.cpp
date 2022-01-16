#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

  
#ifdef __cplusplus
}
#endif

int * temp;
void Merge(int arr[], int l, int h, int m, int n)
{
  int p = l;
  int q = m+1;
  int r = l;
  
  while(p<=m && q<=h)
  {
    if(arr[p]<arr[q])
    {
      temp[r] = arr[p];
      r++;
      p++;
    }
    else
    {
      temp[r] = arr[q];
      r++;
      q++;
    }
  }
  
  while(p<=m)
  {
    temp[r] = arr[p];
    r++;
    p++;
  }
  
  while(q<=h)
  {
    temp[r] = arr[q];
    r++;
    q++;
  }
  
  for(p=l; p<=h; p++)
  {
    arr[p] = temp[p];
  }
}

void MergeSort(int arr[], int l, int h, int n)
{
  int mid;
  if(l<h && (h-l)>10000)
  {
    mid=(l+h)/2;
    #pragma omp task
    MergeSort(arr, l, mid, n);
    #pragma omp task
    MergeSort(arr, mid+1, h, n);
    #pragma omp taskwait
    Merge(arr, l, h, mid,n);
  
  }
  else if(l<h){
    mid=(l+h)/2;
    MergeSort(arr, l, mid, n);
    MergeSort(arr, mid+1, h, n);
    Merge(arr, l, h, mid,n);
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
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);
  
  //insert sorting code here
  
  int nbthreads = atoi(argv[2]);
  omp_set_num_threads(nbthreads);

  temp = new int [n];
  for(int i = 0; i < n; i++){
    temp[i] = arr[i];
  }
 
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  #pragma omp parallel
  {
    #pragma omp single
    {
      MergeSort(arr,0,n-1,n);
    }
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;    
  std::cerr<<elapsed_seconds.count()<<std::endl;
  
  for(int i=0; i<n-1; i++){
    if(arr[i]>arr[i+1])
      std::cout<<arr[i]<<std::endl;
  }
  
  checkMergeSortResult (arr, atoi(argv[1]));
  
  delete[] arr;

  return 0;
}
