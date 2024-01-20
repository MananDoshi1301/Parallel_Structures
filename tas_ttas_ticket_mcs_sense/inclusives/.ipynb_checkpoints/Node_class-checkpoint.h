class Node{
    public:
        atomic <bool> wait;
        atomic <Node*> next;
};