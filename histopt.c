#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <omp.h>

int main(int argc, char *argv[]){
	
//initialize shared variables; read command line arguments
int num_floats = 0;
clock_t start_p, end_p;
double time_p;
int num_bins = atoi(argv[1]);
int num_threads = atoi(argv[2]);
float bin_sz = (20.0 / num_bins);
	
//open file and read the number of floats in the file
FILE * fp; 
char filename[100]=""; 
strcpy(filename, argv[3]);
//strcat(filename, ".txt"); //uncomment if filename excludes extension
if(!(fp = fopen(filename,"r"))){
	printf("Cannot create file %s\n", filename);
	exit(1);
}
fscanf(fp, "%d", &num_floats);

//allocate memory for global float array and histogram
float* x = malloc(sizeof(float) * num_floats);
int global_hist[num_bins];

//initialize values of histograms to 0
for (int i = 0; i < num_bins; i++){
	global_hist[i] = 0;
}

//read floats from file into array
for (int i = 0; i < num_floats; i++){
	fscanf(fp, "%f", &x[i]);
}
fclose(fp);

start_p = clock();

//begin parallel section
#pragma omp parallel num_threads(num_threads)
{
  //create local_histogram
	int local_hist[num_bins];
  for (int i = 0; i < num_bins; i++){
	  local_hist[i] = 0;
  }

	//put floats from array into local bins
	#pragma omp for nowait
	for (int i = 0; i < num_floats; i++){
		for (int j = 1; j <= num_bins; j++){
			//check for correct bin to increment
			if (x[i] < (bin_sz * j)){
				local_hist[(j-1)]++;
				break;
			}
		}
	}
	
	//add values for each local histogram to global histogram
	for (int i = 0; i < num_bins; i++){
		#pragma omp atomic
		global_hist[i] += local_hist[i];		
	}
//end parallel section
}
	
end_p = clock();
time_p = (double)((end_p - start_p) / (double)CLOCKS_PER_SEC);
	
//print values of bins
for (int i = 0; i < num_bins; i++){
	printf("bin[%d] = %d\n", i, global_hist[i]);
}
	
printf("Time parallel section: %f\n", time_p);
	
}
