#include <stdio.h>
#include <assert.h>
#include<stdlib.h>
#include<string.h>
typedef struct list {
    struct node *head;  // 一番目の要素
}list;

typedef struct node {
    int value;  // この要素の値
    struct node *next;  // 次の要素、最後の場合はNULL
}node;
struct list *list_init(void){
    list* p = NULL;
    return p;
}
void list_free(list *this_list){
    this_list = NULL;
    free(this_list);
}
int list_length(list *this_list){
    int ans = 1;
    if(this_list == NULL){
        return 0;
    }else{
        while((this_list->head)->next != NULL){
            this_list->head = (this_list->head)->next;
            ans += 1;
        }
        return ans;
    }
}
int list_append(list *this_list, int value){
    node* newnumber;
    newnumber =(node*)calloc(1,sizeof(node));
    newnumber->next = NULL;
    newnumber->value = value;
    if(this_list == NULL){
        this_list->head = newnumber;
        return 0;
    }
    while(this_list->next != NULL){
        this_list->head = (this_list->head)->next;
        }
    (this_list->head)->next = newnumber->head;
    return(list_length(this_list) - 1);     
}
int main(void) {
    struct list *my_list = list_init();

    assert(list_append(my_list, 1) == 0);
    assert(list_append(my_list, 2) == 1);
    assert(list_append(my_list, 3) == 2);

    printf("List length is %d\n", list_length(my_list));

    list_free(my_list);

    return 0;
}