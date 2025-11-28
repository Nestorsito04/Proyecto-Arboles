#include "model.h"
#include <cstring>

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

TreeNode* findFirstLivingMaleInLine(TreeNode* start) {
    if (start == NULL) return NULL;
    
    if (!start->person.is_dead &&
        !start->person.was_king &&
        start->person.gender == 'H' &&
        start->person.age < 70) {
        return start;
    }
    
    TreeNode* candidate = findFirstLivingMaleInLine(start->left);
    if (candidate != NULL) return candidate;
    
    return findFirstLivingMaleInLine(start->right);
}

void collectLivingFemales(TreeNode* start, TreeNode** candidates, int* count) {
    if (start == NULL) return;
    
    if (!start->person.is_dead &&
        !start->person.was_king &&
        start->person.gender == 'M' &&
        start->person.age >= 15 &&
        start->person.age < 70) {
        candidates[*count] = start;
        (*count)++;
    }
    
    if (start->left != NULL) collectLivingFemales(start->left, candidates, count);
    if (start->right != NULL) collectLivingFemales(start->right, candidates, count);
}

TreeNode* findFirstLivingFemaleInLine(TreeNode* start) {
    if (start == NULL) return NULL;
    
    TreeNode* candidates[MAX_PEOPLE];
    int count = 0;
    
    collectLivingFemales(start, candidates, &count);
    
    if (count == 0) return NULL;
    
    // Ordenar por edad (más joven primero) y por cercanía al primogénito
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (candidates[i]->person.age > candidates[j]->person.age || 
                (candidates[i]->person.age == candidates[j]->person.age && 
                 candidates[i]->person.id > candidates[j]->person.id)) {
                TreeNode* temp = candidates[i];
                candidates[i] = candidates[j];
                candidates[j] = temp;
            }
        }
    }
    
    return candidates[0];
}

TreeNode* findBrother(TreeNode* root, TreeNode* person) {
    if (root == NULL || person == NULL) return NULL;
    
    // Encontrar al padre
    TreeNode* father = findFather(root, person->person.id_father);
    if (father == NULL) return NULL;
    
    // Buscar hermanos (excluyendo a la persona misma)
    if (father->left != NULL && father->left != person) {
        return father->left;
    }
    if (father->right != NULL && father->right != person) {
        return father->right;
    }
    
    return NULL;
}

TreeNode* findUncle(TreeNode* root, TreeNode* person) {
    if (root == NULL || person == NULL) return NULL;
    
    // Encontrar al padre
    TreeNode* father = findFather(root, person->person.id_father);
    if (father == NULL) return NULL;
    
    // Encontrar al abuelo
    TreeNode* grandfather = findFather(root, father->person.id_father);
    if (grandfather == NULL) return NULL;
    
    // Buscar tíos (hermanos del padre)
    return findBrother(root, father);
}

TreeNode* findAncestorWithTwoSons(TreeNode* root, TreeNode* person) {
    if (root == NULL || person == NULL) return NULL;
    
    TreeNode* current = person;
    while (current != NULL && current != root) {
        TreeNode* father = findFather(root, current->person.id_father);
        if (father != NULL) {
            // Verificar si este ancestro tiene al menos dos hijos
            if (father->left != NULL && father->right != NULL) {
                return father;
            }
        }
        current = father;
    }
    
    return NULL;
}

TreeNode* findFirstLivingMaleInPrimaryLine(TreeNode* root) {
    if (root == NULL) return NULL;
    
    TreeNode* current = root;
    while (current != NULL) {
        if (!current->person.is_dead &&
            !current->person.was_king &&
            current->person.gender == 'H' &&
            current->person.age < 70) {
            return current;
        }
        current = current->left;
    }
    
    return NULL;
}

TreeNode* findFirstLivingMaleInSecondaryLine(TreeNode* root) {
    if (root == NULL) return NULL;
    
    if (!root->person.is_dead &&
        !root->person.was_king &&
        root->person.gender == 'H' &&
        root->person.age < 70) {
        return root;
    }
    
    if (root->right != NULL) {
        TreeNode* candidate = findFirstLivingMaleInSecondaryLine(root->right);
        if (candidate != NULL) return candidate;
    }
    
    if (root->left != NULL) {
        TreeNode* candidate = findFirstLivingMaleInSecondaryLine(root->left);
        if (candidate != NULL) return candidate;
    }
    
    return NULL;
}

int assignNewKing(FamilyTree* tree) {
    if (tree == NULL || tree->root == NULL) return 0;
    
    TreeNode* currentKing = findCurrentKing(tree->root);
    
    // Si no hay rey actual, intentar coronar a la raíz directamente
    if (currentKing == NULL) {
        currentKing = tree->root;
        if (!currentKing->person.is_dead && !currentKing->person.is_king) {
            currentKing->person.is_king = 1;
            currentKing->person.was_king = 1;
            return 1;
        }
    }
    
    int needsReplacement = currentKing->person.is_dead || currentKing->person.age >= 70;
    TreeNode* newKing = NULL;
    
    // REGLA 1: Buscar en hijos (primogénito varón vivo)
    if (currentKing->left != NULL) {
        newKing = findFirstLivingMaleInLine(currentKing->left);
    }
    
    // REGLA 2: Si no hay hijos, buscar en hermanos
    if (newKing == NULL) {
        TreeNode* brother = findBrother(tree->root, currentKing);
        if (brother != NULL) {
            if (!brother->person.is_dead &&
                !brother->person.was_king &&
                brother->person.gender == 'H' &&
                brother->person.age < 70) {
                newKing = brother;
            } else {
                newKing = findFirstLivingMaleInLine(brother);
            }
        }
    }
    
    // REGLA 3: Si no hay hermanos, buscar en tíos
    if (newKing == NULL) {
        TreeNode* uncle = findUncle(tree->root, currentKing);
        if (uncle != NULL) {
            if (!uncle->person.is_dead &&
                !uncle->person.was_king &&
                uncle->person.gender == 'H' &&
                uncle->person.age < 70 &&
                uncle->left == NULL && uncle->right == NULL) {
                newKing = uncle;
            } else {
                newKing = findFirstLivingMaleInLine(uncle);
            }
        }
    }
    
    // REGLA 4: Buscar ancestro con dos hijos y tomar primogénito de esa rama
    if (newKing == NULL) {
        TreeNode* ancestor = findAncestorWithTwoSons(tree->root, currentKing);
        if (ancestor != NULL) {
            newKing = findFirstLivingMaleInPrimaryLine(ancestor);
        }
    }
    
    // REGLA 5: Si no hay varones en líneas primogénitas, buscar en líneas secundarias
    if (newKing == NULL) {
        newKing = findFirstLivingMaleInSecondaryLine(tree->root);
    }
    
    // REGLA 6: Si no hay varones, buscar mujeres
    if (newKing == NULL) {
        newKing = findFirstLivingFemaleInLine(tree->root);
    }
    
    if (newKing == NULL) {
        if (needsReplacement) {
            currentKing->person.is_king = 0;
            currentKing->person.was_king = 1;
            return -1; // Rey removido pero sin sucesor
        }
        return 0; // No se requiere cambio y no hay sucesores disponibles
    }
    
    currentKing->person.is_king = 0;
    currentKing->person.was_king = 1;
    newKing->person.is_king = 1;
    newKing->person.was_king = 1;
    return 1; // Se asignó nuevo rey
}

Person* findPersonById(FamilyTree* tree, int id) {
    TreeNode* node = findPerson(tree->root, id);
    return node ? &(node->person) : NULL;
}

void findSuccessionLine(TreeNode* root, TreeNode** succession, int* count) {
    if (root == NULL || succession == NULL || count == NULL) return;
    
    *count = 0;
    
    // Encontrar al rey actual
    TreeNode* currentKing = findCurrentKing(root);
    if (currentKing == NULL) {
        // Si no hay rey actual, empezar desde la raíz
        currentKing = root;
    }
    
    // Si el rey actual está muerto, no hay línea de sucesión
    if (currentKing->person.is_dead) {
        return;
    }
    
    // Usar BFS (recorrido por niveles) para obtener el orden de sucesión
    TreeNode* queue[MAX_PEOPLE];
    int front = 0, rear = 0;
    int visited[MAX_PEOPLE] = {0};
    
    // Marcar y encolar el rey actual
    visited[currentKing->person.id] = 1;
    queue[rear++] = currentKing;
    
    while (front < rear && *count < MAX_PEOPLE) {
        TreeNode* current = queue[front++];
        
        // Solo agregar a la sucesión si está vivo y no es el rey actual
        if (!current->person.is_dead && current != currentKing) {
            succession[*count] = current;
            (*count)++;
        }
        
        // Encolar hijos en orden de primogenitura (primero left, luego right)
        if (current->left != NULL && !visited[current->left->person.id]) {
            visited[current->left->person.id] = 1;
            queue[rear++] = current->left;
        }
        if (current->right != NULL && !visited[current->right->person.id]) {
            visited[current->right->person.id] = 1;
            queue[rear++] = current->right;
        }
    }
    
    // Ordenar por prioridad de sucesión (primogénitos primero)
    for (int i = 0; i < *count; i++) {
        for (int j = i + 1; j < *count; j++) {
            // Priorizar por nivel en el árbol (más cercano al rey primero)
            TreeNode* node1 = succession[i];
            TreeNode* node2 = succession[j];
            
            if (node1->person.id > node2->person.id) {
                TreeNode* temp = succession[i];
                succession[i] = succession[j];
                succession[j] = temp;
            }
        }
    }
}