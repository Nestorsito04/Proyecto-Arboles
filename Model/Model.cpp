#include "model.h"
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

void initializeTree(FamilyTree* tree) {
    tree->root = NULL;
    tree->size = 0;
}

TreeNode* createTreeNode(Person p) {
    TreeNode* newNode = new TreeNode;
    newNode->person = p;
    return newNode;
}

void destroyTree(TreeNode* root) {
    if (root == NULL) return;
    for (TreeNode* child : root->children) {
        destroyTree(child);
    }
    delete root;
}

TreeNode* findPerson(TreeNode* root, int id) {
    if (root == NULL) return NULL;
    if (root->person.id == id) return root;

    for (TreeNode* child : root->children) {
        TreeNode* found = findPerson(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

TreeNode* findFather(TreeNode* root, int id_father) {
    if (root == NULL) return NULL;
    if (root->person.id == id_father) return root;

    for (TreeNode* child : root->children) {
        TreeNode* found = findFather(child, id_father);
        if (found != NULL) return found;
    }
    return NULL;
}

int addChild(TreeNode* father, TreeNode* child) {
    if (father == NULL || child == NULL) return 0;
    father->children.push_back(child);
    return 1;
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
        
        for (int i = static_cast<int>(current->children.size()) - 1; i >= 0; --i) {
            stack[++top] = current->children[i];
        }
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
    
    for (TreeNode* child : start->children) {
        TreeNode* candidate = findFirstLivingMaleInLine(child);
        if (candidate != NULL) return candidate;
    }
    
    return NULL;
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
    
    for (TreeNode* child : start->children) {
        collectLivingFemales(child, candidates, count);
    }
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

TreeNode* findFirstLivingFemaleDescendant(TreeNode* person) {
    if (person == NULL) return NULL;

    TreeNode* candidate = NULL;

    for (TreeNode* child : person->children) {
        candidate = findFirstLivingFemaleInLine(child);
        if (candidate != NULL) return candidate;
    }
    
    return NULL;
}

TreeNode* findAncestorWithTwoSons(TreeNode* root, TreeNode* person) {
    if (root == NULL || person == NULL) return NULL;
    
    TreeNode* current = person;
    while (current != NULL && current != root) {
        TreeNode* father = findFather(root, current->person.id_father);
        if (father != NULL) {
            // Verificar si este ancestro tiene al menos dos hijos
            if (father->children.size() >= 2) {
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
        if (current->children.empty()) break;
        current = current->children[0];
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

    for (size_t i = 1; i < root->children.size(); ++i) {
        TreeNode* candidate = findFirstLivingMaleInLine(root->children[i]);
        if (candidate != NULL) return candidate;
    }

    if (!root->children.empty()) {
        return findFirstLivingMaleInSecondaryLine(root->children[0]);
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
    for (TreeNode* child : currentKing->children) {
        newKing = findFirstLivingMaleInLine(child);
        if (newKing != NULL) break;
    }
    
    // REGLA 2: Si no hay hijos, buscar en hermanos
    if (newKing == NULL) {
        TreeNode* father = findFather(tree->root, currentKing->person.id_father);
        if (father != NULL) {
            for (TreeNode* sibling : father->children) {
                if (sibling == currentKing) continue;
                if (!sibling->person.is_dead &&
                    !sibling->person.was_king &&
                    sibling->person.gender == 'H' &&
                    sibling->person.age < 70) {
                    newKing = sibling;
                    break;
                }
                TreeNode* candidate = findFirstLivingMaleInLine(sibling);
                if (candidate != NULL) {
                    newKing = candidate;
                    break;
                }
            }
        }
    }
    
    // REGLA 3: Si no hay hermanos, buscar en tíos
    if (newKing == NULL) {
        TreeNode* father = findFather(tree->root, currentKing->person.id_father);
        TreeNode* grandfather = father ? findFather(tree->root, father->person.id_father) : NULL;
        if (grandfather != NULL) {
            for (TreeNode* uncle : grandfather->children) {
                if (uncle == father) continue;
                if (!uncle->person.is_dead &&
                    !uncle->person.was_king &&
                    uncle->person.gender == 'H' &&
                    uncle->person.age < 70 &&
                    uncle->children.empty()) {
                    newKing = uncle;
                    break;
                }
                TreeNode* candidate = findFirstLivingMaleInLine(uncle);
                if (candidate != NULL) {
                    newKing = candidate;
                    break;
                }
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
    
    // REGLA 6: Priorizar mujeres dentro de la descendencia directa del rey actual
    if (newKing == NULL) {
        newKing = findFirstLivingFemaleDescendant(currentKing);
    }

    // REGLA 7: Si no hay varones ni descendencia femenina directa, buscar mujeres en todo el arbol
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
        
        // Encolar hijos en orden de primogenitura
        for (TreeNode* child : current->children) {
            if (child != NULL && !visited[child->person.id]) {
                visited[child->person.id] = 1;
                queue[rear++] = child;
            }
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

int editPerson(TreeNode* person) {
    if (person == nullptr) {
        cout << "Error: Persona no valida." << endl;
        return 0;
    }
    
    cout << "\n=== EDITANDO PERSONA ===" << endl;
    cout << "ID: " << person->person.id << " (No editable)" << endl;
    cout << "ID Padre: " << person->person.id_father << " (No editable)" << endl;
    cout << "Informacion actual:" << endl;
    cout << "Nombre: " << person->person.name << endl;
    cout << "Apellido: " << person->person.last_name << endl;
    cout << "Genero: " << person->person.gender << endl;
    cout << "Edad: " << person->person.age << endl;
    cout << "Estado: " << (person->person.is_dead ? "Difunto" : "Vivo") << endl;
    cout << "Fue Rey: " << (person->person.was_king ? "Si" : "No") << endl;
    cout << "Es Rey: " << (person->person.is_king ? "Si" : "No") << endl;
    
    int option;
    cout << "\n¿Que desea hacer?" << endl;
    cout << "1. Editar informacion especifica" << endl;
    cout << "2. Editar toda la informacion" << endl;
    cout << "3. Cancelar" << endl;
    cout << "Seleccione una opcion: ";
    cin >> option;
    
    if (option == 3) {
        cout << "Edicion cancelada." << endl;
        return 0;
    }
    
    if (option == 1) {
        // Edición específica
        cout << "\n¿Que campo desea editar?" << endl;
        cout << "1. Nombre" << endl;
        cout << "2. Apellido" << endl;
        cout << "3. Genero" << endl;
        cout << "4. Edad" << endl;
        cout << "5. Estado (Vivo/Difunto)" << endl;
        cout << "6. Fue Rey" << endl;
        cout << "7. Es Rey" << endl;
        cout << "8. Cancelar" << endl;
        cout << "Seleccione una opcion: ";
        
        int fieldOption;
        cin >> fieldOption;
        
        switch (fieldOption) {
            case 1: {
                cout << "Nuevo nombre: ";
                string newName;
                cin.ignore();
                getline(cin, newName);
                strncpy(person->person.name, newName.c_str(), MAX_NAME_LENGTH - 1);
                person->person.name[MAX_NAME_LENGTH - 1] = '\0';
                cout << "Nombre actualizado correctamente." << endl;
                break;
            }
            case 2: {
                cout << "Nuevo apellido: ";
                string newLastName;
                cin.ignore();
                getline(cin, newLastName);
                strncpy(person->person.last_name, newLastName.c_str(), MAX_LASTNAME_LENGTH - 1);
                person->person.last_name[MAX_LASTNAME_LENGTH - 1] = '\0';
                cout << "Apellido actualizado correctamente." << endl;
                break;
            }
            case 3: {
                cout << "Nuevo genero (H/M): ";
                char newGender;
                cin >> newGender;
                if (newGender == 'H' || newGender == 'M') {
                    person->person.gender = newGender;
                    cout << "Genero actualizado correctamente." << endl;
                } else {
                    cout << "Genero no valido. Debe ser H o M." << endl;
                    return 0;
                }
                break;
            }
            case 4: {
                cout << "Nueva edad: ";
                int newAge;
                cin >> newAge;
                if (newAge >= 0 && newAge <= 150) {
                    person->person.age = newAge;
                    cout << "Edad actualizada correctamente." << endl;
                    
                    // Verificar si el rey supera los 70 años
                    if (person->person.is_king && newAge >= 70) {
                        cout << "¡El rey ha superado los 70 años! Se asignara un nuevo rey automaticamente." << endl;
                    }
                } else {
                    cout << "Edad no valida. Debe estar entre 0 y 150." << endl;
                    return 0;
                }
                break;
            }
            case 5: {
                cout << "Nuevo estado (0=Vivo, 1=Difunto): ";
                int newState;
                cin >> newState;
                if (newState == 0 || newState == 1) {
                    person->person.is_dead = newState;
                    cout << "Estado actualizado correctamente." << endl;
                    
                    // Verificar si el rey ha muerto
                    if (person->person.is_king && newState == 1) {
                        cout << "¡El rey ha muerto! Se asignara un nuevo rey automaticamente." << endl;
                    }
                } else {
                    cout << "Estado no valido. Debe ser 0 o 1." << endl;
                    return 0;
                }
                break;
            }
            case 6: {
                cout << "¿Fue rey? (0=No, 1=Si): ";
                int newWasKing;
                cin >> newWasKing;
                if (newWasKing == 0 || newWasKing == 1) {
                    person->person.was_king = newWasKing;
                    cout << "Estado 'Fue Rey' actualizado correctamente." << endl;
                } else {
                    cout << "Valor no valido. Debe ser 0 o 1." << endl;
                    return 0;
                }
                break;
            }
            case 7: {
                cout << "¿Es rey? (0=No, 1=Si): ";
                int newIsKing;
                cin >> newIsKing;
                if (newIsKing == 0 || newIsKing == 1) {
                    person->person.is_king = newIsKing;
                    cout << "Estado 'Es Rey' actualizado correctamente." << endl;
                } else {
                    cout << "Valor no valido. Debe ser 0 o 1." << endl;
                    return 0;
                }
                break;
            }
            case 8:
                cout << "Edicion cancelada." << endl;
                return 0;
            default:
                cout << "Opcion no valida." << endl;
                return 0;
        }
    } 
    else if (option == 2) {
        // Edición completa
        cout << "\n=== EDITANDO TODA LA INFORMACION ===" << endl;
        
        // Nombre
        cout << "Nuevo nombre: ";
        string newName;
        cin.ignore();
        getline(cin, newName);
        strncpy(person->person.name, newName.c_str(), MAX_NAME_LENGTH - 1);
        person->person.name[MAX_NAME_LENGTH - 1] = '\0';
        
        // Apellido
        cout << "Nuevo apellido: ";
        string newLastName;
        getline(cin, newLastName);
        strncpy(person->person.last_name, newLastName.c_str(), MAX_LASTNAME_LENGTH - 1);
        person->person.last_name[MAX_LASTNAME_LENGTH - 1] = '\0';
        
        // Género
        cout << "Nuevo genero (H/M): ";
        char newGender;
        cin >> newGender;
        while (newGender != 'H' && newGender != 'M') {
            cout << "Genero no valido. Debe ser H o M: ";
            cin >> newGender;
        }
        person->person.gender = newGender;
        
        // Edad
        cout << "Nueva edad: ";
        int newAge;
        cin >> newAge;
        while (newAge < 0 || newAge > 150) {
            cout << "Edad no valida. Debe estar entre 0 y 150: ";
            cin >> newAge;
        }
        person->person.age = newAge;
        
        // Estado
        cout << "Nuevo estado (0=Vivo, 1=Difunto): ";
        int newState;
        cin >> newState;
        while (newState != 0 && newState != 1) {
            cout << "Estado no valido. Debe ser 0 o 1: ";
            cin >> newState;
        }
        person->person.is_dead = newState;
        
        // Fue Rey
        cout << "¿Fue rey? (0=No, 1=Si): ";
        int newWasKing;
        cin >> newWasKing;
        while (newWasKing != 0 && newWasKing != 1) {
            cout << "Valor no valido. Debe ser 0 o 1: ";
            cin >> newWasKing;
        }
        person->person.was_king = newWasKing;
        
        // Es Rey
        cout << "¿Es rey? (0=No, 1=Si): ";
        int newIsKing;
        cin >> newIsKing;
        while (newIsKing != 0 && newIsKing != 1) {
            cout << "Valor no valido. Debe ser 0 o 1: ";
            cin >> newIsKing;
        }
        person->person.is_king = newIsKing;
        
        cout << "Toda la informacion ha sido actualizada correctamente." << endl;
        
        // Verificar condiciones especiales
        if (person->person.is_king) {
            if (person->person.is_dead) {
                cout << "¡ADVERTENCIA: El rey esta marcado como difunto!" << endl;
            }
            if (person->person.age >= 70) {
                cout << "¡ADVERTENCIA: El rey ha superado los 70 años!" << endl;
            }
        }
    }
    
    return 1;
}