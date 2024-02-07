#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

void post_order_free(rbtree *tree, node_t *node);
void rbtree_insert_fixup(rbtree *t, node_t *node);
void left_rotate(rbtree *t, node_t *node);
void right_rotate(rbtree *t, node_t *node);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
void rbtree_erase_fixup(rbtree *t, node_t *current);

rbtree *new_rbtree(void)
{
  rbtree *tree = (rbtree *)calloc(1, sizeof(rbtree));
  if (tree == NULL)
  {
    return NULL;
  }

  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  if (nil_node == NULL)
  {
    free(tree); //이전에 할당한 tree 해제하고 null 반환
    return NULL;
  }

  nil_node->color = RBTREE_BLACK;
  nil_node->parent = nil_node->left = nil_node->right = nil_node;
  nil_node->key = 0;

  tree->root = nil_node;
  tree->nil = nil_node;

  return tree;
}

void delete_rbtree(rbtree *tree)
{
  if (tree == NULL)
    return;

  post_order_free(tree, tree->root);
  free(tree->nil);
  free(tree);
}

//후위순회하여 노드 삭제
void post_order_free(rbtree *tree, node_t *node)
{
  if (node == NULL || node == tree->nil)
    return;

  //모든 노드 삭제
  post_order_free(tree, node->left);
  post_order_free(tree, node->right);
  //현재 노드 삭제
  free(node);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));

  node_t *y = t->nil;  //현재 노드의 부모 노드
  node_t *current = t->root;
  new_node->key = key;

  // 새 노드를 삽입할 위치 탐색
  // current가 nil을 가리키기 전까지 이진탐색 반복
  while (current != t->nil)
  { 
    y = current; 
    if (key < current->key)
    {
      current = current->left;  
    }
    else
      current = current->right;
  }

  new_node->parent = y; //새로운 노드의 부모를 찾은 부모 노드로 설정

  if (y == t->nil)  //트리가 비어있다는 의미
  {
    t->root = new_node;
  }
  else if (key < y->key)
  {
    y->left = new_node;
  }
  else
  {
    y->right = new_node;
  }

  new_node->left = new_node->right = t->nil;
  new_node->color = RBTREE_RED;

  rbtree_insert_fixup(t, new_node);

  return new_node;
}

//삽입 불균형 복구
void rbtree_insert_fixup(rbtree *t, node_t *node)
{
  node_t *y = NULL;

  //부모가 빨간색인 경우
  while (node->parent->color == RBTREE_RED)
  {
    if (node->parent == node->parent->parent->left)
    {
      y = node->parent->parent->right; //삼촌

      //case1 - 부모와 부모의 형제(삼촌)가 모두 red인 경우
      if (y->color == RBTREE_RED)
      {
        node->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
        continue;
      }
      // case2 - 부모의 형제가 black & 부모 왼쪽 & 현재 노드 왼쪽 자식인 경우
      else if (node == node->parent->right)
      {
        node = node->parent;
        left_rotate(t, node); 
      }
      // case3 - 부모의 형제 black & 부모 오른쪽 & 현재 노드 오른쪽 자식
      node->parent->color = RBTREE_BLACK;
      node->parent->parent->color = RBTREE_RED;
      right_rotate(t, node->parent->parent);
    }
    else
    {
      //case1 - 부모와 부모의 형제(삼촌)가 모두 red인 경우
      y = node->parent->parent->left;
      if (y->color == RBTREE_RED)
      {
        node->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
        continue;
      }
      // case2 - 부모의 형제가 black & 부모 왼쪽 & 현재 노드 왼쪽 자식인 경우
      else if (node == node->parent->left)
      {
        node = node->parent;
        right_rotate(t, node);
      }
      // case3 - 부모의 형제 black & 부모 오른쪽 & 현재 노드 오른쪽 자식
      node->parent->color = RBTREE_BLACK;
      node->parent->parent->color = RBTREE_RED;
      left_rotate(t, node->parent->parent);
    }
  }
  t->root->color = RBTREE_BLACK;
}

//왼쪽으로 회전
void left_rotate(rbtree *t, node_t *node)
{
  //node의 오른쪽 자식 노드 y선언
  node_t *y = node->right;
  //y의 왼쪽 자식 노드를 node의 오른쪽 자식 노드로 변경
  node->right = y->left;

  //왼쪽 자식이 nil이 아니라면, 그 자식의 부모를 node로 설정
  if (y->left != t->nil)
    y->left->parent = node;

  //y의 부모 노드를 x의 부모 노드로 변경
  y->parent = node->parent;

  if (node->parent == t->nil) //node의 부모가 nil이라면 트리의 루트 노드를 y로 변경
    t->root = y;
  else if (node == node->parent->left)  //node가 왼쪽 자식이라면, 부모의 왼쪽 자식을 y로 변경
    node->parent->left = y;
  else
    node->parent->right = y;  //node가 오른쪽 자식이라면, 부모의 오른쪽 자식을 y로 변경

  y->left = node; //y의 왼쪽 자식 노드를 node로 변경
  node->parent = y;   //node의 부모를 y로 변경
}

//오른쪽으로 회전
void right_rotate(rbtree *t, node_t *node)
{
  node_t *y = node->left;
  node->left = y->right;

  if (y->right != t->nil)
    y->right->parent = node;

  y->parent = node->parent;

  if (node->parent == t->nil)
    t->root = y;
  else if (node == node->parent->right)
    node->parent->right = y;
  else
    node->parent->left = y;

  y->right = node;
  node->parent = y;
}

//key에 해당하는 노드를 반환하는 함수
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *current = t->root;

  while (current != t->nil && key != current->key)
  {
    if (key < current->key)
      current = current->left;
    else
      current = current->right;
  }
  return (current == t->nil) ? NULL : current; 
  // current가 nil이면 null, 아니면 current 반환
}

//최소값에 해당하는 노드를 반환하는 함수
node_t *rbtree_min(const rbtree *t)
{
  // TODO: implement find
  node_t *current = t->root;
  while (current->left != t->nil)
  {
    current = current->left;
  }
  return current;
}

//최댓값에 해당하는 노드를 반환하는 함수
node_t *rbtree_max(const rbtree *t)
{
  // TODO: implement find
  node_t *current = t->root;
  while (current->right != t->nil)
  {
    current = current->right;
  }

  return current;
}

node_t *rbtree_min_succ(rbtree *t, node_t *y)
{
  node_t *r = y;
  if (r == t->nil) // 새로 추가
    return r;
  while (r->left != t->nil) // y의 왼쪽 자식이 nil이 아닐 때까지 계속 파고들어간다.
  {
    r = r->left;
  }

  // y의 왼쪽 자식이 nil이라면 멈추기 때문에(y가 nil이면 멈추는게 아니라) y는 유의미한 값을 가진 노드를 가르키는 주소값이다. 여기서는 successor라고 보면 된다.
  return r;
}

/* 노드를 삭제하는 함수 */
int rbtree_erase(rbtree *t, node_t *erase)
{
  node_t *y = erase;                       // 트리에서 없어질 노드
  color_t y_try_to_erase_color = y->color; // 삭제할 노드의 원래 색상
  node_t *e_child = NULL;                  // 트리 재조정의 기준점이 될 노드

  if (erase->left == t->nil)  //erase의 왼쪽 자식이 nil이면
  {
    e_child = erase->right;   
    rbtree_transplant(t, erase, erase->right);  //erase를 erase의 오른쪽 자식 노드로 대체
  }
  else if (erase->right == t->nil)
  {
    e_child = erase->left;
    rbtree_transplant(t, erase, erase->left);
  }
  else // 노드의 자식이 2명
  {
    y = rbtree_min_succ(t, erase->right); // 삭제될노드의 계승자를 찾는다
    y_try_to_erase_color = y->color;      // 삭제할 노드의 색을 저장한다
    e_child = y->right;                   // 대체할노드에 삭제할 노드의 오른자식 할당 (succesor 찾음)
    if (y->parent == erase)               // successor의 부모가 바로 지워야 할 위치일때
    {
      e_child->parent = y; // erase자리를 erase자식의 부모로 바로할당
    }
    else // successor가 몇칸 내려가는 경우일 때,
    {
      rbtree_transplant(t, y, y->right); // y자리의 오른쪽 부분 기준 나무심음
      y->right = erase->right;           // 지울 노드의 오른자식을 y위치의 오른자식으로
      y->right->parent = y;
    }

    rbtree_transplant(t, erase, y);
    // 실제 심는과정
    y->left = erase->left;
    y->left->parent = y;
    y->color = erase->color;
  }

  if (y_try_to_erase_color == RBTREE_BLACK)
  {
    rbtree_erase_fixup(t, e_child);
  }

  free(erase);
  return 0;
}

/* 삭제하려는 노드의 대체노드에 각종 삭제정보를 이식하는 함수 */
void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
    t->root = v;
  // 삭제할 노드 u가 트리의 Root노드면, 대체노드 v를 트리의 Root노드로 삼는다
  else if (u == u->parent->left)
    u->parent->left = v;
  // 삭제할 노드 u자신이 왼자식이면, 대체노드 v를 삭제할 노드 u의 부모의 왼자식으로 삼는다
  else
    u->parent->right = v;
  // 둘다 아니면 = 삭제할 노드 u자식이 오른자식이면, 대체노드 v를 삭제할 노드 u의 부모의 오른자식으로 삼는다

  v->parent = u->parent; // 삭제할 노드 u의 부모를 대체노드 v의 부모로 삼는다 (부모가 nil이어도 상관x)
}

void rbtree_erase_fixup(rbtree *t, node_t *current)
{
  node_t *brother = NULL; // 삭제노드x의 형제노드를 가리키는 포인터를 선언.

  while (current != t->root && current->color == RBTREE_BLACK)
  // 삭제노드가 Root노드가 아니며, 색이 Black이어서 지우기 곤란할 때
  {
    if (current == current->parent->left) // 삭제노드가 왼자식일 때
    {
      brother = current->parent->right; // 형제노드를 삭제노드의 형제로

      // <경우 1> : 경우 2, 3, 4로 위임 가능
      if (brother->color == RBTREE_RED) // 형제노드가 Red일 때 (참고#1)
      {
        brother->color = RBTREE_BLACK;
        current->parent->color = RBTREE_RED;
        // 형제노드를 Black으로 바꾸고, 부모노드를 Red로 바꿈( 형제노드가 Red이므로 부모가 무조건 Black이어야 한다 참고#1 )
        left_rotate(t, current->parent); // 삭제노드의 부모를 기준으로 회전
        brother = current->parent->right;
        continue;
        // 회전을 끝내고 난 후에는 current->parent->right가 새로운 노드가 되고
        // 밑의 if, else if, else 중 한 가지, 즉 경우 2, 3, 4의 한 가지로 위임된다.
      }

      // else (brother->color == RBTREE_BLACK){...} else는 indent 증가로 인해 따로 쓰지않음
      // <경우 2> : x노드가 위쪽으로 바뀌는 경우
      if (brother->left->color == RBTREE_BLACK && brother->right->color == RBTREE_BLACK)
      // 형제의 자식들이 모두 Black이면 (+else까지 합치면 형제와그자식들 모두 Black)
      {
        brother->color = RBTREE_RED; // x의 extra black을 current->parent로 넘긴다. 그러면서 w는 red가 된다.
        current = current->parent;   // 새로운 current
        continue;
      }
      else // 형제의 자식들이 모두 Black은 아니면 (Red 1개이상 보유) (참고#2)
      {
        // <경우 3> : 경우 4로 위임 가능
        if (brother->right->color == RBTREE_BLACK)
        // 형제의 왼자식이 Black이면 (= 형제의 오른자식 색이 Red 참고#2)
        {
          brother->left->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;      // 형제(black)과 형제자식(red)를 서로 바꾼 후
          right_rotate(t, brother);         // 오른쪽으로 돌린다
          brother = current->parent->right; // 형제노드를 새로 지정
        }

        // <경우 4> : 특성이 위반되는 것을 해결
        brother->color = current->parent->color;
        current->parent->color = RBTREE_BLACK; // 형제와 부모색 교환 (형제가 Black인 경우이므로 그냥 Black 하드코딩)
        brother->right->color = RBTREE_BLACK;  // 자식 색 검정화
        left_rotate(t, current->parent);
        current = t->root; // 경우 4를 거치면 특성 위반이 해결. x를 root로 설정하여 while문 탈출
      }
    }
    else // 삭제노드가 오른자식일 때 (MIRROR CASE)
    {
      brother = current->parent->left; // 형제노드를 삭제노드의 형제로

      // <경우 1> : 경우 2,3,4로 위임가능
      if (brother->color == RBTREE_RED) // 형제노드가 Red일 때 (참고#1)
      {
        brother->color = RBTREE_BLACK;
        current->parent->color = RBTREE_RED; // 형제노드를 Black으로 바꾸고, 부모노드를 Red로 바꿈( 형제노드가 Red이므로 부모가 무조건 Black이어야 한다 참고#1 )
        right_rotate(t, current->parent);    // 삭제노드의 부모를 기준으로 회전
        brother = current->parent->right;
        continue;
        // 회전을 끝내고 난 후에는 current->parent->right가 새로운 노드가 되고
        // 밑의 if, else if, else 중 한 가지, 즉 경우 2, 3, 4의 한 가지로 위임된다.
      }

      // else (brother->color == RBTREE_BLACK){...} else는 indent 증가로 인해 따로 쓰지않음
      // <경우 2>
      if (brother->right->color == RBTREE_BLACK && brother->left->color == RBTREE_BLACK)
      // 형제의 자식들이 모두 Black이면 (+else까지 합치면 형제와그자식들 모두 Black)
      {
        brother->color = RBTREE_RED; // x의 extra black을 current->parent로 넘긴다. 그러면서 w는 red가 된다.
        current = current->parent;   // 새로운 current
        continue;
      }
      else // 형제의 자식들이 모두 Black은 아니면 (Red 1개이상 보유) (참고#2)
      {
        // <경우 3> : 경우 4로 위임 가능
        if (brother->left->color == RBTREE_BLACK)
        // 형제의 왼자식 Black이면 (= 형제의 오른자식색이 Red 참고#2)
        {
          brother->right->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;     // 형제(black)과 형제자식(red)를 서로 바꾼 후
          left_rotate(t, brother);         // 왼쪽으로 돌린다
          brother = current->parent->left; // 형제노드를 새로 지정
        }

        // <경우 4> : 특성이 위반되는 것을 해결
        brother->color = current->parent->color;
        current->parent->color = RBTREE_BLACK; // 형제와 부모색 교환 (형제가 Black인 경우이므로 그냥 Black 하드코딩)
        brother->left->color = RBTREE_BLACK;   // 자식 색 검정화
        right_rotate(t, current->parent);
        current = t->root; // 경우 4를 거치면 특성 위반이 해결. x를 root로 설정하여 while문 탈출
      }
    }
  }

  current->color = RBTREE_BLACK; // x가 root가 되면 Black속성 부여. 양쪽의 Black-height에 영향이 없으므로
}


void inorder(const rbtree *t, node_t *current, key_t *arr, int *idx, const size_t n)
{
  if (current == t->nil || *idx >= n)
  {
    return;
  }

  inorder(t, current->left, arr, idx, n);

  if (*idx < n)
  {
    arr[(*idx)++] = current->key;
  }

  inorder(t, current->right, arr, idx, n);
}

//t를 inodrder로 n번 순회한 결과를 arr에 담는 함수
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  node_t *current = t->root;
  if (current == t->nil)
    return 0;

  int cnt = 0;
  int *idx = &cnt;
  inorder(t, current, arr, idx, n);

  return 0;
}
