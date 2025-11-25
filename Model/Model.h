#ifndef MODEL_H
#define MODEL_H

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
    struct TreeNode* left;
    struct TreeNode* right;
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
void assignNewKing(FamilyTree* tree);
Person* findPersonById(FamilyTree* tree, int id);

#endif