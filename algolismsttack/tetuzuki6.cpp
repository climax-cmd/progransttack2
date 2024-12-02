#include<stdio.h>
int x[2] = {1,2};
void f(int** p){
    *p = x;
}
int main(void){
    int* p;
    f(&p);
    printf("%d\n",p[0]+p[1]);
    return 0;
}