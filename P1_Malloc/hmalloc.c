/**
 * Author: Chanpech Hoeng
 * Lastest Updates: 9/17
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

/* reassign(node, previousLink)
 * Where node is the address of the node which suffice the requested bytes_of_allocated space. 
 * PreviousLink is a variable used to back track to previous node remove the current node from the list.
 * Respectively the three case which are defined are: 
 * 1. previousLink == NULL mean that the node is at the head.
 * 2. *link == 0 mean that the node is at the end of the list.
 * 3. else it will be in between the head and end.
 */
void *reassign(void *node, int *previousLink)
{
	int *link = node + 4;
	if (previousLink == NULL)
	{
		void *nextHead = node + *link; // temporary of current entry
		void *reuseNode = free_list;
		free_list = nextHead; // update current entry to new entry starting at length
		return reuseNode;
	}
	else if (*link == 0)
	{
		void *reuseNode = node;
		*previousLink = 0; // Removing last node from the list
		return reuseNode;
	}
	else
	{
		void *reuseNode = node;
		void *previousNode = node - *previousLink; // Getting the address of previous node
		void *nextNode = node + *link;			   // Getting the address of the next node
		int new_link_dist = nextNode - previousNode;
		*previousLink = new_link_dist; // Skipping the current node by setting the previous link to the next node
		return reuseNode;
	}
}

/* search()
 * Search for an available node within the free_list which will suffice the requeted bytes.
 * If found we return the address of the allocated space 
 * (meaning n+8 and not at n which is the meta data). 
 * Return NULL otherwise.
 */
void *search(int bytes_to_allocate)
{
	void *node = free_list;
	int *length = node;
	int *link = node + 4;
	int *previousLink = NULL;

	while (1)
	{
		if (*length >= bytes_to_allocate)
		{
			void *reuse_node = reassign(node, previousLink);
			return reuse_node + 8;
		}
		if (*link == 0)
		{ // Reached the end of the list with no suffice space
			return NULL;
		}
		node = node + *link;
		length = node;
		previousLink = link;
		link = node + 4;
	}
	// This return should never be triggered.
	return NULL;
}

/* newAllocation
 * sbrk() the bytes_to_allocate + 8 additional bytes for the 2 words.
 * The 2 words will act as a meta data. Storing length and linker.
 * return the allocateed space address
 */
void *newAllocation(int bytes_to_allocate)
{
	void *startOfAllocation;
	int *length;
	int *link;

	length = sbrk(sizeof(int));					 // n store the length
	link = sbrk(sizeof(int));					 // n + 4 store the link
	startOfAllocation = sbrk(bytes_to_allocate); // n + 8 beyound is start the allocated area
	*length = bytes_to_allocate;				 // Additional 8 bytes from the two word
	*link = 0;

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
	int num = 0;
	//
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
	}
	else
	{
		// The search function will search for node with space that suffice the request.
		// If found it will automatically reassign with reassign() function.
		void *availableNode = search(bytes_to_allocate);

		if (availableNode == NULL)
		{
			startOfAllocation = newAllocation(bytes_to_allocate);
		}
		else
		{
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
	while (temp < bytes_to_allocate) // This looked like it could leak over to some other space
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
	if (free_list == NULL)
	{						   // if list is empty set n - 8 the beginning.
		free_list = (ptr - 8); // point free_list meta (ptr - 8 store the length)
		return;
	}
	void *tmpHead = free_list;				 // temporary of current entry
	int link_dist = ((tmpHead) - (ptr - 8)); // get the distance between new entry and old entry
	int *link = ptr - 4;					 // Locate the linker in the new ptr

	free_list = (ptr - 8); // update current entry to new entry starting at length
	*link = link_dist;	   // The new head should now have a distance that point to the next node
}

/* hrealloc
	The realloc() function shall deallocate the old object pointed to
	   by ptr and return a pointer to a new object that has the size
	   specified by size. 
 */
void *hrealloc(void *ptr, int new_size)
{
	void *new_area = hmalloc(new_size);
	int temp = 0;
	char *copy = new_area;
	while (temp < new_size) // This looked like it could leak over to some other space
	{
		*copy = '\0';
		copy = copy + 1;
		temp++;
	}
	if (ptr != NULL)
		hfree(ptr);
	return new_area;
}