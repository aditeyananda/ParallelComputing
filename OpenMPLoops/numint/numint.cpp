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

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif
float a,b,n,sum = 0.0,nbthreads,granularity;
int id, intensity;
std::string scheduling;
  
float method(float (*f)(float, int), float a, float b, float n, int intensity, int granularity){
	float sum = 0.0;
	#pragma omp parallel for schedule(dynamic, granularity) reduction(+:sum)
	for (int i = 0; i<(int)n; i++){
		sum = sum + f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	}
	return sum;
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
  
  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  //insert code here
  id = atoi(argv[1]);
  a = atoi(argv[2]);
  b = atoi(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  scheduling = argv[7];
  granularity = atoi(argv[8]);
  
  omp_set_num_threads(nbthreads);
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  switch(id){
  	case 1: sum = method(f1, a, b, n, intensity, granularity);
  	break;
  	case 2: sum = method(f2, a, b, n, intensity, granularity);
  	break;
  	case 3: sum = method(f3, a, b, n, intensity, granularity);
  	break;
  	case 4: sum = method(f4, a, b, n, intensity, granularity);
  	break;
  	default: return 0;
  	break;
  }
  
  std::cout << sum << std::endl;
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapased_seconds = end-start;
  
  std::cerr<<elapased_seconds.count()<<std::endl;
  return 0;
}
