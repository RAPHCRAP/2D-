
#pragma once


#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <unordered_set>

using namespace std;

template <typename T>
class List
{
private:
    class Node
    {
    public:
        T data;
        Node* prev;
        Node* nxt;

        Node(const T& e) : data(e), prev(nullptr), nxt(nullptr) {}

        friend ostream& operator<<(ostream& os, const Node* curr)
        {
            os << "<-[ ";
            if (curr)
                os << curr->data;
            else
                os << "*";
            os << " ]->";
            return os;
        }
    };

    Node* head;
    Node* tail;



public:
    List() : head(nullptr), tail(nullptr) {}

    List(const T& e)
    {
        Node* temp = new Node(e);
        head = temp;
        tail = temp;
    }

    List(const List& other) : head(nullptr), tail(nullptr)
    {
        Node* curr = other.head;
        while (curr)
        {
            append(curr->data);
            curr = curr->nxt;
        }
    }

    void clear()
    {
        while (!isEmpty())
        {
            remove();
        }
    }

    ~List()
    {
        clear();
    }

    bool isEmpty() const
    {
        return head == nullptr;
    }

    void move(const T& e)
    {
        Node* temp = new Node(e);
        temp->nxt = head;
        if (head)
            head->prev = temp;
        else
            tail = temp;
        head = temp;
    }

    void append(const T& e)
    {
        Node* temp = new Node(e);
        if (isEmpty())
        {
            head = temp;
            tail = temp;
            return;
        }
        temp->prev = tail;
        tail->nxt = temp;
        tail = temp;
    }



    T remove()
    {
        if (isEmpty())
            return T();

        Node* toDel = head;
        head = head->nxt;

        if (head)
            head->prev = nullptr;
        else
            tail = nullptr;

        T ret = toDel->data;
        delete toDel;
        return ret;
    }

    T erase()
    {
        if (isEmpty())
            return T();

        Node* toDel = tail;

        if (head == tail)
        {

            head = nullptr;
            tail = nullptr;
        }
        else
        {
            tail = tail->prev;
            tail->nxt = nullptr;
        }

        T ret = toDel->data;
        delete toDel;
        return ret;
    }

    bool contains(const T& value) const
    {
        Node* temp = head;
        while (temp)
        {
            if (temp->data == value)
                return true;
            temp = temp->nxt;
        }
        return false;
    }

    void display() const
    {
        Node* curr = head;
        while (curr)
        {
            cout << curr;
            curr = curr->nxt;
        }
        cout << endl << "-" << endl;
    }

    T getTail() const
    {
        if (isEmpty())
            return T();
        return tail->data;
    }

    List& operator=(const List& other)
    {
        if (this == &other)
            return *this;


        clear();


        Node* curr = other.head;
        while (curr)
        {
            append(curr->data);
            curr = curr->nxt;
        }

        return *this;
    }


    class Iterator
    {
    private:
        Node* current;

    public:

        Iterator(const Iterator& othr) : current(othr.current) {}

        Iterator(Node* node) : current(node) {}

        T& operator*() { return current->data; }

        T* operator->() { return &current->data; }

        Iterator& operator++()
        {
            current = current->nxt;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator temp = *this;
            current = current->nxt;
            return temp;
        }

        Iterator& operator--()
        {
            current = current->prev;
            return *this;
        }

        Iterator operator--(int)
        {
            Iterator temp = *this;
            current = current->prev;
            return temp;
        }

        bool operator==(const Iterator& other) const { return current == other.current; }

        bool operator!=(const Iterator& other) const { return current != other.current; }

        Iterator operator+(int n)
        {
            Iterator temp = *this;
            while (n > 0 && temp.current != nullptr)
            {
                temp.current = temp.current->nxt;
                --n;
            }
            return temp;
        }

        Iterator operator-(int n)
        {
            Iterator temp = *this;
            while (n > 0 && temp.current != nullptr)
            {
                temp.current = temp.current->prev;
                --n;
            }
            return temp;
        }

        int operator-(const Iterator& other)
        {
            int distance = 0;
            Iterator temp = other;
            while (temp != *this)
            {
                temp.current = temp.current->nxt;
                ++distance;
            }
            return distance;
        }

        bool operator!() const
        {
            return !current;
        }
    };

    Iterator begin() const { return Iterator(head); }

    Iterator last() const { return Iterator(tail); }

    Iterator end() const { return Iterator(nullptr); }
};



template <typename T>
class Queue
{
private:
    List<T> list;
    int size;

public:
    Queue() : size(0) {}

    Queue(const Queue& othr)
    {
        size = othr.size;
        new (&list) Queue(othr.list);
    }

    ~Queue() {}

    bool enqueue(const T& e)
    {
        list.append(e);
        size++;
        return true;
    }

    bool dequeue(T& result)
    {
        if (isEmpty())
        {
            result = T();
            return false;
        }


        result = list.remove();
        size--;
        return true;
    }

    bool peek(T& result) const
    {
        if (isEmpty())
            return false;

        result = list.getTail();

        return true;
    }

    bool isEmpty() const
    {
        return size == 0;
    }

    int getSize() const
    {
        return size;
    }

    void display()
    {
        list.dispay();
    }
};


class IDManager 
{
    int nextId;
    int offSet;
    Queue<int> freeIds;
    std::unordered_set<int> inUse;

public:
    IDManager(int Id = 0, int offset = 1) : nextId(Id), offSet(offset) {}

    int generate() {
        int id;
        if (!freeIds.isEmpty()) {
            if (freeIds.dequeue(id)) {
                inUse.insert(id);
                return id;
            }
        }

        id = nextId;
        nextId += offSet;
        inUse.insert(id);
        return id;
    }

    void release(int id) {
        // Validate: only release if it was actually in use
        if (inUse.find(id) != inUse.end()) {
            inUse.erase(id);
            freeIds.enqueue(id);
        }
        else {
            std::cerr << "[IDManager] Warning: Attempt to release invalid or duplicate ID " << id << std::endl;
        }
    }
};

template <typename T>
class Stack
{
private:
    List<T> list;
    int size;

public:
    Stack() :size(0) {}

    ~Stack() {}

    bool isEmpty()
    {
        return size == 0;
    }


    void push(T e) {
        ++size;
        list.append(e);  
    }

    T pop() {
        if (isEmpty()) return T();
        --size;
        return list.erase(); 
    }


    T peek()
    {
        if (isEmpty())
        {
            return T();
        }


        return list.getTail();
    }
};

template <typename T>
class BT
{
    class Node
    {
    public:

        T data;
        size_t height;

        Node* left;
        Node* right;


        Node() : data(T()), height(1), left(nullptr), right(nullptr) {}

        Node(const T& e) : data(e), height(1), left(nullptr), right(nullptr) {}

        Node(Node& othr) : data(othr.data), height(1), left(nullptr), right(nullptr) {}

        friend ostream& operator << (ostream& os, const Node* curr)
        {
            if (curr)
                os << curr->data;
            else
                os << "-";


            return os;
        }



    };

    Node* root;

public:


    BT() : root(nullptr) {}

    void copy(Node* curr)
    {
        if (!curr)
        {
            return;
        }


        copy(curr->left);


        insert(curr->key, curr->data);


        copy(curr->right);
    }

    BT(const BT& othr)
    {
        root = nullptr;

        if (othr.root)
        {
            copy(othr.root);
        }
    }

    bool isEmpty()
    {
        return !root;
    }


    // INSERT

    bool insert(const T& e)
    {
        if (isEmpty())
        {
            root = new Node(e);
            return true;
        }

        Queue<Node*> q;

        q.enqueue(root);

        Node* curr;

        while (!q.isEmpty())
        {
            q.dequeue(curr);

            if (!curr->left)
            {
                curr->left = new Node(e);
                height();
                return true;
            }
            q.enqueue(curr->left);

            if (!curr->right)
            {
                curr->right = new Node(e);
                height();
                return true;
            }
            q.enqueue(curr->right);


        }


    }

    //HEIGHT

    size_t NodeHeight(Node* curr) const
    {
        if (!curr)
        {
            return 0;
        }

        size_t leftHeight = NodeHeight(curr->left);
        size_t rightHeight = NodeHeight(curr->right);

        curr->height = 1 + max(leftHeight, rightHeight);

        return curr->height;
    }

    size_t height() const
    {
        return NodeHeight(root);
    }

    // DISPLAY

    void preorderTraversal(Node* curr)
    {
        if (!curr)
        {
            return;
        }


        cout << curr->data << " ";
        preorderTraversal(curr->left);
        preorderTraversal(curr->right);
    }

    void inorderTraversal(Node* curr)
    {
        if (!curr)
        {
            return;
        }



        inorderTraversal(curr->left);

        cout << curr->data << " ";

        inorderTraversal(curr->right);
    }

    void postorderTraversal(Node* curr)
    {
        if (!curr)
        {
            return;
        }


        cout << curr->data << " ";
        postorderTraversal(curr->left);
        postorderTraversal(curr->right);
    }

    void display()
    {
        if (!isEmpty())
        {
            cout << endl << endl;
            inorderTraversal(root);
            cout << endl << endl;
        }
    }

    void print()
    {

        cout << endl << endl;

        if (isEmpty())
        {
            return;
        }

        Queue<Node*> q;

        q.enqueue(root);

        Node* curr;

        int TH = height();
        TH--;

        for (int i = 0; i <= TH; i++)
        {

            for (int sp = 0; sp < pow(2, TH - i) - 1; sp++)
            {
                cout << string(1, ' ');
            }

            for (int l = 0; l < pow(2, i); l++)
            {


                q.dequeue(curr);



                if (curr)
                {
                    cout << curr;

                    q.enqueue(curr->left);
                    q.enqueue(curr->right);
                }
                else
                {
                    cout << string(1, '_');

                    q.enqueue(nullptr);
                    q.enqueue(nullptr);
                }

                for (int sp = 0; sp < pow(2, TH - i + 1) - 1; sp++)
                {
                    cout << string(1, ' ');
                }
            }

            cout << endl;
        }

        cout << endl << endl;
    }



    //DESTRUCT

    void clear(Node*& curr)
    {
        if (!curr)
        {
            return;
        }

        clear(curr->left);
        clear(curr->right);


        delete curr;
        curr = nullptr;
    }

    ~BT()
    {
        clear(root);
    }


};


template <typename T>
class BST
{
    struct Node
    {
        T data;
        int key;
        int height;


        Node* right;
        Node* left;



        Node() : key(0), data(T()), height(1), right(nullptr), left(nullptr) {}

        Node(int key, T e) : key(key), data(e), height(1), right(nullptr), left(nullptr) {}

        Node(const Node& othr) : key(othr.key), data(othr.data), height(1), right(nullptr), left(nullptr) {}

        friend ostream& operator << (ostream& os, Node* curr)
        {
            if (curr)
            {
                cout << curr->data;
            }
            else
            {
                cout << "_";
            }

            return os;
        }

        ~Node()
        {

        }
    };

    Node* root;

public:

    BST() : root(nullptr) {}


    // COPY CONSTRUCT

    void copy(Node* curr)
    {
        if (!curr)
        {
            return;
        }


        copy(curr->left);


        insert(curr->key, curr->data);


        copy(curr->right);
    }

    BST(const BST& othr)
    {
        root = nullptr;

        if (othr.root)
        {
            copy(othr.root);
        }
    }


    // HELPER

    bool isEmpty() const
    {
        return !root;
    }

    //HEIGHT

    size_t NodeHeight(Node* curr)
    {
        if (!curr)
        {
            return 0;
        }

        size_t leftHeight = NodeHeight(curr->left);
        size_t rightHeight = NodeHeight(curr->right);

        curr->height = 1 + max(leftHeight, rightHeight);

        return curr->height;
    }

    size_t height() const
    {
        return NodeHeight(root);
    }


    // INSERT

    Node* insertNode(Node* curr, int key, const T& e)
    {
        if (curr == nullptr)
        {
            Node* newNode = new Node(key, e);
            return newNode;
        }

        if (key < curr->key)
        {
            curr->left = insertNode(curr->left, key, e);
        }
        else if (key > curr->key)
        {
            curr->right = insertNode(curr->right, key, e);
        }

        size_t leftHeight = (curr->left) ? curr->left->height : 0;
        size_t rightHeight = (curr->right) ? curr->right->height : 0;

        curr->height = 1 + max(leftHeight, rightHeight);

        return curr;
    }


    bool insert(int key, const T& e)
    {
        root = insertNode(root, key, e);
        return true;
    }


    //ROTATE 



    Node* rightRotate(Node* y)
    {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = 1 + max(NodeHeight(y->left), NodeHeight(y->right));
        x->height = 1 + max(NodeHeight(x->left), NodeHeight(x->right));

        return x;
    }

    Node* leftRotate(Node* x)
    {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = 1 + max(NodeHeight(x->left), NodeHeight(x->right));
        y->height = 1 + max(NodeHeight(y->left), NodeHeight(y->right));

        return y;
    }

    // TO AVL

    int getBalance(Node* node)
    {
        return node ? NodeHeight(node->left) - NodeHeight(node->right) : 0;
    }

    Node* balanceNode(Node* root)
    {
        root->height = 1 + max(NodeHeight(root->left), NodeHeight(root->right));
        int balance = getBalance(root);

        if (balance > 1)
        {
            if (getBalance(root->left) < 0)
            {
                root->left = leftRotate(root->left);
            }
            root = rightRotate(root);
        }
        else if (balance < -1)
        {
            if (getBalance(root->right) > 0)
            {
                root->right = rightRotate(root->right);
            }
            root = leftRotate(root);
        }

        return root;
    }

    Node* ToAVL(Node* root)
    {
        if (!root)
        {
            return nullptr;
        }

        root->left = ToAVL(root->left);
        root->right = ToAVL(root->right);

        root = balanceNode(root);

        return root;
    }


    bool Balance()
    {
        root = ToAVL(root);
        return true;
    }

    //DISPLAY 

    void preorderTraversal(Node* curr)
    {
        if (!curr)
        {
            return;
        }


        cout << curr->data << " ";
        preorderTraversal(curr->left);
        preorderTraversal(curr->right);
    }

    void inorderTraversal(Node* curr)
    {
        if (!curr)
        {
            return;
        }



        inorderTraversal(curr->left);

        cout << curr->data << " ";

        inorderTraversal(curr->right);
    }

    void postorderTraversal(Node* curr)
    {
        if (!curr)
        {
            return;
        }


        cout << curr->data << " ";
        postorderTraversal(curr->left);
        postorderTraversal(curr->right);
    }

    void display()
    {
        if (!isEmpty())
        {
            cout << endl << endl;
            inorderTraversal(root);
            cout << endl << endl;
        }
    }

    void print()
    {

        cout << endl << endl;

        if (isEmpty())
        {
            return;
        }

        Queue<Node*> q;

        q.enqueue(root);

        Node* curr;

        int TH = height();
        TH--;

        for (int i = 0; i <= TH; i++)
        {

            for (int sp = 0; sp < pow(2, TH - i) - 1; sp++)
            {
                cout << string(1, ' ');
            }

            for (int l = 0; l < pow(2, i); l++)
            {


                q.dequeue(curr);



                if (curr)
                {
                    cout << curr;

                    q.enqueue(curr->left);
                    q.enqueue(curr->right);
                }
                else
                {
                    cout << string(1, '_');

                    q.enqueue(nullptr);
                    q.enqueue(nullptr);
                }

                for (int sp = 0; sp < pow(2, TH - i + 1) - 1; sp++)
                {
                    cout << string(1, ' ');
                }
            }

            cout << endl;
        }

        cout << endl << endl;
    }

    // DELETE

    Node* findMax(Node* node)
    {
        while (node && node->right)
        {
            node = node->right;
        }
        return node;
    }

    Node* findMin(Node* node)
    {
        Node* curr = node;
        while (curr && curr->left)
        {
            curr = curr->left;
        }
        return curr;
    }

    Node* deleteNode(Node* curr, int key)
    {
        if (!curr)
        {
            return curr;
        }

        if (key < curr->key)
        {
            curr->left = deleteNode(curr->left, key);
        }

        else if (key > curr->key)
        {
            curr->right = deleteNode(curr->right, key);
        }

        else
        {

            if (!curr->left)
            {
                Node* temp = curr->right;
                delete curr;
                return temp;
            }
            else if (!curr->right)
            {
                Node* temp = curr->left;
                delete curr;
                return temp;
            }

            //CHOICE FOR TWO CHILDREN CASE

            if (NodeHeight(curr->left) > NodeHeight(curr->right))
            {
                Node* temp = findMin(curr->right);

                curr->key = temp->key;
                curr->data = temp->data;

                curr->right = deleteNode(curr->right, temp->key);
            }
            else
            {
                Node* predecessor = findMax(curr->left);
                curr->key = predecessor->key;
                curr->data = predecessor->data;
                curr->left = deleteNode(curr->left, predecessor->key);
            }
        }

        return curr;
    }

    bool contains(int key)
    {
        Node* current = root;

        while (current != nullptr)
        {
            if (key == current->key)
            {
                return true;
            }
            else if (key < current->key)
            {
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }

        return false;
    }

    bool Delete(int key)
    {
        root = deleteNode(root, key);
        NodeHeight(root);
        Balance();
        return true;
    }

    //DESTRUCT

    void clear(Node*& curr)
    {
        if (!curr)
        {
            return;
        }

        clear(curr->left);
        clear(curr->right);


        delete curr;
        curr = nullptr;
    }

    ~BST()
    {
        clear(root);
    }
};


template <typename T, typename U>
class AVL
{
public:
    struct Node
    {
        T key;
        U data;
        int height;
        Node* left;
        Node* right;

        Node(const T& k, const U& d) : key(k), data(d), height(1), left(nullptr), right(nullptr) {}
    };

    Node* root;

    int getHeight(Node* node) const
    {
        return node ? node->height : 0;
    }

    int getBalance(Node* node) const
    {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    void print()
    {

        cout << endl << endl;

        if (isEmpty())
        {
            return;
        }

        Queue<Node*> q;

        q.enqueue(root);

        Node* curr;

        int TH = height();
        TH--;

        for (int i = 0; i <= TH; i++)
        {

            for (int sp = 0; sp < pow(2, TH - i) - 1; sp++)
            {
                cout << string(1, ' ');
            }

            for (int l = 0; l < pow(2, i); l++)
            {


                q.dequeue(curr);



                if (curr)
                {
                    cout << curr->key;

                    q.enqueue(curr->left);
                    q.enqueue(curr->right);
                }
                else
                {
                    cout << string(1, '_');

                    q.enqueue(nullptr);
                    q.enqueue(nullptr);
                }

                for (int sp = 0; sp < pow(2, TH - i + 1) - 1; sp++)
                {
                    cout << string(1, ' ');
                }
            }

            cout << endl;
        }

        cout << endl << endl;
    }

 

    Node* rightRotate(Node* y) {
        if (!y || !y->left) return y;  //  prevent nullptr bugs


     

        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        std::cout << "[RIGHT ROTATE] at " << y->key << "\n";
        std::cout << "[LEFT ROTATE] at " << x->key << "\n";

        y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }



    Node* leftRotate(Node* x) {
        if (!x || !x->right) return x;  // prevent nullptr bugs

 


        Node* y = x->right;
        Node* T2 = y->left;

        std::cout << "[RIGHT ROTATE] at " << y->key << "\n";
        std::cout << "[LEFT ROTATE] at " << x->key << "\n";

        y->left = x;
        x->right = T2;

        x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }



    Node* insert(Node* node, const T& key, const U& data) {
       


        if (!node)
            return new Node(key, data);

        if (key < node->key) {
            node->left = insert(node->left, key, data);
        }
        else if (key > node->key) {

            node->right = insert(node->right, key, data);
        }
        else {

            node->data = data;
        }

        // Update height
        node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;

        // Get balance factor
        int balance = getBalance(node);


        //// DEBUG LOGGING
        //std::cout << "[BALANCE] Key = " << node->key << ", Balance = " << balance << "\n";

        // Left Left Case
        if (balance > 1 && key < node->left->key)
            return rightRotate(node);

        // Right Right Case
        if (balance < -1 && key > node->right->key)
            return leftRotate(node);

        // Left Right Case
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right Left Case
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }


    Node* deleteNode(Node* root, const T& key)
    {
        // ... (existing deletion code until height update)

        // Update height
        root->height = 1 + std::max(getHeight(root->left), getHeight(root->right));

        int balance = getBalance(root);

        // Left Left Case
        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);

        // Left Right Case
        if (balance > 1 && getBalance(root->left) < 0)
        {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }

        // Right Right Case
        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);

        // Right Left Case
        if (balance < -1 && getBalance(root->right) > 0)
        {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
    }

    Node* minValueNode(Node* node) const
    {
        Node* current = node;
        while (current && current->left)
            current = current->left;
        return current;
    }

    

    void clear(Node* node)
    {
        if (!node)
            return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    Node* copyTree(Node* other) const
    {
        if (!other)
            return nullptr;
        Node* newNode = new Node(other->key, other->data);
        newNode->left = copyTree(other->left);
        newNode->right = copyTree(other->right);
        newNode->height = other->height;
        return newNode;
    }

    void printInOrder(Node* node) const
    {
        if (!node)
            return;
        printInOrder(node->left);
        std::cout << "Key: " << node->key << ", Data: " << node->data << std::endl;
        printInOrder(node->right);
    }

    Node* find(Node* node, const T& key) const
    {
        if (!node)
            return nullptr;
        if (key < node->key)
            return find(node->left, key);
        else if (key > node->key)
            return find(node->right, key);
        else
            return node;
    }

public:
    AVL() : root(nullptr) {}

    AVL(const AVL& other)
    {
        root = copyTree(other.root);
    }

    ~AVL()
    {
        clear();
    }

    void insert(const T& key, const U& data)
    {
        root = insert(root, key, data);
    }

    bool remove(const T& key)
    {
        if (!contains(key))
            return false;
        root = deleteNode(root, key);
        return true;
    }

    void clear()
    {
        clear(root);
        root = nullptr;
    }

    bool isEmpty() const
    {
        return root == nullptr;
    }

    size_t height() const
    {
        return getHeight(root);
    }

    bool contains(const T& key) const
    {
        return find(root, key) != nullptr;
    }

    U& get(const T& key)
    {
        Node* node = find(root, key);
        if (!node)
            throw std::out_of_range("Key not found");
        return node->data;
    }

    const U& get(const T& key) const
    {
        Node* node = find(root, key);
        if (!node)
            throw std::out_of_range("Key not found");
        return node->data;
    }

    void displayInOrder() const
    {
        printInOrder(root);
    }

    void printTree(Node* node, int indent = 0) const {
        if (!node) return;

        for (int i = 0; i < indent; ++i) std::cout << "   ";
        std::cout << node->key << "\n";

        if (node->left || node->right) {
            printTree(node->left, indent + 1);
            printTree(node->right, indent + 1);
        }
    }


    void displayTree() const {
        std::cout << "=== TREE STRUCTURE ===\n";
        printTree(root);
        std::cout << "======================\n";
    }
};


template <typename T>
class minHeap
{
private:
    List<T> heap;

    void heapifyUp()
    {
        auto iter = heap.end();
        --iter;

        while (iter != heap.begin() && *iter < *(iter - 1))
        {
            swap(*iter, *(iter - 1));
            --iter;
        }
    }

    void heapifyDown()
    {
        auto iter = heap.begin();
        auto next = iter + 1;

        while (next != heap.end() && *iter > *next)
        {
            swap(*iter, *next);
            ++iter;
            ++next;
        }
    }

public:
    void insert(const T& value)
    {
        heap.append(value);
        heapifyUp();
    }

    T extractMin()
    {
        if (heap.isEmpty())
            throw runtime_error("Heap is empty.");

        T minValue = *heap.begin();
        heap.remove();
        heapifyDown();
        return minValue;
    }

    bool isEmpty() const
    {
        return heap.isEmpty();
    }

    void print() const
    {
        heap.display();
    }
};

template <typename T>
class maxHeap
{
private:
    List<T> heap;

    void heapifyUp()
    {
        auto iter = heap.end();
        --iter;

        while (iter != heap.begin() && *iter > *(iter - 1))
        {
            swap(*iter, *(iter - 1));
            --iter;
        }
    }

    void heapifyDown()
    {
        auto iter = heap.begin();
        auto next = iter + 1;

        while (next != heap.end() && *iter < *next)
        {
            swap(*iter, *next);
            ++iter;
            ++next;
        }
    }

public:
    void insert(const T& value)
    {
        heap.append(value);
        heapifyUp();
    }

    T extractMax()
    {
        if (heap.isEmpty())
            throw runtime_error("Heap is empty.");

        T maxValue = *heap.begin();
        heap.remove();
        heapifyDown();
        return maxValue;
    }

    bool isEmpty() const
    {
        return heap.isEmpty();
    }

    void print() const
    {
        heap.display();
    }
};

template <typename T>
class Hash
{
private:
    struct Entry
    {
        int key;
        T value;

        Entry(int k = 0, const T& v = T()) : key(k), value(v) {}



        Entry(const Entry& other) : key(other.key), value(other.value) {}

        friend ostream& operator<<(ostream& os, const Hash<T>::Entry& entry)
        {
            os << "Key: " << entry.key << ", Value: " << entry.value;
            return os;
        }
    };

    List<Entry>* table;
    int size;
    int count;


    int hashFunction(int key) const
    {
        return key % size;
    }

    int quadraticProbing(int index, int attempt) const
    {
        return (index + attempt * attempt) % size;
    }

    void rehash()
    {
        int oldSize = size;
        List<Entry>* oldTable = table;

        size *= 2;
        table = new List<Entry>[size];
        count = 0;

        for (int i = 0; i < oldSize; ++i)
        {
            for (auto it = oldTable[i].begin(); !(!it); ++it)
            {
                insert(it->key, it->value);
            }
        }

        delete[] oldTable;
    }

public:

    Hash(int initialSize = 16) : size(initialSize), count(0)
    {
        table = new List<Entry>[size];
    }

    void clear()
    {
        for (int i = 0; i < size; i++)
        {
            table[i].clear();
        }

        delete[] table;
    }

    ~Hash()
    {
        clear();
    }

    void insert(int key, const T& e)
    {
        if (count >= size / 2)
        {
            rehash();
        }

        int index = hashFunction(key);
        int attempt = 0;


        while (!table[index].isEmpty())
        {


            auto it = table[index].begin();

            if (it->key == key)
            {
                table[index].append(Entry(key, e));
                ++count;
                return;
            }

            index = quadraticProbing(index, ++attempt);
        }


        table[index].append(Entry(key, e));
        ++count;


    }


    void display() const
    {
        for (int i = 0; i < size; i++)
        {
            cout << "{" << i << "} : ";
            table[i].display();
            cout << endl;
        }
    }
};

class Graph
{
private:
    int size;
    List<int>* adjList;
    bool* visited;

public:
    Graph(int nodes)
    {
        size = nodes;
        adjList = new List<int>[size];
        visited = new bool[size];
        fill(visited, visited + size, false);
    }

    void addEdge(int src, int dest)
    {
        adjList[src].append(dest);
        adjList[dest].append(src);
    }

    void resetVisited()
    {
        fill(visited, visited + size, false);
    }

    void DFS(int start)
    {
        visited[start] = true;
        cout << "NODE -> " << start << endl;


        for (auto it = adjList[start].begin(); it != adjList[start].end(); ++it)
        {
            if (!visited[*it])
            {
                DFS(*it);
            }
        }
    }

    void BFS(int start)
    {
        resetVisited();
        List<int> queue;
        queue.append(start);
        visited[start] = true;

        while (!queue.isEmpty())
        {
            int current = queue.remove();
            cout << "NODE => " << current << endl;

            for (auto it = adjList[current].begin(); it != adjList[current].end(); ++it)
            {
                if (!visited[*it])
                {
                    queue.append(*it);
                    visited[*it] = true;
                }
            }
        }
    }

    int getSize() const
    {
        return size;
    }

    void display() const
    {
        for (int i = 0; i < size; ++i)
        {
            cout << "{ NODE \"" << i << "\" } : ";
            adjList[i].display();
        }
    }

    ~Graph()
    {
        for (int i = 0; i < size; i++)
        {
            adjList[i].clear();
        }

        delete[] adjList;
        delete[] visited;
    }
};