#include "hashmap.h"

HashMap::HashMap(size_t size) {
  this->array = new linkedlist::LinkedList*[size];
  for(int i =0;i<size;i++)
  {
    this->array[i] = new linkedlist::LinkedList();
  }
  this->size=size;
}


HashMap::HashMap() {
  this->array = new linkedlist::LinkedList*[10];
  for(int i =0;i<10;i++)
    this->array[i] = new linkedlist::LinkedList();
  this->size=10;
}


HashMap::~HashMap() {
  for(int i = 0;i<this->size;i++)
    array[i]->~LinkedList();
}


unsigned long HashMap::hash(string key) {
  unsigned long pre = this->prehash(key);
  unsigned long s = pre%this->size;
  return s;
}
unsigned long HashMap::prehash(string key) {
    int s = key.size();
    unsigned long total = 0;
    for(char c : key)
      total += c;
    return total;
}

bool HashMap::insert(string key, File value) {
  size_t result;
  unsigned long hashed = hash(key);
  result = this->array[hashed]->insert(key,value);
  int buckets = 0;
  for(int i =0;i<this->size;i++) {
    if(this->array[i]->length>0) {
      buckets++;
    }
  }
  double loadfactor = (double)(buckets)/(double)(this->size);
  if(loadfactor>=0.70)
    this->resize(this->size*2);
  if(result==2)
    return true;
  return false;
}

File HashMap::get(string key) {
  unsigned long hashed = hash(key);
  File res = this->array[hashed]->getValue(key); //the LinkedList function would populate with {0, 0}
  if(res.name != key) {                        //if the file does not exist
    std::cout << "File could not be located, you may want to send it first \n";
    res.name = "nothing";
  }
  return res;
}

bool HashMap::contains(string key) {
  unsigned long hashed = hash(key);
  bool res = this->array[hashed]->containsValue(key);
  return res;
}

void HashMap::resize(size_t new_size) {
  linkedlist::LinkedList** old = this->array;
  linkedlist::Node* element[this->len()];
  size_t limit = this->size;
  size_t currentindex = 0;
  for(size_t i =0;i<limit;i++) {
    if(this->array[i]->length>0){
      for(int j = 0;j<this->array[i]->length;j++) {
        element[currentindex++] = this->array[i]->nodeAtindex(j);
      }
    }
  }
  this->size = new_size;
  this->array = new linkedlist::LinkedList*[new_size];
  for(int i =0;i<new_size;i++) {
    this->array[i] = new linkedlist::LinkedList();
  }
  for(int i =0;i<currentindex;i++) {
    this->insert(element[i]->key,element[i]->value);
  }
  for(int i = 0;i<limit;i++) {
    old[i]->~LinkedList();
  }
}


size_t HashMap::len() {
  size_t len = 0;
  for(int i =0;i<this->size;i++) {
    len += this->array[i]->length;
  }
  return len;
}


size_t HashMap::capacity(){
 return (size_t)(0.7*(double)this->size) - 1;
}

void HashMap::print() {
  for(int i =0;i<this->size;i++) {
    printf("List %d: ",i+1);
    this->array[i]->printList();
    printf("\n");
  }
}