#include "Bibliotheque.h"
#include <stdio.h>
#include <stdlib.h>

void initializeRAM(Tree *root, int ramSize);
void executeOperation(Tree *root, TListeProcessus *readyQueue, TListeProcessus *electedQueue, TListeProcessus *blockedStack);
void initializeStack(TStack *blockedStack)
{
    blockedStack->top = NULL;
}
int main()
{
    Tree root = NULL;
    TListeProcessus readyQueue = NULL;
    TListeProcessus electedQueue = NULL;
    TStack blockedStack;

    int ramSize;
    printf("Enter the size of RAM: ");
    scanf("%d", &ramSize);
    initializeRAM(&root, ramSize);

    addToQueue(&readyQueue, root);
    launchProcesses(&readyQueue, &electedQueue, &blockedStack);

    while (1)
    {
        executeOperation(&root, &readyQueue, &electedQueue, &blockedStack);
    }

    return 0;
}

void initializeRAM(Tree *root, int ramSize)
{
    *root = createProcess(0, "Root", ramSize, 1, "");
}

void executeOperation(Tree *root, TListeProcessus *readyQueue, TListeProcessus *electedQueue, TListeProcessus *blockedStack)
{
    int operation;
    printf("\nOperations:\n");
    printf("1. Create\n2. Launch\n3. Block\n4. Unblock\n5. Htop\n6. Kill\n7. Terminate\n");
    printf("Enter operation number: ");
    scanf("%d", &operation);

    switch (operation)
    {
    case 1:
        int pid, ram, priority;
        char name[_NAME_SIZE_], father_name[_NAME_SIZE_];
        printf("Enter process PID: ");
        scanf("%d", &pid);
        printf("Enter process name: ");
        scanf("%s", name);
        printf("Enter process RAM size: ");
        scanf("%d", &ram);
        printf("Enter process priority: ");
        scanf("%d", &priority);
        printf("Enter father process name: ");
        scanf("%s", father_name);

        Tree newProcess = createProcess(pid, name, ram, priority, father_name);
        if (newProcess != NULL)
        {
            printf("Process created and added to the ready queue.\n");
        }
        else
        {
            printf("Error: Failed to create the process.\n");
        }
        break;
    case 2:
        launchProcesses(readyQueue, electedQueue, blockedStack);
        printf("Processes launched.\n");
        break;
    case 3:
        char blockName[_NAME_SIZE_];
        printf("Enter process name to block: ");
        scanf("%s", &blockName);
        blockProcess(root, blockName);
        break;
    case 4:
        char unblockName[_NAME_SIZE_];
        printf("Enter process name to unblock: ");
        scanf("%s", &unblockName);
        unblockProcess(root, unblockName);
        break;
    case 5:
        displayProcessList(*electedQueue);
        break;
    case 6:
        char killName[_NAME_SIZE_];
        printf("Enter process name to kill: ");
        scanf("%s", killName);
        Tree processToKill = findProcessByName(*root, killName);
        if (processToKill != NULL)
        {
            killProcess(root, processToKill);
            printf("Process with name %s killed.\n", killName);
        }
        else
        {
            printf("Error: Process with name %s not found.\n", killName);
        }
        break;
    case 7:
        char terminateName[_NAME_SIZE_];
        printf("Enter process name to terminate: ");
        scanf("%s", terminateName);
        terminateProcess(root, terminateName);
        break;
    default:
        printf("Invalid operation number. Please try again.\n");
        break;
    }
}
