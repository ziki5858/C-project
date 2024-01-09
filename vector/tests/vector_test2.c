#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include <string.h>
struct person {
    int id;
    int height;
    char name[30];
};
void * person_ctor (const void * copy) {
    struct person * new_p = malloc(sizeof(struct person));
    if(new_p == NULL)
        return NULL;
    return memcpy(new_p,copy,sizeof(struct person));
}
void person_dtor(void * item) {
    free(item);
}
int main() {
    struct person p_i;
    void * const * begin;
    void * const * end;
    struct person * p; 
    Vector persons = new_vector(person_ctor,person_dtor);

    while(scanf("%d %d %s",&p_i.id,&p_i.height,p_i.name) == 3) {
        vector_insert(persons,&p_i);
    }
    VECTOR_FOR_EACH(begin, end, persons) {
        if(*begin) {
            p = *begin;
            printf("%d %d %s",p->id,p->height,p->name);
            printf("\n");
        }
    }
    vector_destroy(&persons);
    return 0;
}