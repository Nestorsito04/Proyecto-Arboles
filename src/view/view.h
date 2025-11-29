#ifndef VIEW_H
#define VIEW_H

// Funciones auxiliares disponibles para otros módulos
void displayMenu();
void displayPersonInfo(int id, const char* name, const char* last_name, char gender,
					   int age, int id_father, int is_dead, int was_king, int is_king);
void displayMessage(const char* message);
void displayTreeLevel(const char* text, int level);

// Función principal que maneja toda la aplicación
void runApplication();

#endif