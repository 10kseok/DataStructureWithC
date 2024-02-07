#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  // TODO: initialize struct if needed
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));       //동적 할당, tree와 노드 포인터로 포인팅
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));  

  nil_node->color = RBTREE_BLACK; //초기화
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
  while (z->parent->color == RBTREE_RED){ //부모의 노드 색이 빨강
    if (z->parent == z->parent->parent->left){  // 부모가 조부모의 좌측일때
      node_t *y = z->parent->parent->right; // 삼촌 pointing
      if (y->color == RBTREE_RED){ // 삼촌 빨강 -> recoloring
        y->color = RBTREE_BLACK;
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }else{ //삼촌 검정 ->회전
        if (z->parent->right == z){
          z = z->parent;
          left_rotate(t,z); 
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);
      }
    }else{//부모가 조부모의 우측
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
  while (x!= t->nil){ //x가 leaf 아니면
    y = x;
    if (key < x->key){
      x = x->left;
    }else{
      x = x->right;
    }
  }
  node_t *z = (node_t *)calloc(1,sizeof(node_t)); // 삽입할 노드 point
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
  rbtree_insert_fixup(t,z);// 삽입하면서 rb규칙 확립
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

node_t *rbtree_min(const rbtree *t) //트리 전체에서 제일 작은 값
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
  return temp;// 노드가 nil일때
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

void rbtree_transplant(rbtree *t, node_t *u, node_t *v){ //삭제하고자 하는 노드를 대체할 노드 찾아 노드에 대입하는 함수(u를 v로)
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
void rbtree_delete_fixup(rbtree *t, node_t *x){ // 삭제하고 나서 rb트리 규칙 위반 수정(4가지 case)

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
node_t *rbtree_minimum(rbtree *t, node_t *n){ // 특정 노드에서 부터 최솟값 찾기
  node_t *p = n;
  while (p->left != t->nil){ //leaf아닐때
    p = p->left; //좌측으로 이동
  }
  return p; //해당 노드 반환
}
int rbtree_erase(rbtree *t, node_t *p) { //삭제해야 할 노드 삭제
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

void inorder(const rbtree *t, node_t *node, key_t *arr, int *idx, const size_t n) //inorder 트리구조에 적합
{
  if (node == t->nil)
  {
    return;
  }
  inorder(t, node->left, arr, idx, n);
  if (*idx < n)
  {
    arr[(*idx)++] = node->key; //포인터로 설정하여 가장 작은 범위의 재귀 끝나고 나서 idx증가
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

//avl 빡신 균형 , rb느슨한 균형