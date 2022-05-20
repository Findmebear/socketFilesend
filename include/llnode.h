#include <pack109.hpp>

namespace linkedlist {
    class Node {
        public:
            string key;
            File value;
            size_t length;
            linkedlist::Node* next; // pointer to the next Node
            Node(std::string key, File value); // hashmap now contains string key and File value
            Node();
            ~Node();
    };
}