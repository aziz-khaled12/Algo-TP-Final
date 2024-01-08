#include "Bibliotheque.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


TListeProcessus* readyQueue;
TListeProcessus* electedQueue;
TStack* blockedStack;


void addToQueue(TListeProcessus *queue, Tree process)
{
    TListeProcessus newElement = malloc(sizeof(struct elementProcessus));
    if (newElement != NULL)
    {
        newElement->Info = process;
        newElement->Next = NULL;

        if (*queue == NULL)
        {
            *queue = newElement;
        }
        else
        {
            TListeProcessus current = *queue;
            while (current->Next != NULL)
            {
                current = current->Next;
            }
            current->Next = newElement;
        }
    }
    else
    {
        printf("Error: Failed to allocate memory for the queue element.\n");
    }
}

Tree removeFromQueue(TListeProcessus *queue)
{
    if (*queue != NULL)
    {
        Tree process = (*queue)->Info;
        TListeProcessus temp = *queue;
        *queue = (*queue)->Next;
        free(temp);
        return process;
    }
    else
    {
        printf("Error: Queue is empty.\n");
        return NULL;
    }
}

Tree findProcessByName(Tree root, char *name)
{
    if (root == NULL)
    {
        return NULL;
    }

    if (strcmp(root->Nom, name) == 0)
    {
        return root;
    }

    Tree foundProcess = findProcessByName(root->Fils, name);
    if (foundProcess != NULL)
    {
        return foundProcess;
    }

    return findProcessByName(root->Frere, name);
}

void displayProcessList(TListeProcessus queue)
{
    printf("Process List:\n");
    while (queue != NULL)
    {
        printf("PID: %d, Name: %s, RAM: %d, State: %d\n", queue->Info->PID, queue->Info->Nom, queue->Info->RAM, queue->Info->Etat);
        queue = queue->Next;
    }
}

void launchProcesses(TListeProcessus *readyQueue, TListeProcessus *electedQueue, TStack *blockedStack)
{
    while (*electedQueue != NULL && countProcesses(*electedQueue) < CORES)
    {
        Tree highestPriorityProcess = findHighestPriorityProcess(*readyQueue);
        if (highestPriorityProcess != NULL)
        {
            removeFromQueueWithPriority(readyQueue, highestPriorityProcess->priority);
            highestPriorityProcess->Etat = ELU;
            addToStateQueue(highestPriorityProcess);
        }
        else
        {
            break; 
        }
    }
}

int countProcesses(TListeProcessus queue) {
    int count = 0;
    while (queue != NULL) {
        count++;
        queue = queue->Next;
    }
    return count;
}

void addToStateQueue(Tree process)
{
    switch (process->Etat)
    {
    case PRET:
        addToQueue(readyQueue, process);
        break;
    case ELU:
        addToQueue(electedQueue, process);
        break;
    case BLOCK:
        addToStack(blockedStack, process);  
        break;
    default:
        printf("Error: Unknown state for process with PID %d.\n", process->PID);
        break;
    }
}

Tree removeFromStateQueue(int state)
{
    TListeProcessus *targetQueue = NULL;

    switch (state)
    {
    case PRET:
        targetQueue = readyQueue;
        break;
    case ELU:
        targetQueue = electedQueue;
        break;
    case BLOCK:
        targetQueue = blockedStack;
        break;
    default:
        printf("Error: Unknown state %d.\n", state);
        return NULL;
    }

    return removeFromQueue(targetQueue);
}

Tree createProcess(int pid, char *name, int ram, int priority, char *father_name)
{
    Tree newProcess = malloc(sizeof(struct element));
    if (newProcess != NULL)
    {
        newProcess->PID = pid;
        strncpy(newProcess->Nom, name, _NAME_SIZE_);
        newProcess->RAM = ram;
        newProcess->Etat = PRET;
        addToStateQueue(newProcess);
        newProcess->priority = priority;
        strncpy(newProcess->father_name, father_name, _NAME_SIZE_);  
        newProcess->Fils = NULL;
        newProcess->Frere = NULL;
        addToStateQueue(newProcess);

    }
    return newProcess;
}

void blockProcess(Tree *root, char* processName)
{
    Tree process = findProcessByName(*root, processName);
    if (process != NULL && process->Etat == PRET) {
        process->Etat = BLOCK;
        addToStateQueue(process);
        removeFromQueueByName(electedQueue, processName);
    } else {
        printf("Error: Process with name %s not found or not in the ready state.\n", processName);
    }
}

void removeFromQueueByName(TListeProcessus* queue, char* processName) {
    TListeProcessus* current = queue;
    TListeProcessus temp = NULL;

    while (*current != NULL && strcmp((*current)->Info->Nom, processName) != 0) {
        current = &((*current)->Next);
    }

    if (*current != NULL) {
        temp = *current;
        *current = (*current)->Next;
        free(temp);
    }
}

Tree unblockProcess(Tree *root, char *name)
{
    Tree processToUnblock = findProcessByName(*root, name);

    if (processToUnblock != NULL && processToUnblock->Etat == BLOCK) {        
        removeFromStack(blockedStack);
        processToUnblock->Etat = PRET;
        addToStateQueue(processToUnblock);
        
        printf("Process with name %s unblocked.\n", name);
    } else {
        printf("Error: Process with name %s not found or not blocked.\n", name);
    }
}

Tree removeFromStackWithName(TListeProcessus *stack, char *name) {
    TListeProcessus *current = stack;
    TListeProcessus temp = NULL;

    while (*current != NULL && strcmp((*current)->Info->Nom, name) != 0) {
        current = &((*current)->Next);
    }

    if (*current != NULL) {
        temp = *current;
        Tree process = temp->Info;
        *current = (*current)->Next;
        free(temp);
        return process;
    } else {
        printf("Error: Process with name %s not found in the stack.\n", name);
        return NULL;
    }
}

void killProcessAndChildrenByName(TarbreProcessus *root, Tree process)
{
    if (process != NULL)
    {
        killProcessAndChildrenByName(root, process->Fils);
        killProcessAndChildrenByName(root, process->Frere);

        Tree parent = findProcessByName(*root, process->father_name);
        if (parent != NULL)
        {
            if (parent->Fils == process)
            {
                parent->Fils = process->Frere;
            }
            else
            {
                Tree sibling = parent->Fils;
                while (sibling != NULL && strcmp(sibling->Frere->Nom, process->Nom) != 0)
                {
                    sibling = sibling->Frere;
                }
                if (sibling != NULL)
                {
                    sibling->Frere = process->Frere;
                }
            }
        }

        free(process);
    }
}

void killProcess(Tree *root, Tree process)
{
    Tree process = findProcessByName(*root, process->Nom);
    if (process != NULL)
    {
        killProcessAndChildrenByName(root, process);

        removeFromQueueWithName(electedQueue, process->Nom);

        printf("Process with name %s killed.\n", process->Nom);
    }
    else
    {
        printf("Error: Process with name %s not found.\n", process->Nom);
    }
}

void terminateProcess(Tree *root, char *name)
{
    Tree process = findProcessByName(*root, name);
    if (process != NULL)
    {
        killProcessAndChildrenByName(root, process);

        removeFromQueueWithName(electedQueue, name);

        printf("Process with name %s terminated.\n", name);
    }
    else
    {
        printf("Error: Process with name %s not found.\n", name);
    }
}

void addToStack(TStack *stack, Tree process)
{
    TStackNode newNode = malloc(sizeof(struct stackNode));
    if (newNode != NULL)
    {
        newNode->process = process;
        newNode->next = stack->top;
        stack->top = newNode;
    }
    else
    {
        printf("Error: Failed to allocate memory for the stack node.\n");
    }
}

Tree popFromStack(TStack *stack)
{
    if (stack->top != NULL)
    {
        Tree process = stack->top->process;
        TStackNode temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
        return process;
    }
    else
    {
        printf("Error: Stack is empty.\n");
        return NULL;
    }
}
