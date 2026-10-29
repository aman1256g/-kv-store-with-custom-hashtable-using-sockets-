#include<bits/stdc++.h>
using namespace std;

//  a hamara htab table hai jo ki sari nodes ko store karke rakhega 

struct Node{
    Node *next= nullptr;
    uint64_t hcode=0;
    string key;
    string value;
};

struct Table{
Node **buckets= nullptr;
size_t capacity =0;
size_t size=0;
};

Node* lookup(Table *table , Node *key_node){
    if(!table->buckets || table->capacity == 0){
        return nullptr;
    }

    size_t idx= key_node->hcode % table->capacity;
    Node *curr= table->buckets[idx];


    while(curr != nullptr){
        if(curr->hcode == key_node->hcode && curr->key == key_node->key){
            return curr;

        }
        curr=curr->next;
    }
return nullptr;
}






void Insert(Table *table , Node *node){
    size_t idx=node->hcode % table->capacity;
    node->next = table->buckets[idx];
    table->buckets[idx]=node;
    table->size++;

}











void table_init(Table *table , size_t capacity){
    table->capacity = capacity;
    table->size=0;

    table->buckets = new Node*[capacity]();
}


void free_table(Table *table ){
    for(int i=0;i<table->capacity;i++){
        if(!table->buckets)
        return ;
        Node *curr =table->buckets[i];
while(curr != nullptr){
    Node *next_node=curr->next;
    delete curr; 
    curr=next_node;
}
    }

    delete [] table->buckets;
table->buckets=nullptr;
table->size=0;
table->capacity=0;
    }





    
inline uint64_t hash_string(string &str){
uint64_t hash=5831;
for(char c : str){
    hash=((hash << 5) + hash) + c;
}
return hash;
}

