#include <iostream>
#include <deque>
#include <iterator>
#include <unordered_set>
#include <algorithm>
using KEY_TYPE = int;

class Btree_node{
    public:
        std::deque<KEY_TYPE> keys;
        std::deque<Btree_node*>children;
        bool isleaf;
        Btree_node(bool leaf):isleaf(leaf){} //judge it leaf?
};

class Btree{
    private:
        Btree_node*root;
        int M; //what LEVEL b_tree
    public:
        Btree(int level):M(level)
        {
            root = new Btree_node(true); 
            root -> children.push_back(NULL);
        }
        
        void insertKey(KEY_TYPE key);
        void splitChildren(Btree_node*node, KEY_TYPE key);
        void insertNoFull(Btree_node*cur, KEY_TYPE key);
        void deleteKey(KEY_TYPE key);
        void deleteRe(Btree_node*cur, KEY_TYPE key);
        void print();
};

void Btree::insertKey(KEY_TYPE key)
{
    if(root -> keys.size() == M-1) // if root is full
    {
        Btree_node*new_node = new Btree_node(false); // new root
        new_node -> children.push_front(root); // root push to children
        splitChildren(new_node,0); //split children
        root = new_node; 
    }
    insertNoFull(root, key); 
}

// cur's children is full so split
void Btree::splitChildren(Btree_node*cur, int index)
{
    Btree_node* child = cur -> children[index]; //let the child is cur's children the order is index
    cur -> keys.insert(cur -> keys.begin()+index, child -> keys[(M-1)/2]); //cur's keys insert in brgin+index and the value is child's middle

    Btree_node*child_right = new Btree_node(child -> isleaf); //want to slipt right parts
    for(int i = 0; i < ((M-1)/2); i++) 
    {
        child_right -> keys.push_front(child -> keys.back()); //childe_right push the child's end to middle from head 
        child -> keys.pop_back(); //child pop the value
    }

    child -> keys.pop_back(); // child's pop the middle value

    for(int i = 0; i<(M/2); i++) //it is stitching the children
    {
        child_right -> children.push_front(child -> children.back()); //let children'send stitch to children's head 
        child -> children.pop_back();
    }
    cur -> children.insert(cur -> children.begin() + index + 1,child_right); //cur's children's push child_riht
}

//if cur is not full find a position insert
void Btree::insertNoFull(Btree_node*cur, KEY_TYPE key)
{
    if(cur -> isleaf) // the recursive's end  it's be full because it will judge whether it'is split 
    {
        int i =0; //remeber the position
        for(; i < cur -> keys.size(); ++i) // ++i need to find position  
            {
                if(key == cur -> keys[i]) //is repeat return
                    return;
                if(key < cur -> keys[i])
                    break;
            }
        cur -> keys.insert(cur -> keys.begin()+i,key); //insert
        cur -> children.insert(cur -> children.begin() + i, nullptr); // because we insert the new node so we also need insert the pointer to its children
        return;
    }

    int i = 0;
    for(; i < cur -> keys.size(); ++i)
    {
        if(key == cur -> keys[i])
            return;
        if(key < cur -> keys[i])
            break;
    }
    //judge children whether full
    if(cur -> children[i] -> keys.size() == M-1)
    {
        splitChildren(cur, i); //split children middle is up
        if(key > cur -> keys[i]) //resume to judge the position
            ++i;
    }
    
    insertNoFull(cur -> children[i] , key); //recursive
}

void Btree::deleteKey(KEY_TYPE key)
{
    if(root -> keys.empty()) 
        return;
    else
        deleteRe(root , key);
}

void Btree::deleteRe(Btree_node*cur, KEY_TYPE key)
{
    if(cur -> isleaf) //if it is leaf ,and find the node's poisition whitch want to delete
    {
        int i = 0;
        for(; i<cur -> keys.size(); ++i)
        {
            if(key == cur -> keys[i])
                break;
        }

        if(i == cur -> keys.size()) //the node is not inside 
        {
            std::cout <<"it not inside the tree";
            return;
        }

        cur -> keys.erase(cur -> keys.begin() +i);
        return;
    }
        //if cur is not in the leaf
    int i = 0;  
    for(; i < cur -> keys.size(); ++i) //find the keys position not the children
    {
        if(key <= cur -> keys[i]) //if key is keys[i]'s children and it always smaller than the keys[i+1]
            {
                break;
            }    
    }

    if(i == cur -> keys.size()) //  biger than all keys so it maybe in the right keys's children
    {
        --i; // at the large keys find the children
        Btree_node*left = cur -> children[i]; 
        Btree_node*right = cur -> children[i+1];

        if(right -> keys.size() == (M-1)/2) //if right's children is not enough
        {
            if(left -> keys.size() > (M-1)/2)// if left is enough
            {
                right -> keys.push_front(cur -> keys[i]); //right push parent'keys i now it is the rightmost
                cur -> keys.erase(cur -> keys.begin()+i); //delete parent's keys 
                cur -> keys.insert(cur -> keys.begin()+i , left -> keys.back());//let left last key pop and insert to cur's key
                left -> keys.pop_back();//left delete the node
                right -> children.push_front(left -> children.back()); //let children move with keys
                left -> children.pop_back();
                deleteRe(right,key);
            }
            else //left children is not enough
            {
                right -> keys.push_front(cur -> keys[i]); //right push parent's keys
                cur -> keys.erase(cur -> keys.begin()+i); //keys remove 
                
                for(int index =0; index < (M-1)/2; ++index)  //all left children into right children * < *
                {
                    right -> keys.push_front(left -> keys.back());
                    left -> keys.pop_back();
                    right -> children.push_front(left -> children.back());
                    left-> children.pop_back();
                }
                right -> children.push_front(left -> children.back()); //children more than keys
                left-> children.pop_back();

                cur -> children.erase(cur -> children.begin()+i); //delete cur's children

                delete left;

                if(cur == root && cur -> keys.empty())
                {
                    root = right;
                    delete cur;
                }
                
                deleteRe(right,key);
            }
        }
        else //if keys is more than (M-1)/2
        {
            deleteRe(right,key);
        }
    }
    else if(key != cur -> keys[i]) //keys small than all keys
    {
        Btree_node*left = cur -> children[i];
        Btree_node*right = cur -> children[i+1];

        if(left -> keys.size() == (M-1)/2)
        {
            if(right -> keys.size() > (M-1)/2)
            {
                left -> keys.push_back(cur -> keys[i]); //push the middle
                cur -> keys.erase(cur -> keys.begin() + i);
                cur -> keys.insert(cur -> keys.begin() + i, right -> keys.front());
                right -> keys.pop_front();
                left -> children.push_back(right -> children.front());
                right -> children.pop_front();
                deleteRe(left,key);
            }
            else 
            {
                right -> keys.push_front(cur -> keys[i]);
                cur -> keys.erase(cur -> keys.begin()+i);

                for(int index = 0; index <(M-1)/2; ++index)
                {
                    right -> keys.push_front(left -> keys.back());
                    left -> keys.pop_back();
                    right ->children.push_front(left -> children.back());
                    left -> children.pop_back();
                }
                right -> children.push_front(left -> children.back());
                left -> children.pop_back();
                
                cur -> children.erase(cur -> children.begin()+i);

                delete left;

                if(cur == root && cur -> keys.empty()) //cur is root and cur down
                {
                    root = right;
                    delete cur;
                }

                deleteRe(right,key);
            }
        }
        else
        {
            deleteRe(left, key);
        }
    }
    else //find the key in the cur 
    {
        Btree_node*left = cur -> children[i];
        Btree_node*right = cur -> children[i+1];

        if(left -> keys.size() > (M-1) / 2) //if keys is enough find the bigger number in the left children to replace the key
        {
            cur -> keys[i] = left -> keys.back();
            left -> keys.pop_back();
            return;
        }

        if(right -> keys.size() > (M-1)/2) // if keys is enough find the smallest number in right children to replace the key
        {
            cur -> keys[i] = right -> keys.front();
            right -> keys.pop_front();
            return;
        }
        
        right -> keys.push_front(cur -> keys[i]);
        cur -> keys.erase(cur -> keys.begin()+i);

        for(int index = 0; index < (M-1)/2; ++index) //left children push in right children
        {
            right -> keys.push_front(left -> keys.back());
            left -> keys.pop_back();
            right -> children.push_front(left -> children.back());
            left -> children.pop_back();
        }
        right -> children.push_front(left -> children.back());
        left -> children.pop_back();

        cur -> children.erase(cur ->children.begin()+i);
        
        delete left;

        if(cur == root && cur -> keys.empty()) 
        {
            root = right;
            delete cur;
        }

        deleteRe(right,key);
    }
}

void Btree::print()
{
    std::deque<Btree_node*>que;
    que.push_back(root);
    while(!que.empty())
    {
        int size = (int)que.size();
        for(int i = 0; i < size; ++i)
        {
            Btree_node *temp = que.front();
            que.pop_front();
            std::cout<<"[";
            for(auto keys : temp -> keys)
            {
                std::cout<<keys<< " ";
            }
            std::cout<<"]";
            for(auto child : temp -> children)
            {
                if(child != nullptr)
                {
                    que.push_back(child);
                }
            }
        }
        std::cout<<std::endl;
    }
}

int main()
{
    std::unordered_set<KEY_TYPE> kset; 
    while(kset.size()<26)
    {
        KEY_TYPE rand_ele = ( KEY_TYPE )rand()%10000;
        if(kset.count(rand_ele) > 0)
        {
            continue;
        }
        kset.insert(rand_ele);
    }
    std::deque<KEY_TYPE> arr(kset.begin(),kset.end());
    Btree bt(6);
    for(auto key:arr)
    {
        std::cout<<"insert"<<key<<std::endl;
        bt.insertKey(key);
        bt.print();
    }

    std::random_shuffle(arr.begin(),arr.end());//test the random number insert 
    for(auto key:arr)
    {
        std::cout<<"delete"<<key<<std::endl;
        bt.deleteKey(key);
        bt.print();
    }
    return EXIT_SUCCESS;
}