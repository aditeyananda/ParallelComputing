#include <unistd.h>
#include <stdio.h>
int main() 
{	char hostname[100];
	int size = 100;
	gethostname(hostname, size);
  
  printf("Hostname: %s\n", hostname);
  return 0;
}
