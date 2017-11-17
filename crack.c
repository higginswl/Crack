// Will Higgins
// 10/3/2017

// Include file goes here
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#define _XOPEN_SOURCE
#include <crypt.h>
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <omp.h>
#define buffer_size 256
char salt[3];
char* target;
int end = 0;
int first = 0;
int init = 'a';
pthread_mutex_t lock;

struct myStruct{
        char place[9];
        int index;
        int final;
	struct crypt_data data;
};


void* search_char(char* place1, int index, int final, int current, struct crypt_data* data){
	char* result;
	char* temp = place1 + index;
	int c = 'a';
	for (; c<='z'; c++){
		if (end == 1){
                	break;
       		 }
		*temp = c;
		place1[index] = c;
        	if (index == final){
			place1[0] = current;
			place1[index+1] = '\0';
			//printf("place: %s\n", place1);
			result = crypt_r(place1, salt, data);
			//printf("place: %s, result: %s\n", place1, result);
			if (strcmp(target, result) == 0){
				printf("password is: %s\n", place1);
				end = 1;
				return 0;
			}
		}
		else{
			search_char(place1, index+1, final, current, data);
		}
	}

}

void* thread_entry(void* entry){
        int localinit;
		pthread_mutex_lock(&lock);
		localinit = init;
        	init = init + 1;
        	pthread_mutex_unlock(&lock);
		if(localinit>'z'){
                	end = 1;
                	return 0;
       		 }
        	struct myStruct* arg_ptr = (struct myStruct*) entry;
		arg_ptr->data.initialized = 0;
		int k;
		for(k=arg_ptr->final; k>0; k--){
	        	search_char(arg_ptr->place, arg_ptr->index, k, localinit, &(arg_ptr->data));
		}
	if(localinit>'z'){
                end = 1;
                return 0;
        }
}


int main( int argc, char* argv[] ){


	if( argc != 4 ){
                printf("Usage: crack <threads> <keysize> <target> \n");
                return -1;
        }

	int threads = atoi(argv[1]);
	int final = atoi(argv[2]);
	target = argv[3];
	salt[0] = target[0];
	salt[1] = target[1];
	salt[2] = '\0';

	printf("target is: %s\n", target);
	printf("salt is: %s\n", salt);

	int retu;
	struct myStruct structs[threads];
        pthread_t tids[threads];

	while(end == 0){
	int i;
	for(i= 0; i<threads; i++){
        	structs[i].index = 1; //direct access
        	structs[i].final = final-1; //direct access

        	pthread_t tid;
        	tids[i] = tid;

        	//printf("Before Thread\n");
        	retu = pthread_create(&tids[i], NULL, thread_entry, &structs[i]);
        	if (retu != 0) {
                	printf("error");
        	}
	}
	int j;
	for(j= 0; j<threads; j++){
		pthread_join(tids[j], NULL);
        	//printf("After Thread\n");
        }
	}

	exit(0);

        return 0;
}

