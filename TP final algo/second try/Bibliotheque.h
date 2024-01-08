#ifndef _LIB_H_
#define _LIB_H_

#define _NAME_SIZE_ 12
#define CORES 4

typedef struct element *TarbreProcessus;
#define Tree TarbreProcessus
struct element
{
    int PID;
    char Nom[_NAME_SIZE_];
    int RAM;
    int Etat;
    int priority;
    char father_name[_NAME_SIZE_];  
    Tree Fils;
    Tree Frere;
};

enum ETAT
{
    PRET,
    ELU,
    BLOCK
};

typedef struct elementProcessus *TListeProcessus;
struct elementProcessus
{
    Tree Info;
    struct elementProcessus *Next;
};


typedef struct stackNode *TStackNode;
struct stackNode
{
    Tree process;
    TStackNode next;
};

typedef struct
{
    TStackNode top;
} TStack;

extern TListeProcessus* readyQueue;
extern TListeProcessus* electedQueue;
extern StackNode* blockedStack;


void addToStack(TStack *stack, Tree process);
Tree popFromStack(TStack *stack);
void addToQueue(TListeProcessus *queue, Tree process);
Tree removeFromQueue(TListeProcessus *queue);
Tree findProcessByName(Tree root, char *name);  
void displayProcessList(TListeProcessus queue);
void addToStateQueue(Tree process);
Tree removeFromStateQueue(int state);
Tree createProcess(int pid, char *name, int ram, int priority, char *father_name);  
void blockProcess(Tree *root, char* processName);
Tree unblockProcess(Tree *root, char *name);
void killProcess(Tree *root, Tree process)
void terminateProcess(Tree *root, char *name);  
void launchProcesses(TListeProcessus *readyQueue, TListeProcessus *electedQueue, TStack *blockedStack);
int countProcesses(TListeProcessus queue);

#endif
