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
    
    if (argc < 5) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
        return -1;
    }
    
    int n = atoi(argv[1]);
    int * arr = new int [n];
    
    generateReduceData (arr, atoi(argv[1]));
    
    // insert reduction code here
    int sum = 0;
    char* scheduling;
    
    int numOfThreads = atoi(argv[2]);
    scheduling = argv[3];
    int granularity = atoi(argv[4]);
    
    omp_set_num_threads(numOfThreads);

    if(strcmp(scheduling,"static")==0){
    	omp_set_schedule(omp_sched_static, granularity);
    }
    else if(strcmp(scheduling,"dynamic")==0){
    	omp_set_schedule(omp_sched_dynamic,granularity);
    }
    else{
    	omp_set_schedule(omp_sched_guided, granularity);
    }
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    #pragma omp parallel for schedule(runtime) reduction(+:sum)
    	for(int i=0; i<n; i++){
    		sum = sum + arr[i];
    	}
    std::cout << sum << std::endl;
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapased_seconds = end-start;
    std::cerr<<elapased_seconds.count()<<std::endl;
    
    delete[] arr;
    
    return 0;
}
