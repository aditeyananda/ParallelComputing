#include <mpi.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <tuple>

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

#define QUIT 1

std::tuple<int, int> getData(int rid, long n, int nbp){
	nbp = nbp-1;
	int granularity = n/nbp;
	int ptr_beg = rid*granularity;
	int ptr_end = ptr_beg+granularity;
	
	if((n%nbp!=0)&&(ptr_end>n)){
		ptr_end = n; 
	}
	return std::make_tuple(ptr_beg, ptr_end); 
}

float f_sel(int id, float x, int intensity)
{	if(id==1){
        	return f1(x, intensity);
        }
        else if(id==2){
        	return f2(x, intensity);
        }
        else if(id==3){
        	return f3(x, intensity);
        }
        else if(id==4){
        	return f4(x, intensity);
        }
        else{
        	return -1;

        }
}

float method(int beg, int end, int function_id, int intensity, float a, float b, long n)
{
     float local_sum = 0.0;
     float width = (b-a)/float(n);
     for(int i=beg; i<end; i++){
		float x = (a+(i+0.5)*width);
        	float func = f_sel(function_id, x, intensity);
        	local_sum = local_sum+(width * func);
	}
    return local_sum;
}

float master_method(long n, int nbproc)
{
	int init_req = 0;
	int work_sent =0;
	float fin_sum = 0.0;
	int req_id = -1;
	int begin = 0;
	int end = 0;
	float res=0.0;
	MPI_Status status;
	
	for(int i=1; i<nbproc; i++)
	{
	  MPI_Status *status;
	  MPI_Request *req;
	  req= new MPI_Request[3];
  	  status=new MPI_Status[3];
	  for(int j=0; j<3; j++)
	  {
	    if(end<n)
	    {
	      req_id++;
	      work_sent++;
	      std::tie(begin, end) = getData(req_id, n, nbproc);
	      int work[2]={0};
	      work[0] = begin;
	      work[1] = end;
      	      MPI_Isend(work, 2, MPI_INT, i, 0, MPI_COMM_WORLD, &req[j]);
	    }	
	    else
	    {
		MPI_Isend(0, 0, MPI_INT, i, QUIT, MPI_COMM_WORLD, &req[j]); 
	    }
	}	
	MPI_Waitall(3,req, status);
	}
		
	while(work_sent!=0)
	{
	  MPI_Status status;
 	  MPI_Recv(&res, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
	  int id = status.MPI_SOURCE;
	  fin_sum = fin_sum+res;
	  work_sent--;

	  if(end<n)
	  {
		req_id++;
		work_sent++;
		std::tie(begin, end) = getData(req_id, n, nbproc);
		int work[2] = {0};
		work[0] = begin;
		work[1] = end;
		MPI_Send(work, 2, MPI_INT, id, 0, MPI_COMM_WORLD);
	}
	  else
	  {	
	    MPI_Send(0, 0, MPI_INT, id, QUIT, MPI_COMM_WORLD); 
	  }
  }
	return fin_sum;
}

void worker_method(int function_id, int intensity, float a, float b, long n)
{
	float sum = 0.0;
	int work[2] = {0};
	MPI_Status status;
	while(1)
	{
	   MPI_Recv(work, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	   int flag = status.MPI_TAG;
	   if(flag!=QUIT)
	   { 
	     int begin = work[0];
	     int end = work[1];
	     sum = method(begin, end, function_id, intensity, a, b, n);
	     MPI_Send(&sum, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
	   }
	else
	{
	  return; 
	}
  }
}

int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }
  
    int function_id = atoi(argv[1]);
    float a = atoi(argv[2]);
    float b = atoi(argv[3]);
    long int n = atoi(argv[4]);
    int intensity = atoi(argv[5]);
    float sum = 0.0;

    MPI_Init(NULL, NULL);
    int nbprocess, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nbprocess);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    
    if(rank==0){
    	sum = master_method(n, nbprocess);
    }
    else{
    	worker_method(function_id, intensity, a, b, n);
    }
    
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapased_seconds = end-start;
    if(rank==0){
      //Print the sum value to the output stream
      std::cout << sum << std::endl;

      //Print the total execution time (in sec) to the error stream
      std::cerr<<elapased_seconds.count()<<std::endl;
    }
    
    MPI_Finalize();
    return 0;
}
