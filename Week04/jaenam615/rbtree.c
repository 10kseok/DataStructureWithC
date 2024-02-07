#include "rbtree.h"

#include <stdlib.h>

void rbtree_insert_fixup(rbtree *t, node_t *z);
void rbtree_erase_fixup(rbtree *t, node_t *x);
node_t *rbtree_transplant(rbtree *t, node_t *u, node_t *v);
node_t *left_rotate(rbtree *t, node_t *x);
node_t *right_rotate(rbtree *t, node_t *x);

rbtree *new_rbtree(void) // 새로운 rbtree 생성
{
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // rbtree 구조체 메모리 할당
    node_t *nil = (node_t *)malloc(sizeof(node_t));  //  nil 노드 메모리 할당
    p->root = nil;                                   // 트리 p의 root를 nil로 설정
    p->nil = nil;                                    // 트리 p의 nil을 nil로 설정
    p->nil->color = RBTREE_BLACK;                    // nil의 색깔을 검정으로 설정

    return p; // p 반환
}

void postorder(node_t *p, rbtree *t)
{
    if (p != t->nil)
    {
        postorder(p->left, t);
        postorder(p->right, t);
        free(p);
    }
}

void delete_rbtree(rbtree *t)
{
    // TODO: reclaim the tree nodes's memory
    node_t *x = t->root;
    if (x != t->nil)
    {
        postorder(x, t);
    }
    free(t->nil);
    free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
    node_t *z = (node_t *)malloc(sizeof(node_t));
    node_t *y = t->nil;
    node_t *x = t->root;

    z->key = key;

    while (x != t->nil) // 넣어지는 값(노드)의 위치 찾기(?)
    {
        y = x;
        if (z->key < x->key)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
    z->parent = y;   // z의 부모를 y로 설정
    if (y == t->nil) // 만약 y 가 닐을 가리키면, 즉 트리가 비어있다면
    {
        t->root = z; // 트리의 루트에 z를 넣어라
    }
    else if (z->key < y->key)
    {                // z의 값이 부모의 키값보다 적으면
        y->left = z; // y의 왼쪽에 z를 넣어라
    }
    else
    {
        y->right = z; // 아닐 경우(부모와 같거나 크면) y의 오른쪽에 넣어라
    }
    z->left = t->nil; // z의 왼쪽과 오른쪽을 nil로
    z->right = t->nil;
    z->color = RBTREE_RED;
    rbtree_insert_fixup(t, z);

    return t->root;
}

void rbtree_insert_fixup(rbtree *t, node_t *z)
{
    node_t *y = NULL; // node y 초기화

    while (z->parent->color == RBTREE_RED) //(본 함수 진입 전 삽입 된) node z의 부모가 빨간색일때 -> red conflict
    {
        if (z->parent == z->parent->parent->left) // z의 부모가 할아버지의 왼쪽 자손일 때
        {
            y = z->parent->parent->right; // 노드 y(temp)에 z의 삼촌 저장
            if (y->color == RBTREE_RED)
            {                                          // 삼촌도 빨간색이면 -> RECOLORING
                z->parent->color = RBTREE_BLACK;       // 부모를 검정색으로 만듬
                y->color = RBTREE_BLACK;               // 삼촌도 검정색으로 만듬
                z->parent->parent->color = RBTREE_RED; // 할아버지는 빨강색으로 만듬
                z = z->parent->parent;                 // 할아버지도 검사해야 하기 떄문에, while문에 할아버지 검사하기 위해 z가 할아버지를 가리키게 하고 다시 while조건 확인
                continue;
            }
            if (z == z->parent->right) // 아버지는 왼쪽 자식이지만, 나는 오른쪽 자식일 때, 일자를 만들어야 하기 때문에
            {
                z = z->parent;
                left_rotate(t, z);
            }
            z->parent->color = RBTREE_BLACK;
            z->parent->parent->color = RBTREE_RED;
            right_rotate(t, z->parent->parent);
        }
        else // z의 부모가 할아버지의 오른쪽 자손일때
        {
            y = z->parent->parent->left;
            if (y->color == RBTREE_RED) // 삼촌도 빨간색일때
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
                continue;
            }

            if (z == z->parent->left)
            {
                z = z->parent;
                right_rotate(t, z);
            }
            z->parent->color = RBTREE_BLACK;
            z->parent->parent->color = RBTREE_RED;
            left_rotate(t, z->parent->parent);
        }
    }
    t->root->color = RBTREE_BLACK;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
    node_t *x = t->root; // 인수로 받은 트리 t의 root값을 x가 가리키게 함

    if (t->root == t->nil)
    {
        return NULL;
    }

    while (x != t->nil) // x가 가리키는 것이 트리 t의 nil이 아닐 때
    {
        if (x->key == key) // x가 가리키는 것이 입력받은 key와 같으면
        {
            break; // 탈출(찾기 성공?)
        }

        if (x->left == t->nil && x->right == t->nil) // x가 가리키는 노드의 왼쪽 자식과 오른쪽 자식이 모두 nil이라면
        {
            return NULL; // 아니라는 것을 반환 (탈출)
        }

        if (x->key > key) // x가 가리키는 노드의 키값이 입력받은 인수 key값보다 크다면
        {
            x = x->left; // 왼쪽 자식을 x로 갱신
        }
        else // 반대의 경우
        {
            x = x->right; // 오른쪽 자식을 x로 갱신
        }
    }

    // TODO: implement find
    return x; // x값 반환
}

node_t *rbtree_min(const rbtree *t)
{
    node_t *x = t->root;      // x가 인수로 들어온 RB tree t의 원소 'root'를 가리키게 함
    while (x->left != t->nil) // x의 (= t의 root의) 왼쪽이 nil이 아닐때
    {
        x = x->left; // x가 왼쪽을 가리키게 함(?)
    }
    return x; // x를 반환
}

node_t *rbtree_max(const rbtree *t)
{
    node_t *x = t->root;
    while (x->right != t->nil)
    {
        x = x->right;
    }
    return x;
}

node_t *subtree_max(const rbtree *t, node_t *left) // 왼쪽 서브트리에서 max
{
    node_t *x = NULL;
    x = left;
    while (x->right != t->nil)
    {
        x = x->right;
    }
    return x;
}

node_t *subtree_min(const rbtree *t, node_t *right) // 오른쪽 서브트리에서 min
{
    node_t *x = NULL;
    x = right;
    while (x->left != t->nil)
    {
        x = x->left;
    }
    return x;
}

int rbtree_erase(rbtree *t, node_t *p)
{
    node_t *y = NULL;
    node_t *x = NULL;
    y = p;
    color_t y_ori_color = y->color;

    if (p->left == t->nil)
    {
        x = p->right;
        rbtree_transplant(t, p, p->right);
    }
    else if (p->right == t->nil)
    {
        x = p->left;
        rbtree_transplant(t, p, p->left);
    }
    else // 자식이 2명인 경우
    {
        y = subtree_min(t, p->right);
        y_ori_color = y->color;
        x = y->right;
        if (y->parent == p)
            x->parent = y;
        else
        {
            rbtree_transplant(t, y, y->right);
            y->right = p->right;
            y->right->parent = y;
        }
        rbtree_transplant(t, p, y);
        y->left = p->left;
        y->left->parent = y;
        y->color = p->color;
    }
    free(p);
    if (y_ori_color == RBTREE_BLACK)
        rbtree_erase_fixup(t, x);

    return 0;
}

void rbtree_erase_fixup(rbtree *t, node_t *x)
{
    node_t *w = NULL;
    while (x != t->root && x->color == RBTREE_BLACK)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == RBTREE_RED)
            {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                left_rotate(t, x->parent);
                w = x->parent->right;
                continue;
            }
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
            {
                w->color = RBTREE_RED;
                x = x->parent;
                continue;
            }
            else if (w->right->color == RBTREE_BLACK)
            {
                w->left->color = RBTREE_BLACK;
                w->color = RBTREE_RED;
                right_rotate(t, w);
                w = x->parent->right;
            }

            w->color = x->parent->color;
            x->parent->color = RBTREE_BLACK;
            w->right->color = RBTREE_BLACK;
            left_rotate(t, x->parent);
            x = t->root;
        }
        else
        {
            w = x->parent->left;
            if (w->color == RBTREE_RED)
            {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t, x->parent);
                w = x->parent->right;
                continue;
            }
            if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
            {
                w->color = RBTREE_RED;
                x = x->parent;
                continue;
            }
            else if (w->left->color == RBTREE_BLACK)
            {
                w->right->color = RBTREE_BLACK;
                w->color = RBTREE_RED;
                left_rotate(t, w);
                w = x->parent->left;
            }
            w->color = x->parent->color;
            x->parent->color = RBTREE_BLACK;
            w->left->color = RBTREE_BLACK;
            right_rotate(t, x->parent);
            x = t->root;
        }
    }
    x->color = RBTREE_BLACK;
    // t->root->color = RBTREE_BLACK;
}

node_t *rbtree_transplant(rbtree *t, node_t *u, node_t *v) // u와 v의 부모를 바꿈
{
    if (u->parent == t->nil)
        t->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;
    return t->root;
}

void inorder(const rbtree *t, node_t *x, key_t *arr, int *idx, const size_t n)
{
    if (x == t->nil)
    {
        return;
    }
    inorder(t, x->left, arr, idx, n);
    if (*idx < n)
    {
        arr[(*idx)] = x->key;
        (*idx)++;
    }
    else
    {
        return;
    }
    inorder(t, x->right, arr, idx, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
    node_t *x = t->root;
    if (x == t->nil)
        return 0;
    int cnt = 0;
    int *idx = &cnt;
    inorder(t, x, arr, idx, n);
    return 0;
}

node_t *left_rotate(rbtree *t, node_t *x) // x 기준, 왼쪽으로 회전
{
    node_t *y = x->right;
    x->right = y->left;
    if (y->left != t->nil)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == t->nil)
    {
        t->root = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;

    return t->root;
}

node_t *right_rotate(rbtree *t, node_t *x) // x 기준, 오른쪽으로 회전
{
    node_t *y = x->left;
    x->left = y->right;

    if (y->right != t->nil)
        y->right->parent = x;

    y->parent = x->parent;
    if (x->parent == t->nil)
        t->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->right = x;
    x->parent = y;

    return t->root;
}