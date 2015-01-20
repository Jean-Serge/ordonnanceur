#include <stdio.h>
#include <stdlib.h>
#include "contexte.h"

static int x = 0;
static int y = 0;
static int z = 0;

void f_ping(void *args)
{

  int i;
  for(i = 0 ; i < N ; i++)
    {
      x++;
      z++;
      printf("A"); 
      fflush(stdout); 
    }
}

void f_pong(void *args)
{

  int i;
  for(i = 0 ; i < N/2 ; i++)
    {
      printf("1"); 
      fflush(stdout); 
      y++;
      z++;
    }

}


int main(void)
{
	int i =0;
	  create_ctx(16384, f_ping, NULL);
	  create_ctx(16384, f_pong, NULL);

	  start_sched();

/*	  while(++i != 1000000000); */
	  while(!termine());
	  printf("x : %d\ny : %d\nz : %d\n", x, y, z); 

	  exit(EXIT_SUCCESS);


}
