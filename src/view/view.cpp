#include "view.h"
#include "../controller/controller.h"
#include <iostream>
#include <string>

using namespace std;

void displayMenu() {
    cout << "\n=== SISTEMA DE GESTION REAL ===" << endl;
    cout << "1. Mostrar arbol genealogico" << endl;
    cout << "2. Mostrar rey actual" << endl;
    cout << "3. Mostrar linea de sucesion" << endl;
    cout << "4. Asignar nuevo rey" << endl;
    cout << "5. Buscar persona por ID" << endl;
    cout << "6. Editar persona" << endl;
    cout << "7. Salir" << endl;
    cout << "Seleccione una opcion: ";
}

void displayPersonInfo(int id, const char* name, const char* last_name, char gender, 
                      int age, int id_father, int is_dead, int was_king, int is_king) {
    cout << "\n=== INFORMACION DE PERSONA ===" << endl;
    cout << "ID: " << id << endl;
    cout << "Nombre: " << name << " " << last_name << endl;
    cout << "Genero: " << gender << endl;
    cout << "Edad: " << age << endl;
    cout << "ID Padre: " << id_father << endl;
    cout << "Estado: " << (is_dead ? "Difunto" : "Vivo") << endl;
    cout << "Fue Rey: " << (was_king ? "Si" : "No") << endl;
    cout << "Es Rey: " << (is_king ? "Si" : "No") << endl;
}

void displayMessage(const char* message) {
    cout << message << endl;
}

void displayTreeLevel(const char* text, int level) {
    for (int i = 0; i < level; i++) {
        cout << "  ";
    }
    cout << text << endl;
}

void runApplication() {
    cout << "=== SISTEMA DE GESTION DEL REINO ===" << endl;
    
    // Inicializar el sistema a través del controlador
    initializeSystem();
    
    int option;
    int running = 1;
    
    while (running) {
        displayMenu();
        cin >> option;
        
        switch (option) {
            case 1:
                displayFamilyTree();
                break;
            case 2:
                displayCurrentKing();
                break;
            case 3:
                displaySuccessionLine();
                break;
            case 4: {
                int result = assignNewKing();
                if (result > 0) {
                    displayMessage("Nuevo rey asignado automaticamente segun las reglas de sucesion.");
                }
                displayCurrentKing();
                break;
            }
            case 5:
                searchPersonById();
                break;
            case 6:
                editPerson();
                break;
            case 7:
                displayMessage("Saliendo del sistema...");
                running = 0;
                break;
            default:
                displayMessage("Opcion no valida. Intente nuevamente.");
                break;
        }
    }
    
    // Limpiar recursos a través del controlador
    cleanupSystem();
}