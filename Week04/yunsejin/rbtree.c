#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  if (p->nil == NULL)
  {
    free(p);
    return NULL;
  }
  p->nil->color = RBTREE_BLACK;
  p->nil->parent = p->nil->left = p->nil->right = p->nil;
  p->root = p->nil;
  return p;
}

void delete_node(node_t *node, node_t *sentinel_node)
{
  if (node == sentinel_node)
  {
    return;
  }

  delete_node(node->left, sentinel_node);
  delete_node(node->right, sentinel_node);

  free(node);
}

void delete_rbtree(rbtree *t)
{
  delete_node(t->root, t->nil);
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *x) 
{                         
    node_t *y = x->right; 
    x->right = y->left;   

    if (y->left != t->nil)
    {                        
        y->left->parent = x; 
    }

    y->parent = x->parent;   
    if (x->parent == t->nil) 
        t->root = y;         
    else if (x == x->parent->left)
        x->parent->left = y; 
    else
        x->parent->right = y; 

    y->left = x;   
    x->parent = y; 

    return;
}

void right_rotate(rbtree *t, node_t *x)
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

    return ; 
}


void rbtree_insert_fixup(rbtree *t, node_t *p)
{
  while (p->parent->color == RBTREE_RED)
  {
    if (p->parent == p->parent->parent->left)
    {
      node_t *y = p->parent->parent->right;
      if (y->color == RBTREE_RED)
      {
        p->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        p->parent->parent->color = RBTREE_RED;
        p = p->parent->parent;
      }
      else
      {
        if (p == p->parent->right)
        {
          p = p->parent;
          left_rotate(t, p);
        }
        p->parent->color = RBTREE_BLACK;
        p->parent->parent->color = RBTREE_RED;
        right_rotate(t, p->parent->parent);
      }
    }
    else
    {
      node_t *y = p->parent->parent->left;
      if (y->color == RBTREE_RED)
      {
        p->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        p->parent->parent->color = RBTREE_RED;
        p = p->parent->parent;
      }
      else
      {
        if (p == p->parent->left)
        {
          p = p->parent;
          right_rotate(t, p);
        }
        p->parent->color = RBTREE_BLACK;
        p->parent->parent->color = RBTREE_RED;
        left_rotate(t, p->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // TODO: implement insert
  // 적절한 위치 찾는 코드
  // 값 삽입
  node_t *SENTINEL_NODE = t->nil;
  node_t *parent_node = t->nil;

  node_t *new_node = malloc(sizeof(node_t));
  new_node->key = key;
  new_node->right = SENTINEL_NODE;
  new_node->left = SENTINEL_NODE;
  new_node->color = RBTREE_RED;

  node_t *cur_node = t->root;
  while (cur_node != SENTINEL_NODE)
  {
    parent_node = cur_node;
    if (cur_node->key > key)
    {
      cur_node = cur_node->left;
    }
    else
    {
      cur_node = cur_node->right;
    }
  }
  new_node->parent = parent_node;

  if (parent_node == SENTINEL_NODE)
  {
    t->root = new_node;
  }
  else if (new_node->key < parent_node->key)
  {
    parent_node->left = new_node;
  }
  else
  {
    parent_node->right = new_node;
  }

  rbtree_insert_fixup(t, new_node);
  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *cur_node = t->root;
  while (cur_node != t->nil && key != cur_node->key)
  {
    if (cur_node->key < key)
    {
      cur_node = cur_node->right;
    }
    else if (cur_node->key > key)
    {
      cur_node = cur_node->left;
    }
  }
  return cur_node == t->nil ? NULL : cur_node;
}

node_t *rbtree_min(const rbtree *t)
{
  node_t *cur_node = t->root;
  while (cur_node->left != t->nil)
    cur_node = cur_node->left;

  return cur_node;
}

node_t *rbtree_max(const rbtree *t)
{
  node_t *cur_node = t->root;

  while (cur_node->right != t->nil)
    cur_node = cur_node->right;
  return cur_node;
}

node_t *node_min(node_t *node, node_t *sentinel_node)
{
  node_t *cur_node = node;
  while (cur_node->left != sentinel_node)
  {
    cur_node = cur_node->left;
  }
  return cur_node;
}

void rbtree_erase_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
  {
    t->root = v;
  }
  else if (u == u->parent->left)
  {
    u->parent->left = v;
  }
  else
  {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

void rbtree_erase_fixup(rbtree *t, node_t *fixup_start_node)
{
  node_t *cur_node = fixup_start_node;
  node_t *sibling;
  while (cur_node != t->root && cur_node->color == RBTREE_BLACK)
  {
    if (cur_node == cur_node->parent->left)
    {
      sibling = cur_node->parent->right;

      if (sibling->color == RBTREE_RED)
      {
        sibling->color = RBTREE_BLACK;
        cur_node->parent->color = RBTREE_RED;
        left_rotate(t, cur_node->parent);
        sibling = cur_node->parent->right;
      }

      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK)
      {
        sibling->color = RBTREE_RED;
        cur_node = cur_node->parent;
      }

      else {
        if (sibling->right->color == RBTREE_BLACK)
        {
          sibling->left->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          right_rotate(t, sibling);
          sibling = cur_node->parent->right;
        }
        sibling->color = cur_node->parent->color;
        cur_node->parent->color = RBTREE_BLACK;
        sibling->right->color = RBTREE_BLACK;
        left_rotate(t, cur_node->parent);
        cur_node = t->root;
      }
    }
    else {
      sibling = cur_node->parent->left;
      if (sibling->color == RBTREE_RED)
      {
        sibling->color = RBTREE_BLACK;
        cur_node->parent->color = RBTREE_RED;
        right_rotate(t, cur_node->parent);
        sibling = cur_node->parent->left;
      }
      if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK)
      {
        sibling->color = RBTREE_RED;
        cur_node = cur_node->parent;
      }
      else
      {
        if (sibling->left->color == RBTREE_BLACK)
        {
          sibling->right->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          left_rotate(t, sibling);
          sibling = cur_node->parent->left;
        }
        sibling->color = cur_node->parent->color;
        cur_node->parent->color = RBTREE_BLACK;
        sibling->left->color = RBTREE_BLACK;
        right_rotate(t, cur_node->parent);
        cur_node = t->root;
      }
    }
  }
  cur_node->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  node_t *target_node = p;
  node_t *x;
  color_t target_node_orginal_color = target_node->color;
  if (p->left == t->nil)
  {
    x = p->right;
    rbtree_erase_transplant(t, p, p->right);
  }
  else if (p->right == t->nil)
  {
    x = p->left;
    rbtree_erase_transplant(t, p, p->left);
  }
  else
  {
    target_node = node_min(p->right, t->nil);
    target_node_orginal_color = target_node->color;
    x = target_node->right;
    if (target_node->parent == p)
    {
      x->parent = target_node;
    }
    else
    {
      rbtree_erase_transplant(t, target_node, target_node->right);
      target_node->right = p->right;
      target_node->right->parent = target_node;
    }
    rbtree_erase_transplant(t, p, target_node);
    target_node->left = p->left;
    target_node->left->parent = target_node;
    target_node->color = p->color;
  }
  if (target_node_orginal_color == RBTREE_BLACK)
    rbtree_erase_fixup(t, x);
  free(p);
  return 0;
}

void inorder_traverse(node_t *p, key_t *arr, int *index, node_t *sentinel_node, const size_t n)
{
  if (p == sentinel_node || *index >= n)
    return;

  inorder_traverse(p->left, arr, index, sentinel_node, n);
  if (*index < n)
  {
    arr[*index] = p->key;
    (*index)++;
  }
  inorder_traverse(p->right, arr, index, sentinel_node, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  int index = 0;
  inorder_traverse(t->root, arr, &index, t->nil, n);
  return 0;
}