#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

	printf("enter\n");
	void * p = malloc(1);
	free(p);
	printf("left = %p \n", p);
	return 0;
}
