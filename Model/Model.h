#ifndef MODEL_H
#define MODEL_H

#include <vector>

#define MAX_NAME_LENGTH 50
#define MAX_LASTNAME_LENGTH 50
#define MAX_PEOPLE 1000

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char last_name[MAX_LASTNAME_LENGTH];
    char gender;
    int age;
    int id_father;
    int is_dead;
    int was_king;
    int is_king;
} Person;

typedef struct TreeNode {
    Person person;
    std::vector<TreeNode*> children;
} TreeNode;

typedef struct {
    TreeNode* root;
    int size;
} FamilyTree;

// Funciones del modelo
void initializeTree(FamilyTree* tree);
TreeNode* createTreeNode(Person p);
void destroyTree(TreeNode* root);
TreeNode* findPerson(TreeNode* root, int id);
TreeNode* findFather(TreeNode* root, int id_father);
int addChild(TreeNode* father, TreeNode* child);
int buildFamilyTree(FamilyTree* tree, Person* people, int count);
TreeNode* findCurrentKing(TreeNode* root);
int assignNewKing(FamilyTree* tree);
Person* findPersonById(FamilyTree* tree, int id);
void findSuccessionLine(TreeNode* root, TreeNode** succession, int* count);

// Funciones auxiliares para la sucesión
TreeNode* findFirstLivingMaleInLine(TreeNode* start);
TreeNode* findFirstLivingFemaleInLine(TreeNode* start);
TreeNode* findFirstLivingFemaleDescendant(TreeNode* person);
TreeNode* findAncestorWithTwoSons(TreeNode* root, TreeNode* person);
TreeNode* findFirstLivingMaleInPrimaryLine(TreeNode* root);
TreeNode* findFirstLivingMaleInSecondaryLine(TreeNode* root);

// Función para editar persona
int editPerson(TreeNode* person);

#endif