#include "./functions.h"


void test_stack(int argc,char * argv[],char** env)
{ 
	int i=0;
	// while(env[i]!=0){
	// 	printf("%s\n", env[i++]);
	// }

	printf("ARGC:%d  ARGV:%x\n", argc, (unsigned int)argv);
	for (i = 0; i < argc; i++)
		printf("Argv[%d] = %x pointing at %s \n", i, (unsigned int)argv[i], argv[i]);
}