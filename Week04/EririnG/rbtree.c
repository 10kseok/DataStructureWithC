#include "rbtree.h"

#include <stdlib.h>

void rbtree_insert_fixup(rbtree *t, node_t *z);
void rbtree_erase_fixup(rbtree *t, node_t *x);
node_t *rbtree_transplant(rbtree *t, node_t *u, node_t *v);
node_t *left_rotate(rbtree *t, node_t *x);
node_t *right_rotate(rbtree *t, node_t *x);

rbtree *new_rbtree(void)
{
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    node_t *nil = (node_t *)malloc(sizeof(node_t));
    p->root = nil;
	p->nil = nil;
    p->nil->color = RBTREE_BLACK;
    
    return p;
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
    node_t *x = t->root;
    postorder(x, t);
	free(t->nil);
    free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
    node_t *new = (node_t *)malloc(sizeof(node_t));
    node_t *y = t->nil;
    node_t *x = t->root;
    new->key = key;

    while (x != t->nil)
    { // x가 nil을 가리킬때까지 이진 탐색을 반복
        y = x;
        if (key < x->key)
            x = x->left;
        else
            x = x->right;
    }
    // while이 끝났을 때는 x는 nil을, y는 그 부모를 가리킴

    new->parent = y;
    if (y == t->nil) // 반복이 끝났는데 y가 nil을 가리키면 트리가 비었다는 뜻
        t->root = new;
    else if (key < y->key)
        y->left = new;
    else
        y->right = new;

    new->left = t->nil;
    new->right = t->nil;
    new->color = RBTREE_RED;

    rbtree_insert_fixup(t, new); // 새로운 삽입으로 규칙 위반일 수 있으니 확인

    return t->root; // root가 바뀌었을 수 있으니 루트 반환
}

void rbtree_insert_fixup(rbtree *t, node_t *z)
{
    node_t *y = NULL;

    while (z->parent->color == RBTREE_RED)
    {
        if (z->parent == z->parent->parent->left) // 내 부모 노드가 할아버지 노드의 왼쪽 자식일때
        {
            y = z->parent->parent->right;
            if (y->color == RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else{
                if (z == z->parent->right)
                {
                    z = z->parent;
                    left_rotate(t, z); // p.319 경우(1,2,3) 그림 참고
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                right_rotate(t, z->parent->parent);
            }
        }
        else
        {
            y = z->parent->parent->left;
            if (y->color == RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else{
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
    node_t *y = p;
    node_t *x = NULL;
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
        x = y->right;    // 최종적으로 null 또는 p->right를 가리킴
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
    if (y_ori_color == RBTREE_BLACK){
        rbtree_erase_fixup(t, x);
	}

	free(p);
    return 0;
}

void rbtree_erase_fixup(rbtree *t, node_t *x)
{
    node_t *s = NULL; // x의 형제 노드
    while (x != t->root && x->color == RBTREE_BLACK)
    {
        if (x == x->parent->left) // x가 부모의 왼쪽 자식일 때
        {
            s = x->parent->right;
            if (s->color == RBTREE_RED)
            {
                s->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                left_rotate(t, x->parent);
                s = x->parent->right;
                continue;
            }
            if (s->left->color == RBTREE_BLACK && s->right->color == RBTREE_BLACK)
            {
                s->color = RBTREE_RED;
                x = x->parent;
            }
            else {
				if (s->right->color == RBTREE_BLACK)
				{
					s->left->color = RBTREE_BLACK;
					s->color = RBTREE_RED;
					right_rotate(t, s);
					s = x->parent->right;
				}
				s->color = x->parent->color;
				x->parent->color = RBTREE_BLACK;
				s->right->color = RBTREE_BLACK;
				left_rotate(t, x->parent);
				x = t->root;
			}
        }
        else  // x가 부모의 오른쪽 자식일 때
        {
            s = x->parent->left;
            if (s->color == RBTREE_RED)
            {
                s->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t, x->parent);
                s = x->parent->right;
                continue;
            }
            if (s->right->color == RBTREE_BLACK && s->left->color == RBTREE_BLACK)
            {
                s->color = RBTREE_RED;
                x = x->parent;
            }
            else {
				if (s->left->color == RBTREE_BLACK)
				{
					s->right->color = RBTREE_BLACK;
					s->color = RBTREE_RED;
					left_rotate(t, s);
					s = x->parent->left;
				}
				s->color = x->parent->color;
				x->parent->color = RBTREE_BLACK;
				s->left->color = RBTREE_BLACK;
				right_rotate(t, x->parent);
				x = t->root;
			}
        }
    } 
    x->color = RBTREE_BLACK;
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
        arr[(*idx)++] = x->key;
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
    node_t *y = x->right; // y 설정
    x->right = y->left;   // y의 왼쪽 서브트리를 x의 오른쪽으로 옮겨줌

    if (y->left != t->nil)
    {                        // 왼쪽 서브트리가 nil이 아니면
        y->left->parent = x; // 왼쪽 서브트리의 부모를 x로 설정
    }

    y->parent = x->parent;   // y의 부모를 x의 부모로 설정
    if (x->parent == t->nil) // 만약 x가 root였으면
        t->root = y;         // y를 root로 설정
    else if (x == x->parent->left)
        x->parent->left = y; // x가 부모의 왼쪽 자식이면 y를 연결해줌
    else
        x->parent->right = y; // x가 부모의 오른쪽 자식이면 y를 연결해줌

    y->left = x;   // y의 왼쪽 자식을 x로 설정
    x->parent = y; // x의 부모를 y로 설정

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
