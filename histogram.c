#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

int main(int argc, char *argv[]){

int num_floats;
int num_bins = atoi(argv[1]);
int num_threads = atoi(argv[2]);
omp_set_num_threads(num_threads);

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
float bin_sz = (20.0 / num_threads);

for (int i = 0; i < num_floats; i++){
	fscanf(fp, "%lf", x[i]);
}

fclose(fp);

#pragma omp parallel default(none) shared(x, bin_sz, num_floats, num_threads, global_hist) private(local_hist)
{
	int tid = omp_get_thread_num();

	#pragma omp for
	for (int i = 0; i < num_floats; i++){
		for (int j = 1; j <= num_threads; j++){
			if (x[i] < (bin_sz * j)){
				local_hist[tid][(j-1)]++;
			}
		}
	}

	#pragma omp single
	for (int i = 0; i < num_threads; i++){
		for (int j = 0; j < num_threads; j++){
			global_hist[j] += local_hist[tid][j];
		}	
	}
}

for (int i = 0; i < num_threads; i++){
	printf("bin[%d] = %d\n", &i, global_hist[i]);
}

}
