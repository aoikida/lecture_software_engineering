#include "bptree.h"
#include <vector>
#include <sys/time.h>

NODE *alloc_leaf(NODE *parent);
NODE *insert_in_leaf(NODE *leaf, int key, DATA *data);


struct timeval
cur_time(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t;
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
find_leaf(NODE *node, int key)
{
	int kid;

	if (node->isLeaf) return node;
	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return find_leaf(node->chi[kid], key);
}

NODE *
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

	return leaf;
}

TEMP *
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

	return leaf;
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

NODE *
insert_in_parent(NODE *leaf, int parent_key, NODE *new_leaf)
{	
	NODE *parent_node = NULL;

	if (leaf == Root) {
		parent_node = alloc_leaf(NULL);
		parent_node->isLeaf = false;
		parent_node->chi[0] = leaf;
		parent_node->chi[1] = new_leaf;
		parent_node->key[0] = parent_key;
		parent_node->nkey = 1;
		Root = parent_node;
		leaf->parent = Root;
		new_leaf->parent = Root;

		return parent_node;
	}

	parent_node = Root;
	new_leaf->parent = Root;

	if (parent_node->nkey < N - 1){
		insert_in_internal(parent_node, parent_key, (DATA *)new_leaf);
	}

	else { //以降はclass_05にて

	}
	return parent_node;	
}

NODE *
alloc_leaf(NODE *parent)
{
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = true;
	node->parent = parent;
	node->nkey = 0;

	return node;
}

void 
insert(int key, DATA *data)
{
	NODE *leaf;

	if (Root == NULL) {
		leaf = alloc_leaf(NULL);
		Root = leaf;
	}
  	else {
    	leaf = find_leaf(Root, key);
  	}

	if (leaf->nkey < N-1) {
		insert_in_leaf(leaf, key, data);
	}
	else {
		NODE *new_leaf = alloc_leaf(NULL);
		TEMP *temp_block = (TEMP*)alloc_leaf(NULL);

		for(int i = 0; i < N - 1; i++){
			temp_block->chi[i] = leaf->chi[i];
			temp_block->key[i] = leaf->key[i];
		}

		temp_block->nkey = 3;

		insert_in_temp_leaf(temp_block, key, data);

		new_leaf->chi[N - 1] = leaf->chi[N - 1];
		leaf->chi[N - 1] = new_leaf;

		for(int i = 0; i < N - 1; i++){
			leaf->chi[i] = 0;
			leaf->key[i] = 0;
		}
		leaf->nkey = 2;

		for(int i = 0; i < (N/2); i++){
			leaf->chi[i] = temp_block->chi[i];
			leaf->key[i] = temp_block->key[i];
		}

		for(int i = (N/2); i < N; i++){
			new_leaf->chi[i-(N/2)] = temp_block->chi[i];
			new_leaf->key[i-(N/2)] = temp_block->key[i];
		}
		new_leaf->nkey = 2;
		new_leaf->isLeaf = true;
		
		free(temp_block);
		temp_block = NULL;

		int parent_key = new_leaf->key[0];

		insert_in_parent(leaf, parent_key, new_leaf);

	}
}

void
init_root(void)
{
	Root = NULL;
}

int 
interactive()
{
  int key;

  std::cout << "Key: ";
  std::cin >> key;

  return key;
}

int
main(int argc, char *argv[])
{
  struct timeval begin, end;

	init_root();

	printf("-----Insert-----\n");
	begin = cur_time();
  	while (true) {
		insert(interactive(), NULL);
    	print_tree(Root);
  	}
	end = cur_time();

	return 0;
}
