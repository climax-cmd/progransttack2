#include <stdio.h>
#include <stdlib.h>

#define N 10

typedef struct node {
    int n; // 置いたQの数
    int board[N]; // 盤面
    struct node *next;
} node;

node *new_node(void);
node *copy_node(node *nd);
void push(node **sp, node *nd);
node* pop(node **sp);
int check_collision(node *nd, int c);
int check(node *nd);

int main(void) {
    int i, j, count = 0, nsols = 0;
    node *next;
    node *stack = NULL;
    node *nd = new_node();
    nd->n = 0;
    push(&stack, nd); // 何も置いていない盤面をスタックにpush

    while (stack != NULL) {
        nd = pop(&stack);
        if (check(nd)) {
            if (nd->n == N) { // N個Qを置いたので解を表示
                printf("\nNo. %d:\n", ++nsols);
                for (i = 0; i < N; i++, printf("\n"))
                    for (j = 0; j < N; j++)
                        printf(" %c", i == nd->board[j] ? 'Q' : '-');
            } else { // nd->n列の各行にQを配置した盤面をスタックにpush
                for (i = 0; i < N; i++) {
                    next = copy_node(nd);
                    next->n++;
                    next->board[nd->n] = i; // (a)
                    push(&stack, next);
                }
            }
        }
        free(nd);
        count++;
    }

    printf("\n# solutions: %d\n", nsols);
    printf("# iterations: %d\n", count);
    return 0;
}

node *new_node(void) {
    node *obj;
    obj = (node*)malloc(sizeof(node));
    if (obj == NULL) exit(1);
    return obj;
}

node *copy_node(node *nd) {
    int i;
    node *obj = new_node();
    obj->n = nd->n;
    obj->next = nd->next;
    for (i = 0; i < nd->n; i++)
        obj->board[i] = nd->board[i];
    return obj;
}

void push(node **sp, node *nd) {
    nd->next = *sp;
    *sp = nd;
}

node* pop(node **sp) {
    node *nd;
    nd = *sp;
    if (nd == NULL) exit(1);
    *sp = nd->next; // (b)
    return nd;
}

int check_collision(node *nd, int c) { // Qが取られていない：返り値1
    for (int i = 0; i < nd->n; i++) {
        if ((i > c )&&(nd->board[i] == nd->board[c] || abs(nd->board[i] - nd->board[c]) == i - c))
            return 0; // (c)
    }
    return 1;
}

int check(node *nd) { // Qが取られていない：返り値1
    int i;
    for (i = 0; i < nd->n; i++)
        if (!check_collision(nd, i)) return 0;
    return 1;
}

