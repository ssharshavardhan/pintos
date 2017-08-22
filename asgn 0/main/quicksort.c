#include "./functions.h"

void printarray(int *input,int n){for(int i=0;i<n;i++)printf("%d ",input[i]);printf("\n");}

void swap(int*input,int i, int j)
{
	int temp=input[i];
	input[i]=input[j];
	input[j]=temp;
}

int partition(int*input,int low,int high)
{
	int pivot=input[high],index=low;
	for(int i=low;i<=high;i++)if(input[i]<pivot)swap(input,i,index++);
	swap(input,index,high);
	return index;

}

void quicksort(int*input,int low,int high)
{
	int p;
	if(low<high)
	{
		p=partition(input,low,high);
		quicksort(input,low,p-1);
		quicksort(input,p+1,high);
	}
}
