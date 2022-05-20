#include "llnode.h"

using namespace linkedlist;
Node::Node() {
    this->key = "";
    this->value = {{0},{0}}; //declaring the empty file struct
    this->next = NULL;
}

Node::Node(string key,File value) {
    this->key = key;
    this->value = value;
    this->next = NULL;
}
Node::~Node()
{

}