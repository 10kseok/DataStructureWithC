#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  // TODO: initialize struct if needed
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  nil_node->color = RBTREE_BLACK;
  p->root = nil_node;
  p->nil = nil_node;
  return p;
}

void delete_node(node_t *n, rbtree *t){ //왼쪽과 오른쪽 자식 노드들 메모리 해제
  if (n == t->nil){
    return;
  }

  delete_node(n->left,t); // 왼쪽 자식 노드들 재귀를 돌리면서 해제
  delete_node(n->right,t); // 오른쪽 자식 노드들 재귀를 돌리면서 해제
  free(n); // 해당 노드 메모리 해제
  n = NULL;
  return;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  if (t == NULL){
    return;
  }

  delete_node(t->root,t); // 생성된 노드들 모두 메모리 해제를 위해 delete_node함수 호출
  free(t->nil); // nil노드도 해제
  t->nil = NULL; 
  free(t); // 트리를 가르키는 곳도 해제
  t = NULL;
  return;
}

void left_rotate(rbtree *t, node_t *x){ // 왼쪽으로 회전하는 함수. (회전을 할 때는 부분 노드만 떼서 그 부분만 회전함.)
  node_t *y = x->right;
  x->right = y->left;
  if (y->left != t->nil){
    y->left->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == t->nil){
    t->root = y;
  } else if (x == x->parent->left){
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
  return;
}


void right_rotate(rbtree *t, node_t *x){ // 오른쪽으로 회전하는 함수. (회전을 할 때는 부분 노드만 떼서 그 부분만 회전함.)
  node_t *y = x->left;
  x->left = y->right;
  if (y->right != t->nil){
    y->right->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == t->nil){
    t->root = y;
  } else if (x == x->parent->left){
    x->parent->left = y;
  } else{
    x->parent->right = y;
  }
  y->right  = x;
  x->parent = y;
  return;
}

void rbtree_insert_fixup(rbtree *t, node_t *z){
  while (z->parent->color == RBTREE_RED){
    if (z->parent == z->parent->parent->left){
      node_t *y = z->parent->parent->right;
      if (y->color == RBTREE_RED){
        y->color = RBTREE_BLACK;
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }else{
        if (z->parent->right == z){
          z = z->parent;
          left_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);
      }
    }else{
      node_t *y = z->parent->parent->left;
      if (y->color == RBTREE_RED){
        z->parent->parent->color = RBTREE_RED;
        y->color = RBTREE_BLACK;
        z->parent->color = RBTREE_BLACK;
        z = z->parent->parent;
      }else{
        if (z->parent->left == z){
          z = z->parent;
          right_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t,z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
  return;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *y = t->nil;
  node_t *x = t->root;
  while (x!= t->nil){
    y = x;
    if (key < x->key){
      x = x->left;
    }else{
      x = x->right;
    }
  }
  node_t *z = (node_t *)calloc(1,sizeof(node_t));
  z->parent = y;
  if (y == t->nil){
    t->root = z;
  }else if (key < y->key){
    y->left = z;
  }else{
    y->right = z;
  }
  z->key = key;
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  rbtree_insert_fixup(t,z);
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *x = t->root;

  while (x != t->nil && key != x->key){ // root 노드로부터 내려가면서 nil 노드로 가거나 key값이 없다면 find할 값이 없다고 판단. 
    if (x->key < key){
      x = x->right;
    }else{
      x = x->left;
    }
  }
  if (x == t->nil){
    return NULL;
  }
  return x;
}

node_t *rbtree_min(const rbtree *t)
{
  // TODO: implement find
  node_t *temp = t->root;
  while (temp != t->nil)
  {
    if (temp->left == t->nil)
    { // nil일때 해당 노드가 가장 작음
      return temp;
    }
    temp = temp->left; // 좌측 노드 point
  }
  return temp;
}
node_t *rbtree_max(const rbtree *t)
{
  // TODO: implement find
  node_t *temp = t->root;
  while (temp != t->nil)
  {
    if (temp->right == t->nil)
    { // nil일때 해당 노드가 가장 작음
      return temp;
    }
    temp = temp->right; // 좌측 노드 point
  }
  return temp;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v){
  if (u->parent == t->nil){
    t->root = v;
  }else if(u == u->parent->left){
    u->parent->left = v;
  }else{
    u->parent->right = v;
  }
  v->parent = u->parent;
  return;
}
void rbtree_delete_fixup(rbtree *t, node_t *x){
  node_t *w;
  while ((x != t->root) && (x->color == RBTREE_BLACK)){
    if (x == x->parent->left){
      w = x->parent->right;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t,x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else {
        if(w->right->color == RBTREE_BLACK){
          w->color = RBTREE_RED;
          w->left->color = RBTREE_BLACK;
          right_rotate(t,w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t,x->parent);
        x = t->root;
      }
    }else{
      w = x->parent->left;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t,x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else {
          if(w->left->color == RBTREE_BLACK){
            w->right->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            left_rotate(t,w);
            w = x->parent->left;
          }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t,x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
  return;
}
node_t *rbtree_minimum(rbtree *t, node_t *n){
  node_t *p = n;
  while (p->left != t->nil){
    p = p->left;
  }
  return p;
}
int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t *y = p;
  node_t *x;
  color_t y_original_color = y->color;

  if (p->left == t->nil){
    x = p->right;
    rbtree_transplant(t,p,p->right);
  }else if (p->right == t->nil){
    x = p->left;
    rbtree_transplant(t,p,p->left);
  }else{
    y = rbtree_minimum(t,p->right);
    y_original_color = y->color;
    x = y->right;
    if (y->parent == p){
      x->parent = y;
    }else{
      rbtree_transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rbtree_transplant(t,p,y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  free(p);
  p = NULL;
  if (y_original_color == RBTREE_BLACK){
    rbtree_delete_fixup(t,x);
  }
  return 0;
}

void inorder(const rbtree *t, node_t *node, key_t *arr, int *idx, const size_t n)
{
  if (node == t->nil)
  {
    return;
  }
  inorder(t, node->left, arr, idx, n);
  if (*idx < n)
  {
    arr[(*idx)++] = node->key;
  }
  else
  {
    return;
  }
  inorder(t, node->right, arr, idx, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  // TODO: implement to_array
  node_t *temp = t->root;
  int i = 0;
  int *idx = &i;
  inorder(t, temp, arr, idx, n);
  return 0;
}