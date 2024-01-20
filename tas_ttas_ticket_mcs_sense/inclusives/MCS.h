#include "Node_class.h"

// atomic <Node*> tail = nullptr;

void acquire(Node* &myNode, atomic <Node*>& tail){
    
    Node* oldTail = tail.load(SEQ);
    
    // (myNode->next).store(NULL, SEQ);    
        
    while(!tail.compare_exchange_strong(oldTail, myNode, SEQ)){
        oldTail = tail.load(SEQ);
    }
    
    if(oldTail != nullptr){
        (myNode->wait).store(true, RELAX);
        (oldTail->next).store(myNode, SEQ);
        while((myNode->wait).load(SEQ)){}
    }
}

void release(Node* &myNode, atomic <Node*>& tail){
    // Check if theres no one after us using cas    
    Node* m = myNode;
    if(tail.compare_exchange_strong(m, nullptr, SEQ)){}
    else{
        while((myNode->next).load(SEQ) == NULL){}
        Node * p = myNode->next;
        (p->wait).store(false, SEQ);
    }
}