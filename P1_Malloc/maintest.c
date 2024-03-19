#include "hmalloc.h"
#include <stdio.h>
#include <stdlib.h>

int main(){

    int y = 0;
    int z = 4;
    int x = 5;

    while(y < z){
        int temp = 0;
        
        void *ptr = hcalloc(x * sizeof(int));
        int *test = (int*) ptr;


        temp = 0;

        while(temp < x)
        {
            printf("The value of ptrr at %d is %d\n", temp, *((int*) (ptr + (temp * 4))));
            temp++;
        }
        hfree(ptr);
        printf("Finished H(free)\n");
        y++;
        x--;
    }

    traverse();
    return 1;
}