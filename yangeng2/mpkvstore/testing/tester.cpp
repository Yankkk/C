extern "C" {
#include "datastore_control.h"
#include "test_utility.h"
}

#include <cstring>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;

int STRESS_NUMBER = 1000;

char rand_char()
{
    return (char)( (rand() % 52) + 65);
}

void* thread_test3(void* arg){
    int start = *((int*)arg);
    int i;
    for(i=start; i<STRESS_NUMBER; i+=2){
	if (!STD_ADD_SUCCESS(i)){
	    fail(3);
	}
    }
    for(i=start; i<STRESS_NUMBER; i+=2){
	if (!STD_UPDATE_SUCCESS(i, i+1, 1)){
	    fail(3);
	}
    }
    for(i=start; i<STRESS_NUMBER; i+=2){
	if (!STD_GET_SUCCESS(i, i+1, 2)){
	    fail(3);
	}
    }
    for(i=start; i<STRESS_NUMBER; i+=2){
	if (!STD_DELETE_SUCCESS(i)){
	    fail(3);
	}
    }
    for(i=start; i<STRESS_NUMBER; i+=2){
	if (!STD_GET_FAILURE(i, i, 2)){
	    fail(3);
	}
    }
}

void* thread_test4(void* arg){
    int start = ((int*)arg)[0];
    int sel = ((int*)arg)[1];
    int i;
    switch(sel)
    {
    case 0:
	for(i=start; i<STRESS_NUMBER; i+=2){
	    if (!STD_ADD_SUCCESS(i)){
		//cout << "failing here 1" << endl;
		fail(4);
	    }
	}
	break;
    case 1:
	for(i=start; i<STRESS_NUMBER; i+=2){
	    if (!STD_UPDATE_SUCCESS(i, i+1, 1)){
		//cout << "failing here 2 with i = " << i << endl;
		fail(4);
	    }
	}
	break;
    case 2:
	for(i=start; i<STRESS_NUMBER; i+=2){
	    if (!STD_GET_SUCCESS(i, i+1, 2)){
		//cout << "failing here 3 with i = " << i << endl;
		fail(4);
	    }
	}
	break;
    case 3:
	for(i=start; i<STRESS_NUMBER; i+=2){
	    if (!STD_DELETE_SUCCESS(i)){
		//cout << "failing here 4 with i = " << i << endl;
		fail(4);
	    }
	}
	break;
    case 4:
	for(i=start; i<STRESS_NUMBER; i+=2){
	    if (!STD_GET_FAILURE(i, i, 2)){
		//cout << "failing here 5 with i = " << i << endl;
		fail(4);
	    }
	}
	break;
    default:
	break;
    }
}

void test1()
{ 
    init();
    load();

    int i;
    for(i=0; i<STRESS_NUMBER; i++){
	//cout << "getting here 1" << endl;
    	if (!STD_ADD_SUCCESS(i)){
	    cout << "failing here 1" << endl; 
	    fail(1);
	}
    }
    for(i=0; i<STRESS_NUMBER; i++){
	//cout << "getting here 2" << endl;
	if (!STD_UPDATE_SUCCESS(i, i+1, 1)){
	     cout << "failing here 2" << endl; 
	     fail(1);
	}
    }
    for(i=0; i<STRESS_NUMBER; i++){
	//cout << "getting here 3" << endl;
	if (!STD_GET_SUCCESS(i, i+1, 2)){
	     cout << "failing here 3" << endl; fail(1);
	}
    }
    for(i=0; i<STRESS_NUMBER; i++){
	//cout << "getting here 4" << endl;
	if (!STD_DELETE_SUCCESS(i)){
	     cout << "failing here 4" << endl; fail(1);
	}
    }    
    for(i=0; i<STRESS_NUMBER; i++){
	//cout << "getting here 5" << endl;
	if (!STD_GET_FAILURE(i, i, 2)){
	    cout << "failing here 5" << endl; fail(1);
	}
    }
    save();				     
    success(1);
}

void test2()
{ 
    init();
    load();
    int i;
    for(i=0; i<STRESS_NUMBER; i++){
	if (!STD_ADD_SUCCESS(i))
	    fail(2);
    }
    for(i=0; i<STRESS_NUMBER; i++){
	if (!STD_UPDATE_SUCCESS(i, i+1, 1))
	    fail(2);
    }
    for(i=0; i<STRESS_NUMBER; i++){
	if (!STD_GET_SUCCESS(i, i+1, 2))
	    fail(2);
    }
    for(i=0; i<STRESS_NUMBER; i++){
	if (!STD_DELETE_SUCCESS(i))
	    fail(2);
    }
    for(i=0; i<STRESS_NUMBER; i++){
	if (!STD_GET_FAILURE(i, i, 2))
	    fail(2);
    }
    save();
    success(2);
}

void test3()
{
    init();
    load();

    int e = 0;
    int o = 1;
    pthread_t * thread1 = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_t * thread2 = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_create(thread1, NULL, thread_test3, &e);
    pthread_create(thread2, NULL, thread_test3, &o);
    pthread_join(*thread1, NULL);
    pthread_join(*thread2, NULL);

    free(thread1);
    free(thread2);
    
    save();
    success(3);
}

void test4()
{
    srand(time(0));
    string data[1000];
    
    for(int j = 0; j < 1000; j++)
    {
	int len = 25 + ( std::rand() % 39 );
	string key;
	
	for(int i = 0; i < len; i++)
	{	 
	    key += rand_char();
	    if (i%7 == 0) 
		key[i] = toupper(key[i]); 
	}	
	data[j] = key;
    }
    init();
    load();
    int i;
    for(i=0; i<STRESS_NUMBER; i++){
	if (!STD_ADD2_SUCCESS(data[i].c_str(), data[i].c_str())){
	    //cout << "failing here 1 at i = " << i << endl;
	    fail(4);
	}
    }
    for(i=1; i<STRESS_NUMBER; i++){
	if (!STD_UPDATE2_SUCCESS(data[i].c_str(), data[i-1].c_str(), 1)){
	    //cout << "failing here 2" << endl;
	    fail(4);
	}
    }
    for(i=1; i<STRESS_NUMBER; i++){
	if (!STD_GET2_SUCCESS(data[i].c_str(), data[i-1].c_str(), 2)){
	    //cout << "failing here 3" << endl;
	    fail(4);
	}
    }
    for(i=0; i<STRESS_NUMBER; i++){
	if (!STD_DELETE2_SUCCESS(data[i].c_str())){
	    //cout << "failing here 4" << endl;
	    fail(4);
	}
    }
    for(i=0; i<STRESS_NUMBER; i++){
	if (!STD_GET2_FAILURE( data[i].c_str(), data[i].c_str(), 0)){ 
	    //cout << "failing here 3 @ i = " << i << " with key = " << data[i] << endl;
	    fail(4);
	}
    }
    save();
    success(4);
}

void test5()
{
    srand(time(0));
    string data[1000];

    for(int j = 0; j < 1000; j++)
    {
	
	int len = 25 + ( std::rand() % 39 );
	string key;
	
	for(int i = 0; i < len; i++)
	{
	    key += rand_char();
	    if (i%7 == 0)
		key[i] = toupper(key[i]);
	}
	data[j] = key;
    }

    init();
    load();
    int i;
    for(i=0; i<STRESS_NUMBER/2; i++){
	if (!STD_ADD2_SUCCESS((char*) data[i].c_str(), (char*) data[i].c_str())){
	    //cout << "failing here 1 at i = " << i << endl;             
	    fail(4);
	}
    }
    
    //cout << "got through to here first" << endl;

    save();
    for(i=STRESS_NUMBER/2; i < STRESS_NUMBER; i++)
    {
	if (!STD_ADD2_SUCCESS(data[i].c_str(), data[i].c_str())){                          
	    fail(5);
	}	
    }

    //cout << "got through to here" << endl;
    for(i=0; i<STRESS_NUMBER/2; i++){
	if (STD_DELETE2_SUCCESS((char*) data[i].c_str())){
	    fail(5);
	}
    }
    load();

    for(i=1; i<STRESS_NUMBER; i++){
        if (!STD_UPDATE2_SUCCESS((char*) data[i].c_str(), (char*) data[i-1].c_str(), 1)){
            cout << "failing here 3 @ i = " << i << " with key = " << data[i] << endl;
            fail(5);
        }
    }

    for(i=1; i<STRESS_NUMBER; i++){
	if (!STD_DELETE2_SUCCESS(data[i].c_str()))
	{
	    cout << "failing here 3" << endl;                                                                            fail(5);
	}
    }
    
    save();
    //cout << "getting through to here pre-final" << endl;
    success(5);
}

int main(int argc, char **argv)
{ 
      
    mkdir("data",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    if(argc!=1){	
	switch(atoi(argv[1])){
	case 1:
	    test1();
	    break;
	case 2:
	    test2();
	    break;
	case 3:
	    test3();
	    break;
	case 4:
	    test4();
	    break;
	case 5:
	    test5();
	    break;
	default:
	    break;
	}
    }else{
	test1();
	test2();
	test3(); 
	test4();
	test5();
	cout << "You passed all test-cases!" << endl;
    }
    
    system("rm -rf data");
   
    return 0;
}

