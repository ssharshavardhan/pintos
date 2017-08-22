#include "./functions.h"
#include "../testing/functions.h"

int main(int c,char * argv[], char** env)
{
	test_stack(c,argv,env);

	printf("\n Enter n : ");
	int n,*input;
	scanf("%d",&n);
	input=(int*)malloc(sizeof(int)*n);
	for(int i=0;i<n;i++)scanf("%d",&input[i]);
	quicksort(input,0,n-1);
	printarray(input,n);

}