#ifndef SYNC_H
#define SYNC_H

#include "storage.h"

void init_node_sync(Node* node);


int rdlock_node(Node* node);

int wrlock_node(Node* node);

int unlock_node(Node* node);


int destroy_node_sync(Node* node);

#endif