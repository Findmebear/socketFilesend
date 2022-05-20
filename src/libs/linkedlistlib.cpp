#include "linkedlist.h"

using namespace linkedlist;
LinkedList::LinkedList() {
    head = new Node();
    tail = new Node();
    this->head=NULL;
    this->tail=NULL;
    this->length=0;
}

LinkedList::~LinkedList() {
    Node* temp = this->head;
    while(temp!=NULL) {
        Node* t = temp;
        temp = temp->next;
        free(t);
    }
}

size_t LinkedList::insert(string key, File value)
{
    Node* node = new Node(key,value);
    if (node == NULL)
        return 1;
    if(this->head == NULL) {
        this->head = node;
        this->tail = node;
        this->length++;
    } 
    else {
        Node* current = this->head;
        while(current!=NULL) {
            if(current->key.compare(key)==0) {
                current->value = value;
                return 2;
            }
            else
                current = current->next;
        }
        this->tail->next  = node;
        this->tail 	= this->tail->next;
    }
    this->length++;	
    return 0;
}

Node* LinkedList::nodeAtindex(int index) {
    Node* current = this->head;
    for(int i =0;i<this->length;i++) {
        if(i==index)
            return current;
        else
            current = current->next;
    }
    return NULL;
}

File LinkedList::getValue(string key)
{
    Node* current = this->head;
    while(current!=NULL) {
        if(current->key.compare(key)==0) 
            return current->value;
        current = current->next;
    }
    return {{0},{0}};
}

bool LinkedList::containsValue(string key) {
    Node* current = this->head;
    for(int i=0;i<this->length;i++) {
        if(current->key.compare(key)==0)
            return true;
        current = current->next;
    }
    return false;
}

void LinkedList::printList() {
    size_t limit = this->length;
    if(!this->length)
        printf("Empty");
    Node* temp = this->head;
    for(size_t i = 0; i < limit; i++) {
        printf("%s->\n",temp->value.name.c_str());
        temp = temp->next;
    }
}