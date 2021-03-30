// must compile with -std=c99 -Wall -o checkdiv 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>


int main(int argc, char *argv[]){

int my_rank, comm_sz;  
unsigned int x, n;
//unsigned int i; //loop index
FILE * fp; //for creating the output file
char filename[100]=""; // the file name

clock_t start_p1, start_p3, end_p1, end_p3;
double time_pt1, time_pt2, time_pt3, start_p2, end_p2, loc_time_pt2;


/////////////////////////////////////////
// start of part 1

start_p1 = clock();
// Check that the input from the user is correct.
if(argc != 3){

  printf("usage:  ./checkdiv N x\n");
  printf("N: the upper bound of the range [2,N]\n");
  printf("x: divisor\n");
  exit(1);
}  

n = (unsigned int)atoi(argv[1]); 
x = (unsigned int)atoi(argv[2]);

MPI_Init(NULL, NULL);
MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

MPI_Bcast(&n, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
MPI_Bcast(&x, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

// Process 0 must send the x and n to each process.
// Other processes must, after receiving the variables, calculate their own range.


end_p1 = clock();
time_pt1 = (double)((end_p1 - start_p1) / CLOCKS_PER_SEC);
//end of part 1
/////////////////////////////////////////


/////////////////////////////////////////
//start of part 2
// The main computation part starts here
start_p2 = MPI_Wtime();

int block_sz, loc_start_i, loc_end_i;
block_sz = (n / comm_sz);
int *loc_div_arr = (int*)malloc(sizeof(int) * ((block_sz / x) + 1));

if (my_rank == 0){ loc_start_i = x; }
else{ loc_start_i = (my_rank * block_sz); }

if (my_rank == (comm_sz - 1)){ loc_end_i = n; }
else{ loc_end_i = (loc_start_i + block_sz); } 

while ((loc_start_i % x) != 0){
	loc_start_i++;
}

int ctr = 0;
for (int i = loc_start_i; i < loc_end_i; i += x){
	loc_div_arr[ctr] = i;
	ctr++;
}

int *glob_div_arr;
if (my_rank == 0){
	glob_div_arr = (int*)malloc(sizeof(int) * ((n / x) + 1));
}

MPI_Gather(loc_div_arr, ctr, MPI_INT, glob_div_arr, ((block_sz / x) + 1), MPI_INT, 0, MPI_COMM_WORLD);

end_p2 = MPI_Wtime();
loc_time_pt2 = (end_p2 - start_p2);
MPI_Reduce(&loc_time_pt2, &time_pt2, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
// end of the main compuation part
//end of part 2
/////////////////////////////////////////


/////////////////////////////////////////
//start of part 3
//forming the filename

start_p3 = clock();
if (my_rank == 0){
	strcpy(filename, argv[1]);
	strcat(filename, ".txt");

	if(!(fp = fopen(filename,"w+t")))
	{
	  printf("Cannot create file %s\n", filename);
	  exit(1);
	}

	for(int i = 0; i < (n / x); i++){
	  fprintf(fp,"%d\n", glob_div_arr[i]);
	}

	fclose(fp);
}


end_p3 = clock();
time_pt3 = (double)((end_p3 - start_p3) / CLOCKS_PER_SEC);
//end of part 3
/////////////////////////////////////////

/* Print here the times of the three parts as indicated in the lab description */
if (my_rank == 0){
	printf("time of part1 = %f s\ntime of part2 = %f s\ntime of part3 = %f s\n", time_pt1, time_pt2, time_pt3);
}

MPI_Barrier(MPI_COMM_WORLD);
free(loc_div_arr);
if (my_rank == 0){
	free(glob_div_arr);
}
MPI_Finalize();
return 0;
}
