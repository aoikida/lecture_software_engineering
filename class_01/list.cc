#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>


struct list {
    struct list *next;
    int data;
}; 

struct list *newnode(int x){
    struct list * node;

    node = (struct list *)malloc(sizeof(struct list));
    node->data = x;
    node->next = NULL;
    return node;
}

void print_list(struct list *node){

    while (node != NULL){
        printf("%d ", node->data);
        node = node->next;
    }
    printf("\n");

}

int main(){

    struct list *top = NULL, *node;

    for(int i = 100; i > 0; i--){
        node = newnode(i);
        node->next = top;
        top = node;
    }
    
    print_list(top);
    exit(0);
}