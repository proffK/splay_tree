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

#ifndef __SPLAY_TREE__
#define __SPLAY_TREE__

/*!
        \file splay_tree.h
        \brief Header file for Splay Tree library.
        \author Kireev Klim
        \version 1.0
        \date 04.03.2016
        \copyright GPL 3.0
        \warning Is was written by me for studies purpose. Use it at your own risk.
*/

/*!
        \brief main struct is used as splay tree.
*/
struct splay_tree_t;

/*!
        \brief It is type of objects stored in splay tree.
*/
typedef int node_elem;

/*!
        \brief It is key, that you must use for any operation with tree.
*/
typedef int node_key;

/*!
        \brief It is object, that will return if isn't element, that you've found.
*/
extern const node_elem NULL_NELEM;

/*!
        \brief It is object, that will return if function want return error.
*/
extern const node_key ERR_NELEM;

typedef struct splay_tree_t splay_tree;

/*!
        \brief Create splay tree.
        \param[in] get_key Function, that return key for every object.
        \return Pointer to created splay tree.It return NULL and set 
        errno in ENOMEM, if It isn't memory.
*/
splay_tree* splay_tree_create(node_key (*get_key) (node_elem elem));

/*!
        \brief Delete splay tree.
        \param tr Pointer to existing splay tree.
        \return 0 if succes.
*/
int splay_tree_delete(splay_tree* tr);

/*!
        \brief Insert object in splay tree.
        \param tr Pointer to existing splay tree.
        \param new_elem Data of new tree element.
        \return 0 if succes, -1 if failed and set errno.
*/
int splay_tree_insert(splay_tree* tr, node_elem new_elem);

/*!
        \brief Remove object from splay tree.
        \param tr Pointer to existing splay tree.
        \param key Key of splay tree element.
        \return 0 if succes, -1 if failed and set errno.
*/
int splay_tree_remove(splay_tree* tr, node_key key);

/*!
        \brief Find object in splay tree.
        \param tr Pointer to existing splay tree.
        \param key Key of splay tree element.
        \return Founded element if it exist, NULL_NELEM if not.
*/
node_elem splay_tree_find(splay_tree* tr, node_key key);

/*!
        \brief Do function for every object in splay tree.
        \param tr Pointer to existing splay tree.
        \param callback Function, that will launch for every object.
        \param data Argument for callback.
        \return Last callback returned code.
*/
int splay_tree_for_each_elem(splay_tree* tr,
                                int (*callback) (splay_tree* tr,
                                                 node_elem* elem,
                                                 node_key* key,
                                                 void* data),
                                void* data);

#endif
