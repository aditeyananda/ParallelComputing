#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <chrono>
#include <cmath>
using namespace std;
using seconds = chrono::seconds;
using check_time = std::chrono::high_resolution_clock;
 
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
  float a,b,m;
  int intensity,function_id, begin, end, n;
  string sync;
};
 
pthread_mutex_t mut, mut2;
float sum = 0.0, m = 0.0;
int global_beg = 0, global_end = 0, granularity = 0, comp = 0, n;

bool done(){
  pthread_mutex_lock(&mut2);
 
  if(comp == 0){
    pthread_mutex_unlock(&mut2);
    return true;
  }
  else{
    if(comp < granularity){
         comp = 0;
    }
    else{
         comp = comp-granularity;
    } 
    pthread_mutex_unlock(&mut2);
    return false;
   }
}
 
void getnext(int *begin, int *end){
  pthread_mutex_lock(&mut2);
  *begin = global_beg;
  *end = global_end;
 
  if(granularity<=(n-global_end)){
    global_beg = *end;
    global_end = global_beg + granularity;
  }
  pthread_mutex_unlock(&mut2);
}
 
void* iteration(void* arg){
  int begin, end;
  while(!done()){
    param *p = (param*) arg;

    getnext(&begin, &end);
    for(int i=begin; i<end; i++){
    float x = (p->a + (i + 0.5)*p->m);
    if(p->function_id==1){
      pthread_mutex_lock(&mut);
      sum +=(f1(x, p->intensity)*p->m);
      pthread_mutex_unlock(&mut);
    }
    else if(p->function_id==2){
      pthread_mutex_lock(&mut);
      sum +=(f2(x, p->intensity)*p->m);
      pthread_mutex_unlock(&mut);
    }
    else if(p->function_id==3){
      pthread_mutex_lock(&mut);
      sum +=(f3(x, p->intensity)*p->m);
      pthread_mutex_unlock(&mut);
    }
    else if(p->function_id==4){
      pthread_mutex_lock(&mut);
      sum +=(f4(x, p->intensity)*p->m);
      pthread_mutex_unlock(&mut);
    }
  }   
} 
  pthread_exit(NULL);
}

void* chunk(void* arg){
  while(!done()){
    param *p = (param*)arg;
    int begin, end;
    float sum2 = 0.0;
    getnext(&begin, &end);
    for (int i=begin; i<end; i++){
      float x = (p->a + (i + 0.5)*p->m);
      if(p->function_id==1){
        sum2 +=(f1(x, p->intensity));
      }
      else if(p->function_id==2){
        sum2 +=(f2(x, p->intensity));
      }
      else if(p->function_id==3){
        sum2 +=(f3(x, p->intensity));
      }
      else if(p->function_id==4){
        sum2 +=(f4(x, p->intensity));
      }      
   }   
   pthread_mutex_lock(&mut);
   sum += sum2;
   pthread_mutex_unlock(&mut);
  }  
 pthread_exit(NULL);
}
 
void* thread(void* arg){
  float sum2 = 0.0;
  int begin, end;
  param *p = (param*)arg;
  while(!done()){
   getnext(&begin, &end);
   for (int i=begin; i<end; i++){
     float x = (p->a+(i + 0.5)*p->m);
     if(p->function_id==1){
        sum2 +=(f1(x, p->intensity));
      }
      else if(p->function_id==2){
        sum2 +=(f2(x, p->intensity));
      }
      else if(p->function_id==3){
        sum2 +=(f3(x, p->intensity));
      }
      else if(p->function_id==4){
        sum2 +=(f4(x, p->intensity));
      }
   }   
  }
    pthread_mutex_lock(&mut);
    sum=sum+sum2;
    pthread_mutex_unlock(&mut);
    pthread_exit(NULL);
} 
int main (int argc, char* argv[]){
   std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
 
  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  } 
  int id = atoi(argv[1]);
  float a = atof(argv[2]);
  float b = atof(argv[3]);
  n = atoi(argv[4]);
  int intensity = atoi(argv[5]);
  float m=((b-a)/n);
  int nbthreads = atoi(argv[6]);
  string sync = argv[7];
  granularity = atoi(argv[8]);
  global_end = granularity;
  struct param p[nbthreads];
  pthread_t th[nbthreads];
  comp = n;
  pthread_mutex_init(&mut, NULL);
  pthread_mutex_init(&mut2, NULL);
  
  if(0 == sync.compare("iteration")){
    for(int i= 0; i<nbthreads;i++){
      p[i].a = a;
      p[i].b = b;
      p[i].function_id = id;
      p[i].intensity = intensity; 
      p[i].m = m;
      p[i].sync = sync;
      p[i].n = n;
      pthread_create(&th[i], NULL, iteration, (void*) &p[i]);
    }   
  }
  else if(0 == sync.compare("thread")){
    for(int i= 0; i<nbthreads;i++){
      p[i].a = a;
      p[i].b = b;
      p[i].function_id = id;
      p[i].intensity = intensity;
      p[i].m = m;
      p[i].sync = sync;
      p[i].n = n;
      pthread_create(&th[i], NULL, thread,(void*) &p[i]);
    }
  }
  else if(0 == sync.compare("chunk")){
    for(int i= 0; i<nbthreads;i++){
      p[i].a = a;
      p[i].b = b;
      p[i].function_id = id;
      p[i].intensity = intensity;
      p[i].m = m;
      p[i].sync = sync;
      p[i].n = n;
      pthread_create(&th[i], NULL, chunk, (void*) &p[i]);
    }
  }
 
  for(int i=0; i <nbthreads; i++){
     pthread_join(th[i], NULL);
  }
 
  pthread_mutex_destroy(&mut);
  pthread_mutex_destroy(&mut2);
  cout<<sum*m;;
  std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end_time-start_time;
  std::cerr<<elapsed_seconds.count()<<std::endl;
 
  return 0;
}
