#include <stdio.h>
#include <stdlib.h>
#include "vector.h"



void * num_ctor(const void *num) {
    int * new_num = malloc(sizeof(int));
    if(new_num == NULL)
        return NULL;
    *new_num = *((const int *)num);
    return new_num;
}
void num_dtor(void * num) {
    free(num);
}

int main() {
    int i;
    void *const * begin;
    void *const * end;
    Vector nums = new_vector(num_ctor, num_dtor);
    for(i=1;i<=10;i++) {
        vector_insert(nums,&i);
    }
    
    
    VECTOR_FOR_EACH(begin, end, nums) {
        if(*begin != NULL) {
            printf("%d ",**(int * const *)begin);
        }
    }
    printf("\n");
    VECTOR_FOR_EACH_R(begin, end, nums) {
        if(*end != NULL) {
            printf("%d ",**(int * const *)end);
        }
    }
    printf("\n");
    vector_destroy(&nums);
    return 0;
}