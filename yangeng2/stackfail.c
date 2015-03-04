#include <pthread.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <semaphore.h>

#define N (10)
#define OPS (10)
#define CHECK(expression, mesg) if((expression)) { write(1,mesg,strlen(mesg)); write(1,"\n\n",2); exit(1);}

//  gcc -Wall -pthread -o stackfail stackfail.c 

// Note we are using an array as a simple stack with push and pop operations
// With a classic producer consumer problem
// you would normally use the circular queue (aka Ring Buffer) to ensure FIFO properties



int data[N];
int count = 0;
int c = 0;

pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;
sem_t sem1;
sem_t sem2;
pthread_cond_t cv;



int in_critical_section; // is there a thread inside a critical section?

// Do not modify this method; it represents the critical section where we access 
// the datastructure. All thread-safety must be implemented in the caller
void unsafepush(int value) {
  in_critical_section++;
  usleep(rand() & 3); // make race conditions more likely
 // printf("%d\n", count);
  CHECK(count < 0, "Ooops - unsafepush: invalid count (N negative)");   //
  CHECK(count > N, "Ooops - unsafepush: invalid count (N too large, buffer overflow)");  //
  CHECK(in_critical_section > 1,"Ooops - no mutual exclusion");
  
  if(count == N){
  	count = 0;
  } 
  
  data[count++] = value;
  c++;
  in_critical_section--;
}

// Do not modify this method; it represents the critical section where we access 
// the datastructure. All thread-safety must be implemented in the caller
int unsafepop() {
  in_critical_section++;
  usleep(rand() & 3); // make race conditions more likely
  CHECK(count < 0, "Ooops - unsafepop: invalid count (N zero or negative, buffer underflow)");   //
  CHECK(count > N, "Ooops - unsafepop: invalid count (N too large)");    //
  CHECK(in_critical_section > 1,"Ooops - no mutual exclusion");
  
  if(count == 0){
		count = N;
	}

  int result=data[--count];
  c--;
  in_critical_section--;
  return result;
} 

typedef int (*pop_fn)();
typedef void (*push_fn)();

int first_time=1; // So we can print out a message once. This message might not be printed if our implementation fails immediately.

/* VERSION 1 - Use a mutex 
Only one thread can be in the Critical section at a time (Hurray!)
However we don't prevent buffer underflow or overflow */

void push_v1(int value) {
  if(first_time) {first_time=0; puts("V1; I expect this version to have NO race condition but suffer from buffer overflow and underflow");}
  
  pthread_mutex_lock(&MUTEX);  
  unsafepush(value); // CRITICAL SECTION
  pthread_mutex_unlock(&MUTEX);  
  
}
int pop_v1() {
  pthread_mutex_lock(&MUTEX);  
  int result = unsafepop(); // CRITICAL SECTION
  pthread_mutex_unlock(&MUTEX);;
  return result;
}


////----- Version 2 
// What will happen here!?

void push_v2(int value) {
  if(first_time) {first_time=0; puts("V2; I expect this version to suffer from deadlock, the push does not unlock the mutex");}
  pthread_mutex_lock(&MUTEX);
  while(count == N) {usleep(1);}
  unsafepush(value); // CRITICAL SECTION
}

int pop_v2(int value) {
  pthread_mutex_lock(&MUTEX);
  
  while(count ==0){usleep(1);}
  int result = unsafepop(); // CRITICAL SECTION
  
  pthread_mutex_unlock(&MUTEX);
  return result;
}

////----- Version 3 - Semaphores (not OSX)
// Can you make a version that actually works with semaphores!


void push_v3(int value) {
  if(first_time) {first_time=0; puts("V3; semaphores");}
  
  sem_wait(&sem1);
  pthread_mutex_lock(&MUTEX);  

  unsafepush(value); // CRITICAL SECTION
  	
  pthread_mutex_unlock(&MUTEX);
  sem_post(&sem2);
}

int pop_v3(int value) {
  sem_wait(&sem2);

  pthread_mutex_lock(&MUTEX);  

  int result = unsafepop(); // CRITICAL SECTION
  pthread_mutex_unlock(&MUTEX);
  
  sem_post(&sem1);
  return result;
}

////------ Version 4; Modify version 2 to make a version that uses a condition variable
void push_v4(int value) {
  if(first_time) {first_time=0; puts("V4; Condition variable ");}
  // Use a condition variable and a loop to block while the buffer is full
  pthread_mutex_lock(&MUTEX);
  while(c == N){
  	pthread_cond_wait(&cv, &MUTEX);
  }
  unsafepush(value); // CRITICAL SECTION
  pthread_cond_broadcast(&cv);
  pthread_mutex_unlock(&MUTEX);
  // Don't forget to broadcast... (in case a pop is blocked)
}

int pop_v4(int value) {
  // Use a condition variable and a loop to block while the buffer is empty
  pthread_mutex_lock(&MUTEX);
  while(c == 0){
  	pthread_cond_wait(&cv, &MUTEX);
  }
  int result = unsafepop(); // CRITICAL SECTION
  pthread_cond_broadcast(&cv);
  pthread_mutex_unlock(&MUTEX);
  // Don't forget to broadcast... (in case a pop is blocked)
  return result;
}
//// Version 5 make a bad implementation of your choice
// Challenge:
// Can you create an incorrect verson that does not deadlock, that does not allow buffer underflow or buffer overflow 
// i.e. count stays in range BUT data is occasionally lost or overwritten?
void push_v5(int value) {
  if(first_time) {first_time=0; puts("V5; Blah ");}
  sem_wait(&sem1);
  
  unsafepush(value); // CRITICAL SECTION
  
  sem_post(&sem2);
}

int pop_v5(int value) {
	sem_wait(&sem2);
  int result = unsafepop(); // CRITICAL SECTION
  	sem_post(&sem1);
  return result;
}



//// Version 6 make another  bad implementation of your choice
// Challenge:
// Can you create an incorrect verson that does not deadlock, that does not allow buffer underflow or buffer overflow 
// i.e. count stays in range BUT data is occasionally lost or overwritten?
void push_v6(int value) {
  if(first_time) {first_time=0; puts("V6; Blah ");}
 while(count == N){
 	pthread_cond_wait(&cv, &MUTEX);
 } 
  unsafepush(value); // CRITICAL SECTION
  pthread_cond_broadcast(&cv);
}

int pop_v6(int value) {
	while(count == 0){
		pthread_cond_wait(&cv, &MUTEX);
	}
  int result = unsafepop(); // CRITICAL SECTION
  pthread_cond_broadcast(&cv);
  return result;
}




// Test runner
int producer_push_count;
int consumer_pop_count;

void* producer(void*arg) {
  push_fn push = (push_fn)arg;
  assert(push);
  usleep(rand() &15); // randomize who gets to go first
  int i = 0;
  long total = 0;
  while(i < OPS) {
    total += i;
    push(i);
    i++;
    producer_push_count++;
    
    if(!((i/100)&1)) sleep(rand() &3); // slow producer at times
  }
  puts("Producer finished");
  return NULL;
}

void* consumer(void*arg) {
  pop_fn pop = (pop_fn)arg;
  assert(pop);
  usleep(rand() &15); // randomize who gets to go first

  int i =0;
  long total = 0;
  while(i < OPS) {
    total += pop();
    consumer_pop_count++;
    i++;
    if((i/150)&1) sleep(rand() &3); // slow consumer at times
  }
  puts("Consumer finished");
  return NULL;
}

// Hey look it's a function table in C! Move aside C++ virtual functions :-)
// Be careful of copy-paste errors when adding more functions
pop_fn pop_versions[] = {NULL,pop_v1,pop_v2,pop_v3,pop_v4,pop_v5,pop_v6};
push_fn push_versions[] = {NULL,push_v1,push_v2,push_v3,push_v4,push_v5,push_v6};

#define NUMBER(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))

void print_usage(char*progname) {

  printf("Usage: %s 1-%d\n", progname, (int)NUMBER(pop_versions) -1);
  exit(2);
}
int main(int argc, char**argv) {
	sem_init(&sem1, 0, N);
	sem_init(&sem2, 0, 0);
	pthread_cond_init(&cv, NULL);
	
  CHECK(sizeof(pop_versions) != sizeof(push_versions),"Fix function arrays - they are not the same size")
  
  int version = argc == 2? atoi(argv[1]) : 0;
  if(version<1|| version >= NUMBER(pop_versions)) print_usage(argv[0]);
  
  pthread_t tid1, tid2;  
  pthread_create(&tid1,NULL, producer,push_versions[version]);
  pthread_create(&tid2,NULL, consumer,pop_versions[version]);
  sleep(2);
  printf("After 2 seconds:\nproducer_push_count = %d\nconsumer_pop_count = %d\n", producer_push_count, consumer_pop_count); 
  sleep(2);
  printf("\nAfter 2 seconds.\nproducer_push_count = %d\nconsumer_pop_count = %d\n", producer_push_count, consumer_pop_count); 
  puts("Main thread now waiting for producer and consumer to finish");
  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);
  printf("\nFinished.\nproducer_push_count = %d\nconsumer_pop_count = %d\n", producer_push_count, consumer_pop_count);  
  
  sem_destroy(&sem1);
  sem_destroy(&sem2);
  pthread_mutex_destroy(&MUTEX);
  return 0;
}
