/* This is the file you should write your code for the CS241 malloc mp challenge
  There is malloc,realloc,calloc and free waiting for you.
  
  A reminder the University of Illinois and the department of computer science has 
  a policy about what is allowed regarding collaboration and publishing of solutions.
  
  i.e. Your score should be based on your own work and you may not publish your solutions
  or leave them in 'plain view' -

  https://cs.illinois.edu/current-students/undergraduates/policies-and-procedures-0/honor-code-and-academic-integrity
  
  For this MP, the code you submit must be your own work i.e. entirely your own work product.
  
  Getting started: See the instructions on the CS241 website

  If you have questions please use Piazza
  If your code crashes it might be a great time to learn to use a debugger!
  
  The given portions of this code are copyright the original authors (UIUC CS241 staff).
  Recent additions are copyright Lawrence Angrave 2015
  You own the copyright to your own code - however please see the above warning before choosing to publishing it.
  
  This is is a challenging assignment - and considered "Rite of passage" for any system programmer
  
  To score points you need to add your name and netid to the following line copyright line.
  
  Student code: Copyright (C) Yan Geng (yangeng2) 2015
  
  i.e. replace "[Full name]" and "[netid]" with your full name and netid.
  */
  

/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>



typedef struct _entry_t{
	//void *addr;
	//int free;
	size_t size;
	struct _entry_t *next;
    struct _entry_t *prev;
}_entry_t;


//void split(mem_list *, size_t);
//size_t align8(size_t );
//int valid_addr(void * );
//mem_list * get(void * );
//mem_list * combine(mem_list *);

/*
size_t align8(size_t s){
	if((s & 0x7) == 0)
		return s;
	return ((s>>3) + 1) << 3;
}
*/

/*
void split(mem_list * chosen, size_t s){

	mem_list * n = NULL;
	n = chosen->addr + s;
	n->addr = n + BLOCK_SIZE;
	n->prev = chosen;
	n->next = chosen->next;
	chosen->next = n;
	n->size = chosen->size - s - BLOCK_SIZE;

	chosen->size = s;
	if(n->next != NULL){
		n->next->prev = n;
	}
		
	if(chosen == tail){
		tail = n;
		//tail->next = NULL;
		//brk(n);
	}
	
}
*/


/**
 * Allocate space for array in memory
 * 
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 * 
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size)
{
	void *ptr = malloc(num * size);                // allocate new memory
	
	if (ptr)
		memset(ptr, 0x00, num * size);              // zero it

	return ptr;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
 
 

_entry_t *head=NULL;
_entry_t *tail=NULL;
_entry_t *curr=NULL;


void *malloc(size_t size)
{
        if(!head){                                // initialize free list
            head=sbrk(sizeof(_entry_t));
            tail=sbrk(sizeof(_entry_t));
            head->size=0;
            tail->size=0;
            head->next=tail;
            tail->prev=head;
        }

        curr=head;
        while(curr!=tail){
            if(curr->size>=size){                     // find the block in free list
                    curr->prev->next=curr->next;      // remove from free list
                curr->next->prev=curr->prev;
                void* t=(void*)curr+sizeof(_entry_t);
                return t;
            }
            else{
                curr=curr->next;
            }
        }

    _entry_t* temp=sbrk(sizeof(_entry_t));          // not find block allocate new block
    temp->next=NULL;
    temp->prev=NULL;
    temp->size=size;
    sbrk(size);
    return (void*)temp+sizeof(_entry_t);	
}


/**
 * Deallocate space in memory
 * 
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
/* 
  mem_list * get(void * t){       // this function return mem_list contains t 
 	return (void*)t-BLOCK_SIZE;	
 }
 

 int valid_addr(void * t){
 	if(head){
 		if(t >= start && t <= end)
 			return t == get(t)->data;
 	}
 	return 0;
 }
 */
 /*
 mem_list * combine(mem_list * t){
 	if(t->next && t->next->free){
 		t->size += BLOCK_SIZE + t->next->size;
 		t->next = t->next->next;
 		if(t->next){
 			t->next->prev = t;
 		}
 	}
 	return t;
 }
 */
 
 
void free(void *ptr)
{
	if (ptr==NULL) return;                    // check for NULL pointer

    _entry_t* temp=((void*)ptr)-sizeof(_entry_t);        // add block to free list
     temp->prev=tail->prev;
    tail->prev->next=temp;
    tail->prev=temp;
    temp->next=tail;    
}


/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *    
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size)
{

	if (ptr==NULL)                          // check for NULL pointer
		return malloc(size);

	if (size==0)                               // check for 0 size
	{
		free(ptr);
		return NULL;
	}


    _entry_t* temp=(void*)ptr-sizeof(_entry_t);          // find the block
    if(temp->size>=size)                       // if big enough use it
        return ptr;
	
    void* result=malloc(size);              // not big enough allocate new one
	memcpy(result,ptr,temp->size);          // copy memory
	free(ptr);
	return result;
}

