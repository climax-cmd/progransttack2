#include <stdio.h>
#include <stdlib.h>

typedef struct bsnode{
  int key;
  struct bsnode *tl, *tr;
}*bst;

bst new_node(int key, bst tl, bst tr){
  bst nd = (bst)malloc(sizeof(bst));
  if(nd == NULL) exit(1);
  nd->key = key;
  nd->tl = tl;
  nd->tr = tr;
  return nd;
}

void delete_node(bst nd){
  free(nd);
}

void delete_bst(bst t){
  if(t != NULL){
    delete_bst(t->tl);
    delete_bst(t->tr);
    delete_node(t);
  }
}

int bst_find(bst t, int key){
  if(t == NULL)
    return 0;
  else if(key == t->key)
    return 1;
  else if(key < t->key)
    return bst_find(t->tl, key);
  else
    return bst_find(t->tr, key);
}

bst bst_put(bst t, int key){
  if(t == NULL)
    return new_node(key, NULL, NULL);
  else if(key == t->key)
    return t;
  else if(key < t->key){
    t->tl = bst_put(t->tl, key);
    return t;
  }
  else{
    t->tr = bst_put(t->tr, key);
    return t;
  }
}

bst detach_min(bst t, bst *minp){
  if(t->tl == NULL){
    *minp = t;
    return t->tr;
  }
  else{
    t->tl = detach_min(t->tl, minp);
    return t;
  }
}

bst bst_remove(bst t, int key){
  if (t == NULL)
    return NULL;
  else if(key == t->key){
    if(t->tl == NULL){
      bst tr = t->tr;
      delete_node(t);
      return tr;
    }
    else if(t->tr == NULL){
      bst tl = t->tl;
      delete_node(t);
      return tl;
    }
    else{
      bst mn, nt;
      t->tr = detach_min(t->tr, &mn);
      nt = new_node(mn->key, t->tl, t->tr);
      delete_node(mn);
      delete_node(t);
      return nt;
    }
  }
  else if (key < t->key){
    t->tl = bst_remove(t->tl, key);
    return t;
  }
  else{
    t->tr = bst_remove(t->tr, key);
    return t;
  }
}

void show_bst(bst t){
  if(t == NULL)
    printf("*");
  else{
    printf("(");
    show_bst(t->tl);
    printf(" %d ", t->key);
    show_bst(t->tr);
    printf(")");
  }
}

int main(void){
  int ns[] = {4,2,7,1,3,5,6,8};
  
  int ns_len = sizeof(ns) / sizeof(ns[0]);
  int i;

  bst t = NULL;
  for(i=0; i<ns_len; i++)
    t = bst_put(t, ns[i]);

  t = bst_remove(t,4);
  printf("%d\n", t->key);
  delete_bst(t);

  return 0;
}