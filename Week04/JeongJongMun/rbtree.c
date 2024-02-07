#include "rbtree.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// 헤더 파일에 없는 내부 함수들과 구조체를 정의
typedef enum
{
  LEFT,
  RIGHT
} dir_t;
void rotate(rbtree *t, node_t *x, dir_t dir);
void exchange_color(node_t *a, node_t *b);
void recoloring(node_t *a, node_t *b, node_t *c);
void rbtree_insert_fixup(rbtree *t, node_t *x);
void rbtree_delete_fixup(rbtree *t, node_t *x);

rbtree *new_rbtree(void)
{
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  // 트리 혹은 nil 노드가 할당되지 않은 경우 (메모리가 부족한 경우) 정상적으로 실행되지 않도록 처리
  if (t == NULL || nil == NULL)
  {
    free(t);
    free(nil);
    return NULL;
  }

  t->root = t->nil = nil;    // 트리의 nill과 루트를 nil 노드로 설정
  nil->color = RBTREE_BLACK; // nil 노드는 항상 검은색

  return t;
}

// 후위 순회 방식으로 RB 트리의 노드들의 메모리를 해제
void free_node(rbtree *t, node_t *node)
{
  if (node == t->nil)
    return;
  free_node(t, node->left);
  free_node(t, node->right);
  free(node);
  node = NULL;
}

void delete_rbtree(rbtree *t)
{
  free_node(t, t->root);
  free(t->nil);
  free(t);
}

// 회전 시 아래로 내려가는 노드를 x, 위로 올라가는 노드를 y, 회전 시에 y에서 x로 이동하는 노드를 beta라고 가정
// 좌회전, 우회전 모두 x를 기준으로 원하는 방향으로 회전
/*
  [Left Rotation]
  x               y
   \             / \
    y     =>    x   c
   / \           \
  b   c           b

  [Right Rotation]
      x           y
     /           / \
    y     =>    a   x
   / \             /
  a   b           b
*/
void rotate(rbtree *t, node_t *x, dir_t dir)
{
  node_t *y = dir ? x->left : x->right;
  node_t *beta = dir ? y->right : y->left;

  // 1-1) y의 부모를 x의 부모로 변경
  y->parent = x->parent;
  // 1-2-1) x의 부모가 루트인 경우: y가 새로운 루트
  if (x == t->root)
    t->root = y;
  // 1-2-2) x가 부모의 왼쪽 자식인 경우: y가 x의 부모의 왼쪽 자식
  else if (x == x->parent->left)
    x->parent->left = y;
  // 1-2-3) x가 부모의 오른쪽 자식인 경우: y가 x의 부모의 오른쪽 자식
  else
    x->parent->right = y;

  // 2) x의 자식을 y의 자식으로 변경 (양방향 연결)
  dir ? (x->left = beta) : (x->right = beta);
  if (beta != t->nil)
    beta->parent = x;

  // 3) y의 자식을 x로 변경 (양방향 연결)
  dir ? (y->right = x) : (y->left = x);
  x->parent = y;
}

// a와 b의 색을 교환
void exchange_color(node_t *a, node_t *b)
{
  color_t tmp_color = a->color;
  a->color = b->color;
  b->color = tmp_color;
}
// b와 c의 색을 a로 변경하고 a의 색을 b로 변경
void recoloring(node_t *a, node_t *b, node_t *c)
{
  color_t tmp = a->color;
  a->color = b->color;
  b->color = c->color = tmp;
}

void rbtree_insert_fixup(rbtree *t, node_t *x)
{
  node_t *uncle;
  while (x->parent->color == RBTREE_RED)
  {
    // x의 부모가 왼쪽 자식인 경우
    if (x->parent == x->parent->parent->left)
    {
      uncle = x->parent->parent->right;
      // [CASE #1]: x의 삼촌이 적색인 경우
      if (uncle->color == RBTREE_RED)
      {
        recoloring(x->parent->parent, x->parent, uncle);
        x = x->parent->parent;
      }
      else
      {
        // [CASE #2]: x의 삼촌이 흑색이며, x가 오른쪽 자식인 경우
        if (x == x->parent->right)
        {
          x = x->parent;
          rotate(t, x, LEFT);
        }
        // [CASE #3]: x의 삼촌이 흑색이며, x가 왼쪽 자식인 경우
        exchange_color(x->parent->parent, x->parent);
        rotate(t, x->parent->parent, RIGHT);
      }
    }
    // x의 부모가 오른쪽 자식인 경우
    else
    {
      uncle = x->parent->parent->left;
      if (uncle->color == RBTREE_RED)
      {
        recoloring(x->parent->parent, x->parent, uncle);
        x = x->parent->parent;
      }
      else
      {
        if (x == x->parent->left)
        {
          x = x->parent;
          rotate(t, x, RIGHT);
        }
        exchange_color(x->parent->parent, x->parent);
        rotate(t, x->parent->parent, LEFT);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

// 의사 코드 기반 노드 삽입 구현
node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *parent = t->nil;
  node_t *current = t->root;

  // 새 노드를 삽입할 위치 탐색
  while (current != t->nil)
  {
    parent = current;
    if (key < current->key)
      current = current->left;
    else
      current = current->right;
  }

  // 새 노드 생성
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  *new_node = (node_t){RBTREE_RED, key, parent, t->nil, t->nil};

  if (parent == t->nil)
    t->root = new_node; // 트리가 비어있으면 새 노드를 트리의 루트로 지정
  else if (new_node->key < parent->key)
    parent->left = new_node; // 새 노드를 왼쪽 자식으로 추가
  else
    parent->right = new_node; // 새 노드를 오른쪽 자식으로 추가

  // 불균형 복구
  rbtree_insert_fixup(t, new_node);

  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *current = t->root;
  while (current != t->nil)
  {
    if (current->key == key)
      return current;
    if (current->key < key)
      current = current->right;
    else
      current = current->left;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t)
{
  node_t *current = t->root;
  while (current->left != t->nil)
    current = current->left;
  return current;
}

node_t *rbtree_max(const rbtree *t)
{
  node_t *current = t->root;
  while (current->right != t->nil)
    current = current->right;
  return current;
}

// 노드 u를 노드 v로 대체
void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *x)
{
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    // CASE 1 ~ 4 : LEFT CASE
    if (x == x->parent->left)
    {
      node_t *w = x->parent->right;

      // CASE 1 : x의 형제 w가 적색인 경우
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate(t, x->parent, LEFT);
        w = x->parent->right;
      }

      // CASE 2 : x의 형제 w는 흑색이고 w의 두 지식이 모두 흑색인 경우
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }

      // CASE 3 : x의 형제 w는 흑색, w의 왼쪽 자식은 적색, w의 오른쪽 자신은 흑색인 경우
      else
      {
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rotate(t, w, RIGHT);
          w = x->parent->right;
        }

        // CASE 4 : x의 형제 w는 흑색이고 w의 오른쪽 자식은 적색인 경우
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        rotate(t, x->parent, LEFT);
        x = t->root;
      }
    }
    // CASE 5 ~ 8 : RIGHT CASE
    else
    {
      node_t *w = x->parent->left;

      // CASE 5 : x의 형제 w가 적색인 경우
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate(t, x->parent, RIGHT);
        w = x->parent->left;
      }

      // CASE 6 : x의 형제 w는 흑색이고 w의 두 지식이 모두 흑색인 경우
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }

      // CASE 7 : x의 형제 w는 흑색, w의 왼쪽 자식은 적색, w의 오른쪽 자신은 흑색인 경우
      else
      {
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rotate(t, w, LEFT);
          w = x->parent->left;
        }

        // CASE 8 : x의 형제 w는 흑색이고 w의 오른쪽 자식은 적색인 경우
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        rotate(t, x->parent, RIGHT);
        x = t->root;
      }
    }
  }

  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *target)
{
  node_t *y = target;
  node_t *x;
  color_t yOriginalColor = y->color;

  if (target->left == t->nil)
  {
    x = target->right;
    rb_transplant(t, target, target->right);
  }
  else if (target->right == t->nil)
  {
    x = target->left;
    rb_transplant(t, target, target->left);
  }
  else
  {
    y = target->right;
    while (y->left != t->nil)
    {
      y = y->left;
    }
    yOriginalColor = y->color;
    x = y->right;

    if (y->parent == target)
    {
      x->parent = y;
    }
    else
    {
      rb_transplant(t, y, y->right);
      y->right = target->right;
      y->right->parent = y;
    }

    rb_transplant(t, target, y);
    y->left = target->left;
    y->left->parent = y;
    y->color = target->color;
  }

  if (yOriginalColor == RBTREE_BLACK)
  {
    rbtree_delete_fixup(t, x);
  }

  free(target);

  return 0;
}

// 중위 순회하며 배열에 키를 저장하는 함수
void inorder(const rbtree *t, node_t *node, key_t *arr, const size_t n, int *cnt)
{
  if (*cnt == n || node == t->nil)
    return;
  inorder(t, node->left, arr, n, cnt);
  arr[(*cnt)++] = node->key;
  inorder(t, node->right, arr, n, cnt);
}
// 트리에서 n개의 키를 배열 arr에 오름차순으로 저장하는 함수
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  int cnt = 0;
  inorder(t, t->root, arr, n, &cnt);

  return 0;
}
