//*********************************************************
//
// Rahul Bethi
// COSC 5331, Foundations of Computer System Software (Operating Systems) - Fall 2015
// Programming Project #4: Implementation of Banker's Algorithm
// December 2, 2015
// Instructor: Dr. Ajay Katangur
//
//*********************************************************
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define ARRAY_SIZE 20

int main( int argc, char *argv[] )
{
	int  retVal, i, j, k, l, pNum, rNum, reqProcess, runNum, safe, completeNum, stacklen, rejects, lastReject = -1, test = 0;
	int	 need[ARRAY_SIZE][ARRAY_SIZE];
	int	 needD[ARRAY_SIZE][ARRAY_SIZE][ARRAY_SIZE];	// 3D Arrays to store all values of need allocation and available matrices in all stages
	int  alloc[ARRAY_SIZE][ARRAY_SIZE];
	int  allocD[ARRAY_SIZE][ARRAY_SIZE][ARRAY_SIZE];
	int	 max[ARRAY_SIZE][ARRAY_SIZE];
	int  avail[ARRAY_SIZE];
	int  availD[ARRAY_SIZE][ARRAY_SIZE];
	int  req[ARRAY_SIZE];
	int  stack[ARRAY_SIZE];							// stack array to store all the process which can be run
	FILE *inFile;
	char line[ARRAY_SIZE][ARRAY_SIZE];
	char word[ARRAY_SIZE][ARRAY_SIZE];
	
	if(argv[1] == NULL)
	{
		printf( "\nIncomplete input!!\n\n" );
		return( retVal );
	}
	
	inFile = fopen(argv[1], "rt");		// read text from file
	if(!inFile)
	{
		printf( "\nInput file cannot be loaded!!\n\n" );
		return( retVal );
	}
	i = 0;
	while(fgets(line[i], sizeof(line[i]), inFile)!=NULL)		// store each line into line char array
		i++;
	fclose(inFile);												// close file
	
	pNum = atoi(line[0]);										// number of process
	rNum = atoi(line[2]);										// number of resources
	printf("\nThere are %d processes in the system.\n\nThere are %d resource types.\n\nThe Allocation Matrix is...\n   A B C D", pNum, rNum);
	
	for(i=4; i<(9+(2*pNum)); i++)
	{
		k = 0;
		l = 0;
		for(j=0; line[i][j]!='\0'; j++)							// to get end of line
		{
			if(line[i][j] != ' ' && line[i][j] != ':')			// seperate each word with spaces and ':'
			{
				word[k][l] = line[i][j];
				l++;
			}
			else
			{
				k++;
				l = 0;
			}
		}
		if(i>=4 && i<(4+pNum))
			for(j=0; j<=k; j++)
			{
				alloc[i-4][j]  = atoi(word[j]);					// allocation matrix
				allocD[0][i-4][j] = alloc[i-4][j];				// Duplicating matrices to save original matrices
			}
		else if(i>=(5+pNum) && i<(5+(2*pNum)))
			for(j=0; j<=k; j++)
				max[i-(5+pNum)][j] = atoi(word[j]);				// max matrix
		else if(i==(6+(2*pNum)))
			for(j=0; j<=k; j++)
			{
				avail[j]  = atoi(word[j]);						// available vector
				availD[0][j] = avail[j];
			}
		else if(i==(8+(2*pNum)))
		{
			reqProcess = atoi(word[0]);							// requested process
			for(j=1; j<=k; j++)
				req[j-1] = atoi(word[j]);						// request vector
		}
		
		for(j=0; j<ARRAY_SIZE; j++)
			for(k=0; k<ARRAY_SIZE; k++)
				word[j][k] = NULL;								// Reset words to NULL after reading every line
	}
	
	// Displaing Allocation, Max matrices
	for(i=0; i<pNum; i++)
	{
		printf("\n%d:", i);
		for(j=0; j<rNum; j++)
			printf(" %d", alloc[i][j]);
	}
	
	printf("\n\nThe Max Matrix is...\n   A B C D");
	for(i=0; i<pNum; i++)
	{
		printf("\n%d:", i);
		for(j=0; j<rNum; j++)
			printf(" %d", max[i][j]);
	}
	
	// Calculating and displaying Need matrix
	printf("\n\nThe Need Matrix is...\n   A B C D");
	for(i=0; i<pNum; i++)
	{
		printf("\n%d:", i);
		for(j=0; j<rNum; j++)
		{
			need[i][j]  = max[i][j] - alloc[i][j];
			needD[0][i][j] = need[i][j];
			printf(" %d", need[i][j]);
		}
	}
	
	printf("\n\nThe Available Vector is...\nA B C D\n");
	for(i=0; i<rNum; i++)
		printf("%d ", avail[i]);							//Display Available Vector
	
	// Calculation starts from here
	C:runNum = 0;
	for(i=0; i<pNum; i++)
	{
		for(j=0; j<rNum; j++)
			if(availD[0][j] - needD[0][i][j] < 0)
			{
				runNum++;
				j = rNum;
			}
	}
	
	if(runNum == pNum)
	{
		if(test == 0)						// test phase for the request vector
			printf("\n\nTHE SYSTEM IS IN UNSAFE STATE!\n\n");
		else
			printf("\n\nThis request cannot be granted!\n\n");
		return( retVal );
	}
	else
	{
		stacklen = 0;						// initialize stack length to zero
		for(k=0; k<pNum; k++)				// to run atleast more process x process times
		{
			rejects = 0;					// reset number of rejects everytime
			for(i=0; i<pNum; i++)
			{
				if(i == lastReject)			// reject if the process did not yield result last time
				{
					rejects++;
					//printf("\n>>%d This did not yield result last time", i);
					goto B;
				}
				j = 0;
				while(j<stacklen && stacklen > 0)						// check if the process is already in stack
				{
					if(i == stack[j])
					{
						rejects++;
						//printf("\n>>%d is already in stack", i);
						goto B;											// goto reject line
					}
					j++;
				}
				for(j=0; j<rNum; j++)									// reject if need is more than available
				{
					if(availD[stacklen][j] - needD[stacklen][i][j] < 0)
					{
						rejects++;
						//printf("\n>>%d doesn't have enough available resources", i);
						goto B;
					}
				}
				stack[stacklen] = i;									// if its not rejected in the above cases, it can be run
				stacklen++;												// store it in the stack and append stack length
				/*printf("\nStack Accepted: ");
				for(l=0; l<stacklen; l++)
					printf("%d ", stack[l]);*/
				if(stacklen == pNum)									// if stacklength is equal to number of process, then all of them can be run succesfully
				{
					printf("\n\nThese processes can be executed in the order: ");
					for(l=0; l<stacklen; l++)
						printf("%d ", stack[l]);						// display in which order the process can be run
					goto A;												// goto success line
				}
				for(j=0; j<rNum; j++)
				{
					availD[stacklen][j] = availD[stacklen-1][j] + allocD[stacklen-1][i][j];
					for(l=0; l<pNum; l++)
					{
						allocD[stacklen][l][j] = allocD[stacklen-1][l][j];
						needD[stacklen][l][j] = max[l][j] - allocD[stacklen][l][j];
					}
				}
				/*printf("\nNew Available Vector is... ");
				for(l=0; l<rNum; l++)
					printf("%d ", availD[stacklen][l]);*/
				i = pNum;
				
				B:if(rejects >= pNum)									// reject line - check if there are more rejects as number of process, then all are rejected
				{
					/*printf("\nStack Reject: ");
					for(l=0; l<stacklen; l++)
						printf("%d ", stack[l]);*/
					stacklen--;											// decrease the stack so that the last succesful did not yield any result
					lastReject = stack[stacklen];						// store the last rejected process, so that it cannot be accepted again
					//printf("\n>>last rejected process = %d", stack[stacklen]);
					i = pNum;
				}
			}
		}
		if(test == 0)
			printf("\n\nTHE SYSTEM IS IN UNSAFE STATE!\n\n");
		else
			printf("\n\nThis request cannot be granted!\n\n");
		return( retVal );
	}
	
	A:if(test == 0)														// Success line - goto next step of check request vector
	{
		printf("\n\nTHE SYSTEM IS IN A SAFE STATE!\n\nThe Request Vector is...\n  A B C D\n%d:", reqProcess);
		for(i=0; i<rNum; i++)
			printf("%d ", req[i]);
		for(j=0; j<rNum; j++)
		{
			if(avail[j] - req[j] < 0)									// check if the requested vector is more than available vector
			{
				printf("\n\nRequest cannot be granted as it exceeds Available Vector!\n\n");
				return( retVal );
			}
			else
			{															// change the matrices accordingly to calculate
				availD[0][j] = avail[j] - req[j];
				allocD[0][reqProcess][j] = alloc[reqProcess][j] + req[j];
			}
		}
		/*printf("\n\nNew Available Vector is... ");
		for(l=0; l<rNum; l++)
				printf("%d ", availD[0][l]);
		printf("\n\nNew Alloc Vector for %d is... ", reqProcess);
		for(l=0; l<rNum; l++)
				printf("%d ", allocD[0][reqProcess][l]);*/
		test = 1;														// enable test phase
		goto C;															// goto calculation start line
	}
	else
	{
		printf("\n\nThis request can be granted!\n\n");
	}
	
	return( retVal);
}