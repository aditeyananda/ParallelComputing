#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <time.h>
#include <cmath>
#include <pthread.h>
 
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
  
struct param{
  float a,b,n;
  int intensity, begin, end;
  float (*f)(float, int);
  char sync;
};
 
pthread_mutex_t mut;
float sum=0.0;
 
void static_loop(float (*f)(float, int), float a, float b,float n, int intensity, int nbthreads, char sync);
void *method(void* arg);
 
int main (int argc, char* argv[]) {
 
  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }
 
  float a, b, n;
  int id, intensity, nbthreads;
  char sync;  
  sscanf(argv[1],"%d",&id);
  sscanf(argv[2],"%f",&a);
  sscanf(argv[3],"%f",&b);
  sscanf(argv[4],"%f",&n);
  sscanf(argv[5],"%d",&intensity);
  sscanf(argv[6],"%d",&nbthreads);
  sscanf(argv[7],"%c",&sync);
  
  switch(id){
    case 1:static_loop(f1,a,b,n,intensity,nbthreads,sync);
        	break;
    case 2:static_loop(f2,a,b,n,intensity,nbthreads,sync);
        	break;
    case 3:static_loop(f3,a,b,n,intensity,nbthreads,sync);
        	break;
    case 4:static_loop(f4,a,b,n,intensity,nbthreads,sync);
        	break;
    default:return 0;
   }
  return 0;
}
 
void* method(void* arg){
        	float a, b, n;
        	int intensity, begin, end;
        	char sync;
        	float (*f)(float, int);
        	struct param* p = (param*) arg;
        	a = p->a;
        	b = p->b;
        	n = p->n;
        	intensity = p->intensity;
        	begin = p->begin;
        	end = p->end;
        	f = p->f;
        	sync = p->sync;
        	char x = 'iteration';
        	if(sync==x){
                    	for(int i = begin; i<end; i++){
                                	pthread_mutex_lock(&mut);
                                	sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
                                	pthread_mutex_unlock(&mut);
                    	}
        	}
        	else{
                    	float sum2=0.0;
                    	for(int i=begin; i<end; i++){
                                	sum2+=f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
                    	}
                    	pthread_mutex_lock(&mut);
                                	sum+=sum2;
                    	pthread_mutex_unlock(&mut);
        	}
        	
        	return NULL;
        	
}
 
void static_loop(float (*f)(float, int), float a, float b, float n, int intensity, int nbthreads, char sync){
  pthread_t th[nbthreads];
  float* range = new float[nbthreads];
  int i;
  for(i=0; i<nbthreads-1; i++){
        	range[i] = floor((i+1)*(n/nbthreads));
  }
  range[i]=n;
  param* p = new param[nbthreads];
  
  std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
  
  for(i = 0; i<nbthreads; i++){
        	param p1;
        	p1.a = a;
        	p1.b = b;
        	p1.n = n;
        	p1.begin = range[i-1];
        	p1.intensity = intensity;
        	p1.f = f;
        	p1.end = range[i];
        	p1.sync = sync;
        	p[i] = p1;
        	pthread_create(&th[i], NULL, method, &p[i]);
  }
 
 for(i = 0; i<nbthreads; i++){
        	pthread_join(th[i], NULL);
 }
 
  std::chrono::time_point<std::chrono::system_clock> finish = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = finish-begin;
  std::cerr<<elapsed_seconds.count()<<std::endl;
  printf("%lf\n", sum);
}
 



