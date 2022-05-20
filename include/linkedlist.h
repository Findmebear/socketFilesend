#include "llnode.h"

namespace linkedlist{
    class LinkedList {
    private:
        linkedlist::Node* head;
        linkedlist::Node* tail;
    public:
        size_t length;
        LinkedList();
        ~LinkedList();
        size_t insert(string key, File value);
        File getValue(string key);
        Node* nodeAtindex(int index);
        void printList();
        bool containsValue(string key);
    }; 
} 
