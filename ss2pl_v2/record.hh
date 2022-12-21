#pragma once

#include "rwlock.hh"

typedef struct _DATA {
	int key;
	int val;
	RWLock lock;
	struct _DATA *next;
	
} DATA;