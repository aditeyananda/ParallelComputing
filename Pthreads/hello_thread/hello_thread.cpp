#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* f(void* nbthreads){
	int p = *(int*) nbthreads;
	printf("I am thread %d in nbthreads\n", p);
	return NULL;
}

int main (int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
    return -1;
}
  int threads;
  sscanf(argv[1], "%d", &threads);
  
  int arr[threads];
  
  pthread_t th[threads];
  
  for(int i = 0; i <= threads; i++){
    arr[i]=i;
}  

  for (int i = 1; i <= threads; ++i){
    pthread_create(&th[i], NULL, f, &arr[i]);
}
  for (int i = 1; i <= threads; i++){
    pthread_join(th[i], NULL);
}
  
  return 0;
}
