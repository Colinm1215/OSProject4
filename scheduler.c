#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int currentTime = 0;

struct job {
    int id ;
    int length ;
    // other meta data
    struct job * next;
    int responseTime;
    int waitTime;
    int turnAround;
};

struct job *job_list;

// function that prints job meta data
void print_job(struct job * j) {
    printf("Job %d: length %d\n", j->id, j->length);
}

//Read in the jobs from the file
struct job * read_jobs(char * filename) {
    // open the file
    FILE * fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Couldn't open file\n");
        return NULL;
    }
    // read in the jobs
    struct job * start = NULL;
    struct job * curr = NULL;
    int id = 0;
    int runtime;
    while (fscanf(fp, "%d", &runtime) == 1) {
        struct job * myJOB = malloc(sizeof(struct job));
        myJOB->id = id;
        myJOB->length = runtime;
        myJOB->next = NULL;
        if (start == NULL) {
            start = myJOB;
        } 
        else {
            curr->next = myJOB;
        }
        curr = myJOB;
        id++;
    }
    fclose(fp);
    return start;
}

void print_job_order(void){
    struct job * current_job = job_list;
    while(current_job != NULL){
        printf("job %d will run for: %d\n", current_job->id, current_job->length);
        current_job = current_job->next;
    }
}

// Function that executes jobs first in first out
void execute_jobs_fifo(struct job * job_list) {
    struct job * curr = job_list;
    while (curr != NULL) {
        curr->responseTime = currentTime;
        curr->waitTime = currentTime;
        currentTime += curr->length;
        curr->turnAround = currentTime;
    }
    print_job_order();
}

// Function moves job to end of list based on ID
void move_to_end(int number_of_jobs, int id) {
    struct job * current_job = job_list;
    struct job * prev_job = NULL;
    while (current_job != NULL) {
        if (current_job->id == id) {

            //If we are the first element in the list
            if (prev_job == NULL) {
                job_list = current_job->next;
            }
            //If we are somewhere in the middle of the list 
            else {
                prev_job->next = current_job->next;
            }
            //Move the job to the end of the list
            struct job * tmp = job_list;
            //current_job->next = NULL;
            
            for(int i = 0; i < number_of_jobs - 2; i++){
                tmp = tmp->next;
            }
            current_job->next = tmp->next;
            tmp->next = current_job;
            return;
        }
        prev_job = current_job;
        current_job = current_job->next;
    }
    
}



// Function that executes jobs in order of length
int get_index_of_largest(int my_size) {
    // Sort the jobs by length
    struct job * current_job = job_list;
    //struct job * sorted_list = NULL;
    int max = current_job->length;
    int index_of_max = current_job->id;

    while(current_job->next != NULL && my_size > 0){
        if(current_job->length > max){
            max = current_job->length;
            index_of_max = current_job->id;
        }
        current_job = current_job->next;
        my_size--;
    }
    return index_of_max;

}

//Return the total number of jobs
int get_total_jobs(){
    struct job * current_job = job_list;
    int total_jobs = 0;
    while(current_job != NULL){
        total_jobs++;
        current_job = current_job->next;
    }
    return total_jobs;
}



void execute_jobs_sjf(struct job * job_list) {
    int starting_total_jobs = get_total_jobs();

    for(int i = starting_total_jobs; i > 1; i--){
        int index_of_largest = get_index_of_largest(i);
        move_to_end(i, index_of_largest);
    }

    
    struct job * curr = job_list;

    while (curr != NULL) {
        curr->responseTime = currentTime;
        curr->waitTime = currentTime;
        currentTime += curr->length;
        curr->turnAround = currentTime;
    }

    print_job_order();
}

void execute_jobs_rr(int timeSlice) {
    struct job * current_job = job_list;
    struct job * last_job = job_list;

    while (last_job->next != NULL) {
        last_job = last_job->next;
    }

    while (current_job != NULL) {
        if (current_job->length > timeSlice) {
            int leftover = current_job->length-timeSlice;
            current_job->length = timeSlice;
            struct job * preempted = malloc(sizeof(struct job));
            preempted->id = current_job->id;
            preempted->length = leftover;
            preempted->next = NULL;
            last_job->next = preempted;
            last_job = last_job->next;
        }
        current_job = current_job->next;  
    }

    
    struct job * curr = job_list;
    
    print_job_order();
}

int main(int argc, char **argv){
    if( argc == 4 ) {
        char* policyName = (char*) argv[1];
        char* jobTrace = (char*) argv[2];
        int timeSlice = atoi(argv[3]);
        printf("Execution trace with %s:\n", policyName);
        job_list = read_jobs(jobTrace);
        if ((strcmp("FIFO",policyName) == 0)) {
            execute_jobs_fifo(job_list);
        } else if ((strcmp("SJF",policyName) == 0)) {
            execute_jobs_sjf(job_list);
        } else if ((strcmp("RR",policyName) == 0)) {
            execute_jobs_rr(timeSlice);
        }
        printf("End of execution with %s\n", policyName);
   }
   else if( argc > 4 ) {
      printf("Too many arguments supplied.\n");
   }
   else {
      printf("3 arguments expected.\n");
   }
    return 0;
}