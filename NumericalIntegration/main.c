#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

double calc_numerical_integration(int functionid, int a, int b, int n, int intensity){
  float x, y, integr=0.0;
  x = (float)(b-a)/n;
  for(int i = 0; i < n; i++)
  {
    	y = a + ((i + 0.5) * x);
    	
    	if(functionid==1){
    		integr = integr + f1(y,intensity);
	}
	else if(functionid==2){
		integr = integr + f2(y,intensity);
	}
	else if(functionid==3){
		integr = integr + f3(y,intensity);
	}
	else if(functionid==4){
	    	integr = integr + f4(y,intensity);
	}
	else{
	     	return -1;
    		}
  }
  return(integr*x);
}

int main (int argc, char* argv[]) {
    
    if (argc < 6) {
        fprintf(stderr, "usage: %s <functionid> <a> <b> <n> <intensity>", argv[0]);
        return -1;
    }
    int function_id, a, b, n, intensity;
    sscanf(argv[1],"%d",&function_id);
    sscanf(argv[2],"%d",&a);
    sscanf(argv[3],"%d",&b);
    sscanf(argv[4],"%d",&n);
    sscanf(argv[5],"%d",&intensity);
  	  
    clock_t t; // t represents clock ticks which is of type 'clock_t'
    t = clock(); // start clock
    double r = calc_numerical_integration(function_id, a, b, n, intensity);
    t = clock()-t; // end clock=
    float time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds; CLOCKS_PER_SEC is the number of clock ticks per second
    printf("%lf\n", r);
    fprintf(stderr, "%f\n", time_taken);
    
    return 0;
}

