/**
 * Author: Chanpech Hoeng
 * Lastest Updates: 9/11
 * Description:
 * 		Hmalloc.c implement the abstract function declared by hmalloc.h file.
 * 		Through out each implementation, there are some functions that utilized the following helper functions:
 * 		reassign(), search(), newAllocation()
 * Objectives: #include "hmalloc.c" 
 * 1. Call hmalloc() function to dynamically allocate space by first checking the free-list for freed note of sufficient size
 *    otherwise it will calls sbrk() to allocate new space.
 * 2. Call hfree() function to free allocated space through pointer manipulation which are maintain by 
 *    free-list where each node consist of 2 words followed by allocated space. 
 * 3. Call hcalloc() function to performs the same function as hmalloc but also clears the allocated
 *    area.
 * 4. Call traverse() function to start at the free-list head, visit and print the length of each
 * 	  area in the free pool.
 **/
#include "hmalloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void *free_list = NULL;
void *list_head = NULL;

void *reassign(void *node, int *previousLink)
{
	//printf("Reassigning!!!\n");
	int *link = node + 4;
	//printf("LINK IS %d \n", *link);
	if (previousLink == NULL)
	{
		void *nextHead = node + *link;				 // temporary of current entry
		void *reuseNode = free_list;
		//printf("Current head is %p and node is %p and will be set to %p\n", free_list, node, nextHead);
		free_list = nextHead;					 // update current entry to new entry starting at length
		//printf("Free List New Head is %p \n", free_list);
		return reuseNode;
		//This is not completely right
	}else if(*link == 0){ //Was *link == 0
		//printf("FOUND LAST\n");
		void *previousNode = node - *previousLink;
		void *reuseNode = node;
		//printf("Current head is %p previous link is %d and previous node is at %p\n", node, *previousLink, previousNode);
		*previousLink = 0; //Removing last node from the list
		return reuseNode;
	}else{
		//printf("FOUND IN THE MIDDLE\n");
		void *reuseNode = node;
		void *previousNode = node - *previousLink; //Getting the address of previous node
		void *nextNode = node + *link;				//Getting the address of the next node
		int new_link_dist = nextNode - previousNode;
		//printf("\nCurrent previousLink: %d previousNode %p, currentNode %p, nextNode %p\n", *previousLink, previousNode, node, nextNode);
		*previousLink = new_link_dist; //Skipping the current node by setting the previous link to the next node
		//printf("previousLink has been updated to %d\n", *previousLink);
		return reuseNode;
	}
}

void *search(int bytes_to_allocate)
{
	//printf("Searching for available node that will suffice space!\n");
	void *node = free_list;
	int *length = node;
	int *link = node + 4;
	int *previousLink = NULL;
	while (1)
	{
		if (*length >= bytes_to_allocate)
		{
			//printf("Node at address %p has a length of %d and pointing to %d that will suffice\n", node, *length, *link);
			void* reuse_node = reassign(node, previousLink);
			return reuse_node + 8;
		}
		if (*link == 0) { //Reached the end of the list with no suffice space
			//printf("Reached the end of the list with no suffice space\n");
			return NULL;
		}

		node = node + *link;
		length = node;
		previousLink = link;
		link = node + 4;
	}
	//This return should never be triggered.
	return NULL;
}

void *newAllocation(int bytes_to_allocate)
{
	//printf("Size of int is %d\n", bytes_to_allocate);

	void *startOfAllocation;
	int *length;
	int *link;
	void *address = sbrk(0);

	//printf("The current address is %p\n", address);

	length = sbrk(sizeof(int)); // n store the length
	address = sbrk(0);
	//printf("Alocated %ld bytes for length. The current address is %p\n", sizeof(int), address);

	link = sbrk(sizeof(int)); // n + 4 store the link
	address = sbrk(0);
	//printf("Alocated %ld bytes for pointer. The current address is %p\n", sizeof(int), address);

	startOfAllocation = sbrk(bytes_to_allocate); // n + 8 beyound is start the allocated area
	address = sbrk(0);
	//printf("Allocation %d requested bytes and the end of allocated space is address %p\n", bytes_to_allocate, address);

	*length = bytes_to_allocate; // Additional 8 bytes from the two word
	*link = 0;
	//printf("Length: %d\n\n", *length);

	return startOfAllocation;
}

/* traverse
 * Start at the free list head, visit and print the length of each
 * area in the free pool. Each entry should be printed on a new line.
 */
void traverse()
{
	/* Printing format:
	 * "Index: %d, Address: %08x, Length: %d\n"
	 *    -Index is the position in the free list for the current entry.
	 *     0 for the head and so on
	 *    -Address is the pointer to the beginning of the area.
	 *    -Length is the length in bytes of the free area.
	 */
	// NOTES!!!
	// When traversing through the linked list and search for x avaliable of space.
    // We can give the address with length equal or greater than the requested.
	// Check if nothing exist we break the traversal/
	if (free_list == NULL)
	{ // if nothing exist we just exit
		return;
	}
	void *node = free_list;
	int *length = node;
	int *link = node + 4;
	int num = 1;
	while (1)
	{
		printf("Index: %d, Address: %08x, Length: %d\n", num, node, *length);
		if (*link == 0)
			break;
		node = node + *link;
		length = node;
		link = node + 4;
		num++;
	}
}

/* hmalloc
 * Allocation implementation.
 *    -will not allocate an initial pool from the system and will not
 *     maintain a bin structure.
 *    -permitted to extend the program break by as many as user
 *     requested bytes (plus length information).
 *    -keeps a single free list which will be a linked list of
 *     previously allocated and freed memory areas.
 *    -traverses the free list to see if there is a previously freed
 *     memory area that is at least as big as bytes_to_allocate. If
 *     there is one, it is removed from the free list and returned
 *     to the user.
 */
void *hmalloc(int bytes_to_allocate)
{
	void *startOfAllocation;
	if (free_list == NULL)
	{
		startOfAllocation = newAllocation(bytes_to_allocate);
		printf("!!!ALLOCATED NEW SPACE!!!\n");
	}
	else
	{
		// The search function will search for node with space that suffice the request.
		// If found it will automatically reassign with reassign() function.
		void *availableNode = search(bytes_to_allocate);
		
		if (availableNode == NULL)
		{
			startOfAllocation = newAllocation(bytes_to_allocate);
		}else{
			startOfAllocation = availableNode;
		}
	}
	return startOfAllocation;
}

/* hcalloc
 * Performs the same function as hmalloc but also clears the allocated
 * area by setting all bytes to 0.
 */
void *hcalloc(int bytes_to_allocate)
{
	void *zero = hmalloc(bytes_to_allocate);
	int temp = 0;
	char *allocate = zero;
	while (temp < bytes_to_allocate) //This looked like it could leak over to some other space
	{
		*allocate = '\0';		
		allocate = allocate + 1; // The 1 here is an integer so it will be of size 4 byte.
		temp++;
	}

	return zero;
}

/* hfree - step 5
 * Responsible for returning the area (pointed to by ptr) to the free
 * pool.
 *    -simply appends the returned area to the beginning of the single
 *     free list.
 */
void hfree(void *ptr)
{
	//printf("Started hfree\n");
	//printf("Current free-list head %p \n", free_list);
	if (free_list == NULL)
	{						   // if list is empty set n - 8 the beginning.
		free_list = (ptr - 8); // point free_list meta (ptr - 8 store the length)
		//printf("free_list start at %p with ptr of %p\n", free_list, ptr);
		//list_head = free_list; // keep track of the beginning of the free_list
		return;
	}
	void *tmpHead = free_list;				 // temporary of current entry
	int link_dist = ((tmpHead) - (ptr - 8)); // get the distance between new entry and old entry
	int *link = ptr - 4;					 // Locate the linker in the new ptr
	
	/*
	int *lenght = tmpHead;
	printf("Old head address is at n is %p with a length of %d\n", tmpHead, *lenght);
	printf("New ptr address is at n is %p\n", (ptr-8));
	printf("The ptr will be the new head and link distance to the old head at %d\n", link_dist);
	*/

	free_list = (ptr - 8); // update current entry to new entry starting at length
	*link = link_dist;	   // The new head should now have a distance that point to the next node
}

/* hrealloc
The realloc() function shall deallocate the old object pointed to
       by ptr and return a pointer to a new object that has the size
       specified by size.  The contents of the new object shall be the
       same as that of the old object prior to deallocation, up to the
       lesser of the new and old sizes. Any bytes in the new object
       beyond the size of the old object have indeterminate values. If
       the size of the space requested is zero, the behavior shall be
       implementation-defined: either a null pointer is returned, or the
       behavior shall be as if the size were some non-zero value, except
       that the behavior is undefined if the returned pointer is used to
       access an object. If the space cannot be allocated, the object
       shall remain unchanged.


 */
void *hrealloc(void *ptr, int new_size)
{
	void *new_area = hmalloc(new_size);
	int temp = 0;
	char *copy = new_area;
	printf("While LOOP!\n");
	while (temp < new_size) //This looked like it could leak over to some other space
	{
		*copy = '\0';
		copy = copy + 1; 
		temp++;
	}
	if(ptr != NULL)
		hfree(ptr);
	return new_area;
}