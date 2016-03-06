/*
    This file is part a my splay tree library :)
    Copyright (C) 2016  Kireev Klim

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    My email: proffk@linux.com
*/

#include <splay_tree.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define ERR_EXIT(cond, error, ret) \
        do {\
                if ((cond)) {\
                        errno = (error);\
                        return (ret);\
                }\
        } while (0)

struct node_t {
        struct node_t* left;
        struct node_t* right;
        struct node_t* parent;
        node_elem data;
        node_key key;
};

typedef struct node_t node;

struct splay_tree_t {
        node* root;
        node_key (*get_key) (node_elem elem);
};

const node_elem NULL_NELEM = 0;
const node_key ERR_NELEM = -1;

typedef struct splay_tree_t splay_tree;

static node* node_create(node_elem data, node_key key)
{
        node* new_node = (node*) calloc (sizeof(node), 1);
        ERR_EXIT(new_node == NULL, ENOMEM, NULL);

        new_node->data = data;
        new_node->key = key;

        return new_node;
}

#ifdef TREE_DEBUG
static int node_dump(node* nd) {

        fprintf(stderr, "node:  %p\n"
                        "par:   %p\n"
                        "left:  %p\n"
                        "right: %p\n"
                        "key:   %d\n\n",
                        nd, nd-> parent, nd->left, nd->right, nd->key);
        return 0;
}
#endif

static int node_delete(node* nd)
{
        if (nd == NULL) {
                return 0;
        }


        node_delete(nd->left);
        node_delete(nd->right);

        //node_dump(nd);
        free(nd);
        return 0;
}

static inline void set_parent(node* child, node* parent)
{
        if (child != NULL) {
                child->parent = parent;
        }
}

static inline void keep_parent(node* parent)
{
        if (parent != NULL) {
                set_parent(parent->left, parent);
                set_parent(parent->right, parent);
        }
}

static inline void rotate(node* parent, node* child)
{
        node* gparent = parent->parent;
        if (gparent != NULL) {
                if (gparent->left == parent) {
                        gparent->left = child;
                } else {
                        gparent->right = child;
                }
        }

        if (parent->left == child) {
                parent->left = child->right;
                child->right = parent;
        } else {
                parent->right = child->left;
                child->left = parent;
        }

        keep_parent(child);
        keep_parent(parent);

        set_parent(child, gparent);
}

static node* node_splay(node* nd)
{
        node* parent = nd->parent;
        node* gparent = NULL;

        if (parent == NULL) {
                return nd;
        }

        gparent = parent->parent;

        if (gparent == NULL) {
                /* Zig */

                rotate(parent, nd);
                return nd;
        }

        if ((nd == parent->left && parent == gparent->left) ||
            (nd == parent->right && parent == gparent->right)) {
                /* Zig Zig */
                
                rotate(gparent, parent);
                rotate(parent, nd);
        } else {
                /* Zig Zag */

                rotate(parent, nd);
                rotate(gparent, nd);
        }

        return node_splay(nd);
}

static node* node_find(node* nd, node_key key)
{
        if (nd->key == key) {
                return node_splay(nd);
        }

        if (nd->key < key && nd->right != NULL) {
                return node_find(nd->right, key);
        }

        if (nd->key > key && nd->left != NULL) {
                return node_find(nd->left, key);
        }

        return node_splay(nd);
}

static int node_split(node* root, node_key key, node** out_pair)
{
        root = node_find(root, key);

        if (root->key == key) {
                //fprintf(stderr, 
                          //"! l %p r %p\n", root->left, root->right);
                set_parent(root->left, NULL);
                set_parent(root->right, NULL);
                out_pair[0] = root->left;
                out_pair[1] = root->right;
                root->left = NULL;
                root->right = NULL;
                node_delete(root);
                return 0;
        }

        if (root->key > key) {
                node* tmp_left = root->left;
                root->left = NULL;
                set_parent(tmp_left, NULL);

                out_pair[0] = tmp_left;
                out_pair[1] = root;
        } else {
                node* tmp_right = root->right;
                root->right = NULL;
                set_parent(tmp_right, NULL);

                out_pair[0] = root;
                out_pair[1] = tmp_right;
        }

        return 0;
}

static int node_insert(node* root, node* new_node)
{
        node* split_pair[2] = {};

        node_split(root, new_node->key, split_pair);

        new_node->left = split_pair[0];
        new_node->right = split_pair[1];

        keep_parent(new_node);

        return 0;
}

static node* node_merge(node* left, node* right)
{
        if (left == NULL) {
                return right;
        }
        if (right == NULL) {
                return left;
        }

        right = node_find(right, left->key);
        right->left = left;
        set_parent(left, right);

        return right;
}

static node* node_remove(node* root, node_key key)
{
        node* left = root->left;
        node* right = root->right;
        root = node_find(root, key);

        set_parent(root->left, NULL);
        root->left = NULL;

        set_parent(root->right, NULL);
        root->right = NULL;

        node_delete(root);

        return node_merge(left, right);
}
static int node_for_each_elem(splay_tree* tr, node* nd,
                                int (*callback) (splay_tree* tr,
                                                 node_elem* elem,
                                                 node_key* key,
                                                 void* data),
                                void* data)
{
        if (nd == NULL) return 0;

        node_for_each_elem(tr, nd->left, callback, data);
        node_for_each_elem(tr, nd->right, callback, data);

        return callback(tr, &(nd->data), &(nd->key), data);
}

splay_tree* splay_tree_create(node_key (*get_key) (node_elem))
{
        ERR_EXIT(get_key == NULL, EINVAL, NULL);

        splay_tree* new_tree = (splay_tree*) 
                                calloc (sizeof(splay_tree), 1);
        ERR_EXIT(new_tree == NULL, ENOMEM, NULL);

        new_tree->get_key = get_key;

        errno = 0;
        return new_tree;
}
        
int splay_tree_delete(splay_tree* tr)
{
        ERR_EXIT(tr == NULL, EINVAL, -1);

        if (tr->root != NULL)
        {
                node_delete(tr->root);
        }

        errno = 0;
        free(tr);

        return 0;
}

int splay_tree_insert(splay_tree* tr, node_elem new_elem)
{
        node* new_node = NULL;
        node_key key = 0;

        ERR_EXIT(tr == NULL, EINVAL, -1);

        key = tr->get_key(new_elem);

        new_node = node_create(new_elem, key);
        ERR_EXIT(new_node == NULL, ENOMEM, -1);

        if (tr->root == NULL) {
                tr->root = new_node;
                return 0;
        }

        node_insert(tr->root, new_node);

        tr->root = new_node;
        
        //node_dump(new_node);

        return 0;
}

int splay_tree_remove(splay_tree* tr, node_key key)
{
        node_elem find_ret = NULL_NELEM;

        find_ret = splay_tree_find(tr, key);

        if (find_ret == NULL_NELEM || find_ret == ERR_NELEM) {
                return -1;
        }

        tr->root = node_remove(tr->root, key);

        return 0;
}

node_elem splay_tree_find(splay_tree* tr, node_key key)
{
        ERR_EXIT(tr == NULL, EINVAL, ERR_NELEM);

        if (tr->root == NULL) {
                return NULL_NELEM;
        }

        tr->root = node_find(tr->root, key);

        if (tr->root->key != key) {
                return NULL_NELEM;
        }

        return tr->root->data;
}

int splay_tree_for_each_elem(splay_tree* tr,
                                int (*callback) (splay_tree* tr,
                                                 node_elem* elem,
                                                 node_key* key,
                                                 void* data),
                                void* data)
{
        ERR_EXIT(tr == NULL, EINVAL, -1);
        ERR_EXIT(callback == NULL, EINVAL, -1);

        if (tr->root == NULL) {
                errno = 0;
                return -1;
        }

        return node_for_each_elem(tr, tr->root, callback, data);
}
