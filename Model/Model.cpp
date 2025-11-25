#include "model.h"
#include <stdio.h>
#include <string.h>

void initializeTree(FamilyTree* tree) {
    tree->root = NULL;
    tree->size = 0;
}

TreeNode* createTreeNode(Person p) {
    TreeNode* newNode = new TreeNode;
    newNode->person = p;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void destroyTree(TreeNode* root) {
    if (root == NULL) return;
    destroyTree(root->left);
    destroyTree(root->right);
    delete root;
}

TreeNode* findPerson(TreeNode* root, int id) {
    if (root == NULL) return NULL;
    if (root->person.id == id) return root;
    
    TreeNode* found = findPerson(root->left, id);
    if (found != NULL) return found;
    
    return findPerson(root->right, id);
}

TreeNode* findFather(TreeNode* root, int id_father) {
    if (root == NULL) return NULL;
    if (root->person.id == id_father) return root;
    
    TreeNode* found = findFather(root->left, id_father);
    if (found != NULL) return found;
    
    return findFather(root->right, id_father);
}

int addChild(TreeNode* father, TreeNode* child) {
    if (father == NULL || child == NULL) return 0;
    
    if (father->left == NULL) {
        father->left = child;
        return 1;
    } else if (father->right == NULL) {
        father->right = child;
        return 1;
    }
    return 0;
}

int buildFamilyTree(FamilyTree* tree, Person* people, int count) {
    if (count == 0) return 0;
    
    // Encontrar la raíz (primer rey, el que no tiene padre)
    TreeNode* root = NULL;
    for (int i = 0; i < count; i++) {
        if (people[i].id_father == -1 || people[i].id_father == 0) {
            root = createTreeNode(people[i]);
            tree->root = root;
            break;
        }
    }
    
    if (root == NULL) return 0;
    
    int added[MAX_PEOPLE] = {0};
    int rootIndex = -1;
    
    for (int i = 0; i < count; i++) {
        if (people[i].id == root->person.id) {
            added[i] = 1;
            rootIndex = i;
            break;
        }
    }
    
    int totalAdded = 1;
    
    while (totalAdded < count) {
        int addedThisRound = 0;
        
        for (int i = 0; i < count; i++) {
            if (!added[i] && people[i].id_father != -1) {
                TreeNode* father = findFather(tree->root, people[i].id_father);
                if (father != NULL) {
                    TreeNode* child = createTreeNode(people[i]);
                    if (addChild(father, child)) {
                        added[i] = 1;
                        totalAdded++;
                        addedThisRound++;
                    } else {
                        delete child;
                    }
                }
            }
        }
        
        if (addedThisRound == 0) break;
    }
    
    tree->size = totalAdded;
    return totalAdded;
}

TreeNode* findCurrentKing(TreeNode* root) {
    if (root == NULL) return NULL;
    
    TreeNode* stack[MAX_PEOPLE];
    int top = -1;
    
    stack[++top] = root;
    
    while (top >= 0) {
        TreeNode* current = stack[top--];
        
        if (current->person.is_king && !current->person.is_dead) {
            return current;
        }
        
        if (current->right != NULL) stack[++top] = current->right;
        if (current->left != NULL) stack[++top] = current->left;
    }
    
    return NULL;
}

TreeNode* findSuccessor(TreeNode* node) {
    if (node == NULL) return NULL;
    
    TreeNode* successor = node->left;
    
    if (successor == NULL || successor->person.is_dead) {
        if (node->right != NULL && !node->right->person.is_dead) {
            return node->right;
        }
        return NULL;
    }
    
    return successor;
}

static TreeNode* findNextEligible(TreeNode* root, TreeNode* exclude) {
    if (root == NULL) return NULL;
    
    TreeNode* queue[MAX_PEOPLE];
    int front = 0;
    int rear = 0;
    queue[rear++] = root;
    
    while (front < rear) {
        TreeNode* current = queue[front++];
        
        if (current != exclude && !current->person.is_dead) {
            return current;
        }
        
        if (current->left != NULL) queue[rear++] = current->left;
        if (current->right != NULL) queue[rear++] = current->right;
    }
    
    return NULL;
}

void assignNewKing(FamilyTree* tree) {
    if (tree->root == NULL) return;
    
    TreeNode* currentKing = findCurrentKing(tree->root);
    TreeNode* successor = NULL;
    
    if (currentKing != NULL) {
        successor = findSuccessor(currentKing);
        if (successor == NULL || successor->person.is_dead) {
            successor = findNextEligible(tree->root, currentKing);
        }
        
        if (successor != NULL) {
            currentKing->person.is_king = 0;
            currentKing->person.was_king = 1;
        }
    } else {
        successor = findNextEligible(tree->root, NULL);
    }
    
    if (successor != NULL) {
        successor->person.is_king = 1;
        successor->person.was_king = 1;
    } else if (currentKing != NULL) {
        // No candidato valido; restaurar bandera del rey actual
        currentKing->person.is_king = 1;
    }
}

Person* findPersonById(FamilyTree* tree, int id) {
    TreeNode* node = findPerson(tree->root, id);
    return node ? &(node->person) : NULL;
}
