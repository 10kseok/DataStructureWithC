#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  p->root = p->nil = nil;

  return p;
}

void search_delete(rbtree *t , node_t *node){

  if(node->left != t->nil)
    search_delete(t,node->left);
  if(node->right != t->nil)
    search_delete(t,node->right);

  // printf("node: %d, color: %s, parent %d\n", 
  //       node->key, node->color ? "BLACK" : "RED", node->parent->key);
  free(node);

}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  node_t *node = t->root;
  if(node != t->nil)
    search_delete(t,node);
  
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t , node_t *x){
 node_t *y ;
  y = x->right;
  x->right = y->left;
  if(y->left != t->nil){y->left->parent = x;}
  y->parent = x->parent;
  if(x->parent == t->nil){t->root = y;}
  else if (x == x->parent->left){x->parent->left = y;}
  else{x->parent->right = y;}
  y->left = x;
  x->parent = y;}
void right_rotate(rbtree *t , node_t *x){
 node_t *y ;
  y = x->left;
  x->left = y->right;
  if(y->right != t->nil){y->right->parent = x;}
  y->parent = x->parent;
  if(x->parent == t->nil){t->root = y;}
  else if (x == x->parent->right){x->parent->right = y;}
  else{x->parent->left = y;}
  y->right = x;
  x->parent = y;}

void rbtree_insert_fixup(rbtree *t , node_t *z){
  node_t *y;
  while(z->parent->color == RBTREE_RED){
    if(z->parent == z->parent->parent->left){
      y = z->parent->parent->right;
      if(y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;}
      else {if(z == z->parent->right){
        z = z->parent;
        left_rotate(t,z);}
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);}}
    else{
      y = z->parent->parent->left;
      if(y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;}
      else {if(z == z->parent->left){
        z = z->parent;
        right_rotate(t,z);}
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t,z->parent->parent);}}
  }
  t->root->color = RBTREE_BLACK;  
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z = (node_t *)calloc(1,sizeof(node_t));
  z->key = key;
  node_t *y = t->nil;
  node_t *x = t->root;
  while(x != t->nil)
  {y = x;
  if(z->key < x->key){x = x->left;}
  else{x = x->right;}}
  z->parent = y;
  if(y == t->nil){t->root = z;}
  else if (z->key < y->key){y->left = z;}
  else {y->right = z;}
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  rbtree_insert_fixup(t,z);
  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // printf("Find Key: %d\n", key);
  // TODO: implement find
  node_t *current = t->root;
    while (current != t->nil){
      // printf("cur key: %d\n", current->key);
      if(key == current->key)
        return current;
      else
        current = (key < current->key) ? current->left : current->right;
    }
    return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *finder = t->root;
  while(finder->left != t->nil){finder = finder->left;}
  return finder;
}
node_t *rbtree_min_search(const rbtree *t , node_t *node) {
  // TODO: implement find
  node_t *finder = node;
  while(finder->left != t->nil){finder = finder->left;}
  return finder;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *finder = t->root;
  while(finder->right != t->nil){finder = finder->right;}
  return finder;
}

void rb_transplant(rbtree *t,node_t *u , node_t *v){
  if(u->parent == t->nil){t->root = v;}
  else if(u == u->parent->left){u->parent->left = v;}
  else{u->parent->right = v;}
  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t , node_t *x){
while(x != t->root && x->color == RBTREE_BLACK){
if(x == x->parent->left){
node_t *w = x->parent->right;
if(w->color == RBTREE_RED){
  w->color = RBTREE_BLACK;
  x->parent->color = RBTREE_RED;
  left_rotate(t,x->parent);
  w = x->parent->right;}
if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
  w->color = RBTREE_RED;
  x = x->parent;}
else {if (w->right->color == RBTREE_BLACK){
  w->left->color = RBTREE_BLACK;
  w->color = RBTREE_RED;
  right_rotate(t,w);
  w = x->parent->right;}

  w->color = x->parent->color;
  x->parent->color = RBTREE_BLACK;
  w->right->color = RBTREE_BLACK;
  left_rotate(t,x->parent);
  x = t->root;}}
else{
  node_t *w = x->parent->left;
if(w->color == RBTREE_RED){
  w->color = RBTREE_BLACK;
  x->parent->color = RBTREE_RED;
  right_rotate(t,x->parent);
  w = x->parent->left;}
if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
  w->color = RBTREE_RED;
  x = x->parent;}
else {if (w->left->color == RBTREE_BLACK){
  w->right->color = RBTREE_BLACK;
  w->color = RBTREE_RED;
  left_rotate(t,w);
  w = x->parent->left;}

  w->color = x->parent->color;
  x->parent->color = RBTREE_BLACK;
  w->left->color = RBTREE_BLACK;
  right_rotate(t,x->parent);
  x = t->root;}}}
x -> color = RBTREE_BLACK;}
int rbtree_erase(rbtree *t, node_t *z) {
  // TODO: implement erase
  node_t *y = z;
  node_t *x;
  color_t y_oricol = y->color;
  if(z->left == t->nil){
    x = z->right;
    rb_transplant(t,z,z->right);
  }else if(z->right == t->nil){
    x = z->left;
    rb_transplant(t,z,z->left);
  }else{y = rbtree_min_search(t,z->right);
  y_oricol = y->color;
  x = y->right;
  if(y->parent == z){x->parent = y;}
  else{rb_transplant(t,y,y->right);
  y->right = z->right;
  y->right->parent = y;}
  rb_transplant(t,z,y);
  y->left = z->left;
  y->left->parent = y;
  y->color = z->color;}
if(y_oricol == RBTREE_BLACK){
  rbtree_delete_fixup(t,x);} 
free(z);
return 0;}

void inorder(const rbtree *t, node_t *node, key_t *arr, const size_t n, size_t *cnt)
{
  if (*cnt == n || node == t->nil)
    return;
  inorder(t, node->left, arr, n, cnt);
  arr[*cnt] = node->key;
  *cnt += 1;
  inorder(t, node->right, arr, n, cnt);
}
// 트리를 중위 순회하며 n개의 키를 배열 arr에 저장
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  size_t cnt = 0;
  size_t *pcnt = &cnt;

  inorder(t, t->root, arr, n, pcnt);

  return 0;
}


