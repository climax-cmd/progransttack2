#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 10000
#define STRLEN 20
typedef struct node{
    int key;
    char data[STRLEN];
    struct node *parent;
    struct node *left;
    struct node *right;
} node;
node* insert_tree(node *root, int key, char string[]);
node* delete_tree(node *root, node *ptr);
node* search_tree(node *ptr, int key);
void print_tree(node *ptr);
void print_node(node *p);
node *search_tree(node *ptr, int key){
    if(ptr == NULL){
        return NULL;
    }
    if(key < ptr->key){
    return search_tree(ptr->left, key);
    }else if(key == ptr->key){
return ptr;
    }else // key > ptr->key
return search_tree(ptr->right, key);
}

void print_tree(node *ptr){
 if(ptr != NULL){
    printf("(");
    print_tree(ptr->left);
    printf(" / ");
    print_node(ptr);
    printf(" / ");
    print_tree(ptr->right);
    printf(")");
 }
}
void print_node(node *ptr){
printf("[%d,%s]", ptr->key, ptr->data);
}
node *insert_tree(node *root, int key, char string[]){
node *new2, *x, *y;
// allocation
if((new2 = (node*)calloc(1, sizeof(node))) == NULL){
fprintf(stderr, "calloc error.¥n");
exit(1);
}
new2->key = key;
if(strlen(string) < STRLEN)
strcpy(new2->data, string);
else{
fprintf(stderr, "string too long: %s¥n", string);
exit(1);
}
y = NULL; x = root;
while(x != NULL){
y = x;
if(key < x->key) x = x->left;
else x = x->right;
}
new2->parent = y;
new2->left = new2->right = NULL;
if(y == NULL) return new2;
else{
if(key < y->key) y->left = new2;
else y->right = new2;
return root;
}
}
int myheight(node* root);
int mymax(int x, int y);
int mymin(int x, int y);
int main(void){
node *root = NULL;
root = insert_tree(root, 21, "KAWASHIMA");
print_tree(root); printf("\n\n");
root = insert_tree(root, 3, "KOMANO");
print_tree(root); printf("\n\n");
root = insert_tree(root, 22, "NAKAZAWA");
print_tree(root); printf("\n\n");
root = insert_tree(root, 4, "TULIO");
print_tree(root); printf("\n\n");
root = insert_tree(root, 5, "NAGATOMO");
print_tree(root); printf("\n\n");
root = insert_tree(root, 2, "ABE");
print_tree(root); printf("\n\n");
root = insert_tree(root, 8, "MATSUI");
print_tree(root); printf("\n\n");
root = insert_tree(root, 17, "HASEBE");
print_tree(root); printf("\n\n");
root = insert_tree(root, 7, "ENDO");
print_tree(root); printf("\n\n");
root = insert_tree(root, 16, "OKUBO");
print_tree(root); printf("\n\n");
root = insert_tree(root, 18, "HONDA");
print_tree(root); printf("\n\n");
printf("height = %d\n",myheight(root));
print_node(search_tree(root, 18));
printf("\n\n");
print_node(search_tree(root, 7));
printf("\n\n");
return 0;
}
int myheight(node* root){
    if(root == NULL){
        return 0;
    }else{
        return(mymax(myheight(root->left)+1,myheight(root->right)+1));
    }
}
int mymax(int x, int y){
    if(x>y){
        return x;
    }else{
        return y;
    }
}
int mymin(int x,int y){
    if(x>y){
        return y;
    }else{
        return x;
    }
}