/**
 * Author: Chanpech Hoeng
 * Lastest Updates: 9/17
 **/
#include <stdio.h>
#include <stdlib.h>
#include "hmalloc.h"
/*You may include any other relevant headers here.*/

// Creating a link list data structure with struct

/*	main()
 *	Use this function to develop tests for hmalloc. You should not
 *	implement any of the hmalloc functionality here. That should be
 *	done in hmalloc.c
 *	This file will not be graded. When grading I will replace main
 *	with my own implementation for testing.*/
int main(int argc, char *argv[])
{

  char *t1, *t2, *t3, *t4;
  t1 = hmalloc(sizeof(char) * 12);
  t2 = hmalloc(sizeof(char) * 5);
  t3 = hmalloc(sizeof(char) * 10);
  t4 = hmalloc(sizeof(char) * 8);
  printf("The address to the beginning of the allocation is %p\n", t1);
  printf("The address to the beginning of the allocation is %p\n", t2);
  printf("The address to the beginning of the allocation is %p\n", t3);
  printf("The address to the beginning of the allocation is %p\n", t4);
  printf("\n");

  traverse();
  hfree(t1);
  hfree(t2);
  hfree(t3);
  hfree(t4);
  printf("\n");
  traverse();

  // Now we have some free space in the free-list
  // We should able to reuse them
  printf("\nTrying to reuse\n");
  t1 = hmalloc(sizeof(char) * 12);
  printf("t1 reuse have an address at %p\n", t1);
  hfree(t1);
  printf("\n");
  traverse();

  t1 = hcalloc(sizeof(char) * 12); // Not sure if this fully works 100%
  t2 = hcalloc(sizeof(char) * 5);
  t3 = hcalloc(sizeof(char) * 10);
  t4 = hcalloc(sizeof(char) * 8);
  traverse();

  printf("The address to the beginning of the allocation is %p\n", t1);
  printf("The address to the beginning of the allocation is %p\n", t2);
  printf("The address to the beginning of the allocation is %p\n", t3);
  printf("The address to the beginning of the allocation is %p\n", t4);

  printf("Freeing CALLOC\n");
  printf("After hfree\n");
  hfree(t1);
  traverse();
  hfree(t2);
  traverse();
  hfree(t3);
  traverse();
  hfree(t4);
  traverse();
  printf("\n");
  traverse();

  return 0;
}
