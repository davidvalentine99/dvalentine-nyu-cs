#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Declare a struct to hold the individual processes
typedef struct Process
{
	int pid;
	int cpu_t;
	int io_t;
	int entry_t;
	int exit_t;
	int remaining_t;
	int burst_t;
	int state; //0 for blocked, 1 for ready, 2 for running
} process;

int cmpPs(struct Process *p1, struct Process *p2){
	int comp = p1->pid - p2->pid;
	if (comp == 0){return -1;}
	return comp;
}

int main(int argc, char * argv[]){

	struct Process *running_process = NULL;
	int cycle = 0;
	int blocked_processes = 0;
	int ready_processes = 0;
	int cpu_idle_t = 0;
	int batch = 0;
	int tail = 0;
	int head = 0;

	//Read the input
	char *filename = argv[1];
	char S_num_processes[2];
	FILE *fp = fopen(filename, "r");
	fgets(S_num_processes, 2, fp);
	int scheduling_algorithm = atoi(argv[2]);
	int num_processes = atoi(S_num_processes);
	int active_processes = num_processes;

	//create output file
	char outputfilename[50];
	char delim[2] = ".";
	char ext[3] = "-";
	strcat(ext, argv[2]);
	char *fnames;
	fnames = strtok(argv[1], delim);
	strcat(outputfilename, fnames);
	strcat(outputfilename, ext);
	strcat(outputfilename, ".txt");

	FILE *outputfile = fopen(outputfilename, "w");

	//Create array of processes
	struct Process *pl[num_processes];
	struct Process *ready_q[num_processes];
	struct Process *blocked_q[num_processes];
	struct Process *batch_q[num_processes];

	int a, b, c, d;

	for (int i = 0; i < num_processes; i++){

		char int_holder[2];
		fscanf(fp, "%s", int_holder);
		a = atoi(int_holder);
		fscanf(fp, "%s", int_holder);
		b = atoi(int_holder);
		fscanf(fp, "%s", int_holder);
		c = atoi(int_holder);
		fscanf(fp, "%s", int_holder);
		d = atoi(int_holder);

		pl[i] = (struct Process *) malloc(sizeof(struct Process));

		pl[i]->pid = a;
		pl[i]->cpu_t = b;
		pl[i]->io_t = c;
		pl[i]->entry_t = d;
		pl[i]->exit_t = 0;
		pl[i]->remaining_t = b;
		pl[i]->burst_t = (b / 2);
		pl[i]->state = -1;

		ready_q[i] = NULL;
		blocked_q[i] = NULL;
		batch_q[i] = NULL;
	}

	for (int i = 0; i < num_processes; i++){
		//printf("%d, %d, %d, %d\n", pl[i]->pid, pl[i]->cpu_t, pl[i]->io_t, pl[i]->entry_t);
		ready_q[i] = NULL;
		blocked_q[i] = NULL;
		batch_q[i] = NULL;
	}


	if (scheduling_algorithm == 0){
		//FCFS
		while (active_processes > 0){

			for (int i = 0; i < num_processes; i++){
				if (cycle == pl[i]->entry_t){
					batch_q[batch] = pl[i];
					ready_processes++;
					batch++;
				}
			}

			for (int i = 0; i < num_processes; i++){
				if (blocked_q[i] != NULL) {
					blocked_q[i]->io_t--;
					if (blocked_q[i]->io_t == -1){
						batch_q[batch] = blocked_q[i];
						ready_processes++;
						batch++;
						blocked_q[i] = NULL;
						blocked_processes--;
					}
				}
			}

			qsort(batch_q, batch, sizeof(struct Process*), cmpPs);
			for (int i = 0; i < batch; i++){
				if (tail == num_processes){tail = 0;}
				ready_q[tail] = batch_q[i];
				tail++;
			}
			batch = 0;

			for(int i = 0; i < num_processes; i++){
				if (batch_q[i] == NULL) {break;}
				else {batch_q[i] = NULL;}
			}

			if (running_process == NULL && ready_q[head] != NULL){
				running_process = ready_q[head];
				ready_q[head] = NULL;
				head++;
				if (head == num_processes) {head = 0;}
			}

			fprintf(outputfile, "%d ", cycle);
			if (running_process != NULL){
				fprintf(outputfile, "%d:running ", running_process->pid);
			}
			for (int i = 0; i < num_processes; i++){
				if (ready_q[i] != NULL){
					fprintf(outputfile, "%d:ready ", ready_q[i]->pid);
				}
				if (blocked_q[i] != NULL){
					fprintf(outputfile, "%d:blocked ", blocked_q[i]->pid);
				}
			}
			fprintf(outputfile, "\n");


			if (running_process == NULL) {cpu_idle_t++;}
			else{

				running_process->remaining_t--;

				if (running_process->remaining_t == running_process->burst_t){
					for (int i = 0; i < num_processes; i++){
						if (blocked_q[i] == NULL){
							blocked_q[i] = running_process;
							break;
						}
					}
					blocked_processes++;
					running_process = NULL;
				}
				else if (running_process->remaining_t == 0){
					//it's done, remove
					running_process->exit_t = cycle;
					running_process = NULL;
					active_processes--;
				}
			}
			cycle++;
		}
	}
	else if (scheduling_algorithm == 1){
	    int q = 0;
		//round robin
		while (active_processes > 0){

			for (int i = 0; i < num_processes; i++){
				if (cycle == pl[i]->entry_t){
					batch_q[batch] = pl[i];
					ready_processes++;
					batch++;
				}
			}

			for (int i = 0; i < num_processes; i++){
				if (blocked_q[i] != NULL) {
					blocked_q[i]->io_t--;
					if (blocked_q[i]->io_t == -1){
						batch_q[batch] = blocked_q[i];
						ready_processes++;
						batch++;
						blocked_q[i] = NULL;
						blocked_processes--;
					}
				}
			}
			if (q == 2){
				batch_q[batch] = running_process;
				batch++;
				ready_processes++;
				running_process = NULL;
				q = 0;
			}

			qsort(batch_q, batch, sizeof(struct Process*), cmpPs);
			for (int i = 0; i < batch; i++){
				if (tail == num_processes){tail = 0;}
				ready_q[tail] = batch_q[i];
				tail++;
			}
			batch = 0;

			for(int i = 0; i < num_processes; i++){
				if (batch_q[i] == NULL) {break;}
				else {batch_q[i] = NULL;}
			}

			if (running_process == NULL && ready_q[head] != NULL){
				running_process = ready_q[head];
				ready_q[head] = NULL;
				head++;
				if (head == num_processes) {head = 0;}
			}

			fprintf(outputfile, "%d ", cycle);
			if (running_process != NULL){
				fprintf(outputfile, "%d:running ", running_process->pid);
			}
			for (int i = 0; i < num_processes; i++){
				if (ready_q[i] != NULL){
					fprintf(outputfile, "%d:ready ", ready_q[i]->pid);
				}
				if (blocked_q[i] != NULL){
					fprintf(outputfile, "%d:blocked ", blocked_q[i]->pid);
				}
			}
			fprintf(outputfile, "\n");

			if (running_process == NULL) {cpu_idle_t++;}
			else{

				running_process->remaining_t--;
				q++;

				if (running_process->remaining_t == running_process->burst_t){
					for (int i = 0; i < num_processes; i++){
						if (blocked_q[i] == NULL){
							blocked_q[i] = running_process;
							break;
						}
					}
					blocked_processes++;
					running_process = NULL;
					q = 0;
				}
				else if (running_process->remaining_t == 0){
					//it's done, remove
					running_process->exit_t = cycle;
					running_process = NULL;
					q = 0;
					active_processes--;
				}
			}
			cycle++;
		}
	}
	else if (scheduling_algorithm == 2){
		//shortest time remaining
		while (active_processes > 0){

			for (int i = 0; i < num_processes; i++){
				if (cycle == pl[i]->entry_t){
					for (int j = 0; j < num_processes; j++){
						if (ready_q[j] == NULL) {
							ready_q[j] = pl[i];
							break;
						}
					}
					ready_processes++;
				}
			}

			for (int i = 0; i < num_processes; i++){
				if (blocked_q[i] != NULL) {
					blocked_q[i]->io_t--;
					if (blocked_q[i]->io_t == -1){
						for (int j = 0; j < num_processes; j++){
							if (ready_q[j] == NULL) {
								ready_q[j] = blocked_q[i];
								break;
							}
						}
						ready_processes++;
						blocked_q[i] = NULL;
						blocked_processes--;
					}
				}
			}

			int min_i = -1;
			int min = 100000000;
			struct Process *min_p = NULL;
			for (int i = 0; i < num_processes; i++){
				if (ready_q[i] != NULL){
					if (ready_q[i]->remaining_t < min){
						min = ready_q[i]->remaining_t;
						min_p = ready_q[i];
						min_i = i;
					}
					else if (ready_q[i]->remaining_t == min){
						if (ready_q[i]->pid < min_p->pid){
							min = ready_q[i]->remaining_t;
							min_p = ready_q[i];
							min_i = i;
						}
					}
				}
			}

			running_process = min_p;

			if (min_i != -1) {ready_q[min_i] = NULL;}

			fprintf(outputfile, "%d ", cycle);
			if (running_process != NULL){
				fprintf(outputfile, "%d:running ", running_process->pid);
			}
			for (int i = 0; i < num_processes; i++){
				if (ready_q[i] != NULL){
					fprintf(outputfile, "%d:ready ", ready_q[i]->pid);
				}
				if (blocked_q[i] != NULL){
					fprintf(outputfile, "%d:blocked ", blocked_q[i]->pid);
				}
			}
			fprintf(outputfile, "\n");

			if (running_process == NULL) {cpu_idle_t++;}
			else{

				running_process->remaining_t--;

				if (running_process->remaining_t == running_process->burst_t){
					for (int i = 0; i < num_processes; i++){
						if (blocked_q[i] == NULL){
							blocked_q[i] = running_process;
							break;
						}
					}
					blocked_processes++;
					running_process = NULL;
				}
				else if (running_process->remaining_t == 0){
					running_process->exit_t = cycle;
					running_process = NULL;
					active_processes--;
				}
				else{
					ready_q[min_i] = running_process;
					running_process = NULL;
				}
			}

			cycle++;

		}
	}

	//calculate and print statistics
	fprintf(outputfile, "\n");
	fprintf(outputfile, "Finishing time: %d\n", (cycle - 1));
	float cpu_util = (float)(cycle - cpu_idle_t) / (float)cycle;
	fprintf(outputfile, "CPU utilization: %f\n", cpu_util);
	for (int i = 0; i < num_processes; i++){
		fprintf(outputfile, "Turnaround process %d: %d\n", pl[i]->pid, (pl[i]->exit_t - pl[i]->entry_t + 1));
		free(pl[i]);
	}

}