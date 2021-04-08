#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

int main(int argc, char *argv[]){

int num_floats = 0;
int num_bins = atoi(argv[1]);
int num_threads = atoi(argv[2]);


FILE * fp; 
char filename[100]=""; 
strcpy(filename, argv[3]);
//strcat(filename, ".txt");
if(!(fp = fopen(filename,"r"))){
	printf("Cannot create file %s\n", filename);
	exit(1);
}
fscanf(fp, "%d", &num_floats);


float* x = malloc(sizeof(float) * num_floats);
int local_hist[num_threads][num_bins];
int global_hist[num_bins];
	
for (int i = 0; i < num_bins; i++){
	for (int j = 0; j < num_threads; j++){
		local_hist[j][i] = 0;
	}
	global_hist[i] = 0;
}
	
float bin_sz = (20.0 / num_bins);
for (int i = 0; i < num_floats; i++){
	fscanf(fp, "%f", &x[i]);
}
fclose(fp);
	

for (int i = 0; i < num_bins; i++){
	printf("bin[%d] = %d\n", i, local_hist[0][i]);
}
printf("\n");

#pragma omp parallel num_threads(num_threads)
{
	int tid = omp_get_thread_num();
	#pragma omp for nowait
	for (int i = 0; i < num_floats; i++){
		for (int j = 1; j <= num_bins; j++){
			if (x[i] < (bin_sz * j)){
				local_hist[tid][(j-1)]++;
				break;
			}
		}
	}

	for (int i = 0; i < num_bins; i++){
		#pragma omp atomic
		global_hist[i] += local_hist[tid][i];	
	}
	
}
for (int i = 0; i < num_bins; i++){
	printf("bin[%d] = %d\n", i, global_hist[i]);
}
	
}
