#include "controller.h"
#include "../model/model.h"
#include "../view/view.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;
static FamilyTree tree;
static Person people[MAX_PEOPLE];
static int personCount = 0;

// Función auxiliar recursiva para mostrar el árbol
void displayTreeRecursive(TreeNode* node, int level) {
    if (node == NULL) return;
    
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s %s (ID: %d%s%s%s)", 
             node->person.name, node->person.last_name, node->person.id,
             node->person.is_king ? ", REY ACTUAL" : "",
             node->person.was_king && !node->person.is_king ? ", EX-REY" : "",
             node->person.is_dead ? ", DIFUNTO" : "");
    
    displayTreeLevel(buffer, level);
    
    displayTreeRecursive(node->left, level + 1);
    displayTreeRecursive(node->right, level + 1);
}

int loadFromCSV(const char* filename, Person* people, int maxPeople) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }
    
    char line[256];
    int count = 0;
    
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file) && count < maxPeople) {
        char* token = strtok(line, ",");
        if (token == NULL) continue;
        
        people[count].id = atoi(token);
        
        token = strtok(NULL, ",");
        if (token) strncpy(people[count].name, token, MAX_NAME_LENGTH - 1);
        
        token = strtok(NULL, ",");
        if (token) strncpy(people[count].last_name, token, MAX_LASTNAME_LENGTH - 1);
        
        token = strtok(NULL, ",");
        if (token) people[count].gender = token[0];
        
        token = strtok(NULL, ",");
        if (token) people[count].age = atoi(token);
        
        token = strtok(NULL, ",");
        if (token) people[count].id_father = atoi(token);
        
        token = strtok(NULL, ",");
        if (token) people[count].is_dead = atoi(token);
        
        token = strtok(NULL, ",");
        if (token) people[count].was_king = atoi(token);
        
        token = strtok(NULL, ",");
        if (token) people[count].is_king = atoi(token);
        
        count++;
    }
    
    fclose(file);
    return count;
}

void initializeSystem() {
    initializeTree(&tree);
    
    // Intentar cargar desde el archivo CSV en la carpeta bin
    personCount = loadFromCSV("bin/familia_real.csv", people, MAX_PEOPLE);
    
    if (personCount == 0) {
        displayMessage("Error: No se pudo cargar el archivo 'familia_real.csv' desde la carpeta bin.");
        displayMessage("Asegurese de que el archivo CSV existe en la misma carpeta donde se ejecuta el programa.");
        return;
    }
    
    if (buildFamilyTree(&tree, people, personCount)) {
        displayMessage("Arbol genealogico construido exitosamente.");
        
        if (findCurrentKing(tree.root) == NULL) {
            assignNewKing(&tree);
        }
    } else {
        displayMessage("Error: No se pudo construir el arbol genealogico.");
    }
}

void cleanupSystem() {
    destroyTree(tree.root);
}

void displayFamilyTree() {
    if (tree.root == NULL) {
        displayMessage("No hay datos cargados. El arbol esta vacio.");
        return;
    }
    cout << "\n=== ARBOL GENEALOGICO REAL ===\n";
    displayTreeRecursive(tree.root, 0);
}

void displayCurrentKing() {
    if (tree.root == NULL) {
        displayMessage("No hay datos cargados. No se puede determinar el rey.");
        return;
    }
    
    TreeNode* king = findCurrentKing(tree.root);
    if (king == NULL) {
        displayMessage("No hay rey actual asignado o todos los candidatos estan muertos.");
    } else {
        cout << "\n=== REY ACTUAL ===\n";
        displayPersonInfo(king->person.id, king->person.name, king->person.last_name,
                         king->person.gender, king->person.age, king->person.id_father,
                         king->person.is_dead, king->person.was_king, king->person.is_king);
    }
}

void assignNewKing() {
    if (tree.root == NULL) {
        displayMessage("No hay datos cargados. No se puede asignar un nuevo rey.");
        return;
    }
    assignNewKing(&tree);
}

void searchPersonById() {
    if (tree.root == NULL) {
        displayMessage("No hay datos cargados. No se puede buscar personas.");
        return;
    }
    
    int id;
    string input;
    cout << "Ingrese ID a buscar: ";
    while (true) {
        if (!getline(cin, input)) {
            displayMessage("Error al leer la entrada. Operacion cancelada.");
            return;
        }
        try {
            size_t processed = 0;
            id = stoi(input, &processed);
            if (processed == input.size()) {
                break;
            }
        } catch (...) {
        }
        displayMessage("Entrada no valida. Ingrese un numero entero.");
        cout << "Ingrese ID a buscar: ";
    }
    
    Person* person = findPersonById(&tree, id);
    if (person != NULL) {
        displayPersonInfo(person->id, person->name, person->last_name, 
                         person->gender, person->age, person->id_father,
                         person->is_dead, person->was_king, person->is_king);
    } else {
        displayMessage("Persona no encontrada.");
    }
}

