#include "../trie.h"
#include <stdio.h>

int main() {
    int i;
    Trie t;
    const char *f;
    const char *strings[4] = {
        "yarin",
        "hello",
        "hellow",
        "hellno"
    };
    t = trie();
    for(i=0;i<4;i++) {
        insert_to_trie(t, strings[i], (char*)strings[i]);
    }
    for(i=0;i<4;i++) {
        f = exist_in_trie(t, strings[i]);
        if(f != NULL) {
            printf("%s\n",f);
        }
        else {
            printf("string:'%s' could not be found.\n",strings[i]);
        }
    }
    delete_from_trie(t, strings[1]);
    for(i=0;i<4;i++) {
        f = exist_in_trie(t, strings[i]);
        if(f != NULL) {
            printf("%s\n",f);
        }
        else {
            printf("string:'%s' could not be found.\n",strings[i]);
        }
    }
    destroy_trie(&t);
    return 0;
}