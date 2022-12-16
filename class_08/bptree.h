#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <atomic>
#include <thread>
#include <assert.h>
#include <strings.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "debug.h"
#include "rwlock.hh"
using namespace std;
#define MAX_OBJ (1000*1000)
#define N 4

typedef struct _DATA {
	int key;
	int val;
	RWLock rwlock;
	struct _DATA *next;
	
} DATA;

typedef struct _NODE {
	bool isLeaf;
	struct _NODE *chi[N];
	int key[N-1]; 
	int nkey;
	struct _NODE *parent;
} NODE;

typedef struct _TEMP {
	bool isLeaf;
	NODE *chi[N+1]; // for internal split (for leaf, only N is enough)
	int key[N]; // for leaf split
	int nkey;
} TEMP;

DATA Head;
DATA *Tail;

NODE *Root = NULL;

void
delete_entry(NODE *node, int key, DATA *data, bool node_is_predecessor);


void
init_root(void)
{
	Root = NULL;
}


void
print_tree_core(NODE *n)
{
	printf("[");
	for (int i = 0; i < n->nkey; i++) {
		if (!n->isLeaf) print_tree_core(n->chi[i]);
		printf("%d", n->key[i]);
		if (i != n->nkey-1 && n->isLeaf) putchar(' ');
	}
	if (!n->isLeaf) print_tree_core(n->chi[n->nkey]);
	printf("]");
}


void
print_tree(NODE *node)
{
	print_tree_core(node);
	printf("\n"); fflush(stdout);
}


NODE *
alloc_leaf(NODE *parent)
{
	NODE *node = NULL;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = true;
	node->parent = parent;
	node->nkey = 0;

	return node;
}


TEMP *
alloc_temp(NODE *parent)
{
	TEMP *node = NULL;
	if (!(node = (TEMP *)calloc(1, sizeof(TEMP)))) ERR;
	node->isLeaf = true;
	node->nkey = N-1;

	return node;
}


NODE *
find_leaf(NODE *node, int key)
{
	int kid;

	if (node->isLeaf) return node;
	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return find_leaf(node->chi[kid], key);
}


void
insert_in_leaf(NODE *leaf, int key, DATA *data)
{
	int i;

	if (key < leaf->key[0]) {
		for (i = leaf->nkey; i > 0; i--) {
			leaf->chi[i] = leaf->chi[i-1] ;
			leaf->key[i] = leaf->key[i-1] ;
		}
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else {
		for (i = 0; i < leaf->nkey; i++) {
			if (key < leaf->key[i]) break;
		}
		for (int j = leaf->nkey; j > i; j--) {
			leaf->chi[j] = leaf->chi[j-1] ;
			leaf->key[j] = leaf->key[j-1] ;
		}

		leaf->key[i] = key;
		leaf->chi[i] = (NODE *)data;

		}

	leaf->nkey++;

	return;
}


void
insert_in_temp_leaf(TEMP *leaf, int key, DATA *data)
{
	int i;
	if (key < leaf->key[0]) {
		for (i = leaf->nkey; i > 0; i--) {
			leaf->chi[i] = leaf->chi[i-1] ;
			leaf->key[i] = leaf->key[i-1] ;
		}
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else {
		for (i = 0; i < leaf->nkey; i++) {
			if (key < leaf->key[i]) break;
		}
		for (int j = leaf->nkey; j > i; j--) {
			leaf->chi[j] = leaf->chi[j-1] ;
			leaf->key[j] = leaf->key[j-1] ;
		}

		leaf->key[i] = key;
		leaf->chi[i] = (NODE *)data;

	}

	leaf->nkey++;

	return;
}


void
insert_in_internal(NODE *internal, int key, DATA *data)
{
	int i;

	if (key < internal->key[0]) {
		for (i = internal->nkey; i > 0; i--) {
			internal->key[i] = internal->key[i-1];
			internal->chi[i+1] = internal->chi[i];
		}
		internal->key[0] = key;
		internal->chi[1] = (NODE*)data;

	}
	else {
		for (i = 0; i < internal->nkey; i++) {
			if (key < internal->key[i]) break;
		}
		for (int j = internal->nkey; j > i; j--) {
			internal->key[j] = internal->key[j-1];
			internal->chi[j+1] = internal->chi[j];
		}

		internal->key[i] = key;
		internal->chi[i+1] = (NODE*)data;

	}

	internal->nkey++;

	return;
}


void
insert_in_temp_internal(TEMP *internal, int key, DATA *data)
{
	int i;

	if (key < internal->key[0]) {
		for (i = internal->nkey; i > 0; i--) {
			internal->key[i] = internal->key[i-1];
			internal->chi[i+1] = internal->chi[i];
		}
		internal->key[0] = key;
		internal->chi[1] = (NODE*)data;

	}
	else {
		for (i = 0; i < internal->nkey; i++) {
			if (key < internal->key[i]) break;
		}
		for (int j = internal->nkey; j > i; j--) {
			internal->key[j] = internal->key[j-1];
			internal->chi[j+1] = internal->chi[j];
		}

		internal->key[i] = key;
		internal->chi[i+1] = (NODE*)data;

	}

	internal->nkey++;

	return;
}


void
insert_in_parent(NODE *node, int key, DATA *split_node)
{
	NODE *parent_node = NULL;
	NODE *parent_split_node = NULL;
	NODE *internal_node = NULL;
	TEMP *temp = NULL;
	int i;
	int grand_parent_key;


	if (node == Root)
	{
		parent_node = alloc_leaf(NULL);
		parent_node->key[0] = key;
		parent_node->chi[0] = node;
		parent_node->chi[1] = (NODE *)split_node;
		parent_node->nkey = 1;
		Root = parent_node;
		parent_node->isLeaf = false;
		node->parent = parent_node;
		((NODE *)split_node)->parent = parent_node;

		return ;
	}

	parent_node = node->parent;
	((NODE*)split_node)->parent = parent_node;

	if (parent_node->nkey+1 < N)
	{

		parent_node->isLeaf = true;
		internal_node = find_leaf(Root, key);

		insert_in_internal(internal_node, key, split_node);

		parent_node->isLeaf = false;

	}
	else
	{
		temp = alloc_temp(NULL);

		for(i=0; i<N-1; i++){
			temp->key[i] = parent_node->key[i];
		}
		for(i=0; i<N; i++){
			temp->chi[i] = parent_node->chi[i];
		}

		insert_in_temp_internal(temp, key, split_node);

		for(i=0;i<N-1;i++){
			parent_node->key[i] = 0;
		}
		for(i=0;i<N;i++){
			parent_node->chi[i] = 0;
		}

		parent_split_node = alloc_leaf(NULL);
		parent_split_node->isLeaf = false;

		for(i=0; i<(N/2)+1; i++){
			parent_node->chi[i] = temp->chi[i];
		}
		for(i=0; i<(N/2); i++){
			parent_node->key[i] = temp->key[i];
		}

		parent_node->nkey = 2;
		grand_parent_key = temp->key[(N+1)/2];

		for(i=(N/2)+1; i<N+1; i++){
			parent_split_node->chi[i-(N/2)-1] = temp->chi[i];
			parent_split_node->chi[i-(N/2)-1]->parent = parent_split_node;
		}
		for(i=(N/2)+1; i<N; i++){
			parent_split_node->key[i-(N/2)-1] = temp->key[i];
		}

		free(temp);
		temp = NULL;

		parent_split_node->nkey = 1;

		insert_in_parent(parent_node, grand_parent_key, (DATA *)parent_split_node);

	}
}


void
insert(int key, DATA *data)
{
	NODE *leaf = NULL;
	NODE *split_leaf = NULL;
	TEMP *temp = NULL;
	int i;
	int parent_key;

	if (Root == NULL) {
		leaf = alloc_leaf(NULL);
		Root = leaf;
	}
  else {
    leaf = find_leaf(Root, key);
  }

	if (leaf->nkey < (N-1)) {
		insert_in_leaf(leaf, key, data);
	}
	else {
		temp = alloc_temp(NULL);

		for(i = 0; i<N-1; i++){
			temp->chi[i] = leaf->chi[i];
			temp->key[i] = leaf->key[i];
		}

		insert_in_temp_leaf(temp, key, data);

		split_leaf = alloc_leaf(NULL);

		split_leaf->chi[N-1] = leaf->chi[N-1];
		leaf->chi[N-1] = split_leaf;

		for(i = 0;i<N-1;i++){
			leaf->chi[i] = 0;
			leaf->key[i] = 0;
		}
		for(i = 0; i<(N/2); i++){
			leaf->chi[i] = temp->chi[i];
			leaf->key[i] = temp->key[i];
		}
		leaf->nkey = 2;

		for(i = (N/2); i<N; i++){
			split_leaf->chi[i-(N/2)] = temp->chi[i];
			split_leaf->key[i-(N/2)] = temp->key[i];
		}

		free(temp);
		temp = NULL;

		split_leaf->isLeaf = true;
		split_leaf->nkey = 2;

		parent_key = split_leaf->key[0];

		insert_in_parent(leaf, parent_key, (DATA *)split_leaf);

	}
}


int
search_key(NODE * node, int key)
{
	int kid;

	if (node->isLeaf){
		for(int i = 0; i < node->nkey; i++){
			if (key == node->key[i]){
				return 0;
			}
		}
		//失敗時
		return -1;
	}

	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return search_key(node->chi[kid], key);

}

NODE*
update(NODE* node, DATA* record)
{
	int kid;

	if (node->isLeaf){
		for(int i = 0; i < node->nkey; i++){
			if (node->key[i] == record->key){
				((DATA*)(node->chi[i]))->val = 2; //update
				return 0;
			}
		}

		return 0;
	}

	for (kid = 0; kid < node->nkey; kid++) {
		if (record->key < node->key[kid]) break;
	}

	return update(node->chi[kid], record);

}
