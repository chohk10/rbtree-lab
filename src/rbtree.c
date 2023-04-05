#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree)); 
  node_t *sentinel = (node_t *)calloc(1, sizeof(node_t));
  sentinel->color = RBTREE_BLACK;
  t->nil = sentinel; 
  t->root = t->nil; 
  return t;
}

static void delete_nodes(node_t *node, node_t *nil) {
  if (node != nil) { 
    delete_nodes(node->left, nil); 
    delete_nodes(node->right, nil);
    free(node); 
  }
}

void delete_rbtree(rbtree *t) {
  delete_nodes(t->root, t->nil);
  free(t->nil);  
  free(t);
}

static void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  x->right = y->left;  
  if (y->left != t->nil) { 
    y->left->parent = x; 
  }
  y->parent = x->parent; 
  if (x->parent == t->nil) { 
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

static void right_rotate(rbtree *t, node_t *y) {
  node_t *x = y->left;
  y->left = x->right;
  if (x->right != t->nil) {
    x->right->parent = y;
  }
  x->parent = y->parent;
  if (y->parent == t->nil) {
    t->root = x;
  } else if (y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }
  x->right = y;
  y->parent = x;
}

static void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) { 
    if (z->parent == z->parent->parent->left) {
      node_t *y = z->parent->parent->right;
      if (y->color == RBTREE_RED) { 
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED; 
        z = z->parent->parent;
      } else { 
        if (z == z->parent->right) {
          z = z->parent;  
          left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    } else {
      node_t *y = z->parent->parent->left;
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
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

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *parent = t->nil; 
  node_t *node = t->root;
  while (node != t->nil) {  
    parent = node;  
    if (key < node->key) {
      node = node->left;
    } else { 
      node = node->right;
    } 
  }
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->color = RBTREE_RED;  
  new_node->key = key;
  new_node->parent = parent; 
  new_node->left = t->nil;  
  new_node->right = t->nil;  
  if (parent == t->nil) {
    t->root = new_node;
  } else if (key < parent->key) { 
    parent->left = new_node;
  } else {
    parent->right = new_node;
  }
  rbtree_insert_fixup(t, new_node);
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x = t->root;
  while (x != t->nil && x->key != key) {
    if (x->key <= key) {
      x = x->right;
    } else {
      x = x->left;
    }
  }
  return (x == t->nil) ? NULL : x;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *x = t->root;
  while (x->left != t->nil) {
    x = x->left;
  }
  return x;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *x = t->root;
  while (x->right != t->nil) {
    x = x->right;
  }
  return x;
}

static void transplant(rbtree *t, node_t *old_node, node_t *new_node) { 
  if (old_node->parent == t->nil) {
    t->root = new_node;
  }
  if (old_node == old_node->parent->left) {
    old_node->parent->left = new_node;
  } else {
    old_node->parent->right = new_node;
  }
  new_node->parent = old_node->parent;
}

static node_t *subtree_min(node_t *node, node_t *nil) {
  while (node->left != nil) {
    node = node->left;
  }
  return node;
}

static node_t *subtree_max(node_t *node, node_t *nil) {
  while (node->right != nil) {
    node = node->right;
  }
  return node;
}

static void rbtree_erase_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {  
    if (x == x->parent->left) {
      node_t *w = x->parent->right; 
      if (w->color == RBTREE_RED) {  
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);  
        w = x->parent->right;  
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { 
        w->color = RBTREE_RED;
        x = x->parent;
      } else {  
        if (w->right->color == RBTREE_BLACK) {
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
    } else { 
      node_t *w = x->parent->left; 
      if (w->color == RBTREE_RED) {  
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent); 
        w = x->parent->left; 
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) { 
        w->color = RBTREE_RED;
        x = x->parent; 
      } else { 
        if (w->left->color == RBTREE_BLACK) {
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
  }
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *z) {
  if (z == NULL) {
    return 0;
  }
  node_t *y = z;                   
  color_t y_original_color = y->color; 
  node_t *x = NULL;
  if (z->left == t->nil) {    
    x = z->right;             
    transplant(t, z, z->right); 
  } else {  
    if (z->right == t->nil) {   
      x = z->left;              
      transplant(t, z, z->left);
    } else {                    
      y = subtree_min(z->right, t->nil); 
      y_original_color = y->color;   
      x = y->right;             
      if (y == z->right) {      
        x->parent = y;          
      } else {                  
        transplant(t, y, y->right); 
        y->right = z->right;        
        y->right->parent = y;
      }
      transplant(t, z, y);          
      y->left = z->left;            
      y->left->parent = y;
      y->color = z->color;          
    }
  }
  if (y_original_color == RBTREE_BLACK) { 
    rbtree_erase_fixup(t, x);
  }
  free(z);
  return 0;
}

static void inorder_to_array(const rbtree *t, const node_t *root, int *pidx, key_t *arr){
  if (root != t->nil){
    inorder_to_array(t, root->left, pidx,arr);
    arr[(*pidx)++] = root->key;
    inorder_to_array(t, root->right, pidx,arr);
  }
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int idx = 0;
  int *pidx = &idx;
  inorder_to_array(t, t->root, pidx, arr);
  return 0;
}