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

typedef struct mem_list
{
	void * addr;
	size_t size;
	int free;
	struct mem_list * next;
	struct mem_list * prev;
	
} mem_list;

mem_list * list = NULL;
int list_ct = 0;

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
  return NULL;
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
void *malloc(size_t size)
{
	mem_list * p = list;
	mem_list * chosen = NULL;
	
	while(p != NULL){
		if(p->free && (p->size >= size )){
			if(chosen == NULL || (chosen && p->size < chosen->size)){
				chosen = p;
			}
		}
		
		p = p->next;
	}
	
	if(chosen != NULL){
		chosen->free = 0;
		/**
		size_t * t = chosen->addr + size;
		*(t) = size;
		*/
		return chosen->addr;
	}
	
	chosen = sbrk(0);
	sbrk(sizeof(mem_list));
	chosen->addr = sbrk(0);
	if(sbrk(size) == (void *)-1){
		return NULL;
	}
	//chosen->addr += sizeof(mem_list);
	chosen->size = size;
	chosen->free = 0;
	chosen->next = list;
	chosen->prev = NULL;
	/**
	size_t * t = chosen->addr + size;
	*(t) = size;
	*/
	if(list != NULL){
		list->prev = chosen;
	}
	list = chosen;
	
	return chosen->addr;
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
void free(void *ptr)
{
	// "If a null pointer is passed as argument, no action occurs."
	if (!ptr)
		return;

	mem_list * temp = (mem_list * )(ptr - sizeof(mem_list));
	
	temp->free = 1;
	/**
	if(temp->next != NULL){
		if(temp->next->free){
			size_t s = temp->size + temp->next->size + sizeof(mem_list);
			temp->size = s;
			mem_list * te = temp->next->next;
			temp->next = te;
			if(te != NULL){
				te->prev = temp;
			}
		}
	}
	
	if(temp->prev != NULL){
		if(temp->prev->free){
			size_t s = temp->size + temp->prev->size + sizeof(mem_list);
			temp->prev->size = s;
			
			temp->prev->next = temp->next;
			if(temp->next != NULL){
				temp->next->prev = temp->prev;
			}
		}
	}
	*/
	return;
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
 // Optimization hint: Suppose size is a bit less than your existing allocation...
 
 	if(!size){
 		free(ptr);
 		return NULL;
 	}
 	
 	size_t old_size = 0;
 	void * new_ptr = malloc(size);
 	
 	if(!ptr)
 		return new_ptr;
 	mem_list * p = list;
 	
 	while(p){
 		if(p->addr == ptr){
 			old_size = p->size;
 			break;
 		}
 		p = p->next;
 	}
 	memcpy(new_ptr, ptr, old_size < size ? old_size : size);
 	free(ptr);

	return new_ptr;
}
