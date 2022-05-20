#include <functional>
#include "linkedlist.h"
#include <string>
using namespace linkedlist;
class HashMap {
  private:
    linkedlist::LinkedList** array;
    unsigned long prehash(string key);

  public:
    size_t size;
    HashMap();
    HashMap(size_t size);
    ~HashMap();
    unsigned long hash(string key);
    bool insert(string key,File value);
    File get(string key);
    bool contains(string key);
    void resize(size_t new_size);
    size_t len();
    size_t capacity();
    void print();
};
