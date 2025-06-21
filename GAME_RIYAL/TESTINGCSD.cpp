//#include <iostream>
//#include <string>
//#include <algorithm> 
//#include "CDS.h"
//#include <stack>
//// For std::max
//using namespace std;
//
//// Include your full AVL class code here
//// ... (copy your AVL class above this main)
//int main() {
//    AVL<float, std::string> tree;
//
//    // Insert two keys: 500 (closer) and 600 (farther)
//    tree.insert(500.0f, "object_500");
//    tree.insert(400.0f, "object_600");
//
//    // Confirm AVL tree structure
//    tree.print();
//
//    // === Reverse In-Order Traversal: R  N  L ===
//    std::stack<AVL<float, std::string>::Node*> nodeStack;
//    auto* current = tree.root;
//
//    cout << "\nTraversing from farthest to closest:\n";
//    cout << fixed << setprecision(3);
//
//    while (current || !nodeStack.empty()) {
//        // Go as far right as possible
//        while (current) {
//            nodeStack.push(current);
//            current = current->right;
//        }
//
//        // Visit node
//        current = nodeStack.top();
//        nodeStack.pop();
//
//        cout << "Key: " << current->key << ", Data: " << current->data << '\n';
//
//        // Then go left
//        current = current->left;
//    }
//
//    return 0;
//}