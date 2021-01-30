#include "List.h"

listStruct* InitList()
{
	listStruct* pList = (listStruct*)malloc(sizeof(listStruct));
	pList->head = NULL;
	InitializeCriticalSection(&pList->critSec);

	return pList;
}


void PrintList(listStruct* pList)
{
	EnterCriticalSection(&pList->critSec);

	node_t* temp = pList->head;

	printf("====================\n");
	while (temp != NULL)
	{
		printf("Client Id: %d\n", temp->clientWorker->idClient);
		printf("Number of Workers: %d\n", temp->clientWorker->numOfWorkers);
		for (int i = 0; i < temp->clientWorker->numOfWorkers; i++)
		{
			printf("\tWorker Id: %d\n", temp->clientWorker->idWorkers[i]);
		}
		printf("Counter: %d\n", temp->clientWorker->counter);
		printf("Determinant: %d\n", temp->clientWorker->det);
		printf("Is ready: %s\n", temp->clientWorker->ready ? "true" : "false");
		printf("Next node: %d\n", temp->next);
		temp = temp->next;
	}
	printf("\n");

	LeaveCriticalSection(&pList->critSec);
}


void AddOnEnd(listStruct* pList, clientWorkerStruct* clientWorker)
{
	EnterCriticalSection(&pList->critSec);
	if (pList->head == NULL) {

		node_t* newNode = (node_t*)malloc(sizeof(node_t));
		if (newNode == NULL) {
			printf("Not enough memory.\n");
			LeaveCriticalSection(&pList->critSec);
			return;
		}

		newNode->clientWorker = clientWorker;
		newNode->next = NULL;

		AddHead(pList, newNode);

		LeaveCriticalSection(&pList->critSec);
	}
	else {

		node_t* current = pList->head;


		while (current->next != NULL) {
			current = current->next;
		}

		node_t* newNode = (node_t*)malloc(sizeof(node_t));
		if (newNode == NULL) {
			printf("Not enough memory.\n");
			LeaveCriticalSection(&pList->critSec);
			return;
		}

		newNode->clientWorker = clientWorker;
		newNode->next = NULL;

		current->next = newNode;
		LeaveCriticalSection(&pList->critSec);
	}
}


void AddHead(listStruct* pList, node_t *node)
{
	EnterCriticalSection(&pList->critSec);
	node->next = pList->head;
	pList->head = node;
	LeaveCriticalSection(&pList->critSec);
}

// el dobro, tj kako ide ?
void UpdateNode(listStruct* pList, clientWorkerStruct* clientWorker)
{
	clientWorkerStruct* updateNode = NULL;

	EnterCriticalSection(&pList->critSec);

	for (node_t** current = &pList->head; *current; current = &(*current)->next) {
		if ((*current)->clientWorker->idClient == clientWorker->idClient) {
			updateNode = (*current)->clientWorker;
			node* next = (*current)->next;
		}
	}

	LeaveCriticalSection(&pList->critSec);
}

clientWorkerStruct* RemoveByID(listStruct* pList, int clientId)
{
	clientWorkerStruct* returnVal = NULL;

	EnterCriticalSection(&pList->critSec);

	for (node_t** current = &pList->head; *current; current = &(*current)->next) {
		if ((*current)->clientWorker->idClient == clientId) {

			returnVal = (*current)->clientWorker;
			node* next = (*current)->next;
			free(*current);
			*current = next;

			break;
		}
	}

	LeaveCriticalSection(&pList->critSec);

	return returnVal;
}


bool IsListEmpty(listStruct* pList)
{
	EnterCriticalSection(&pList->critSec);
	if (pList->head == NULL) {
		LeaveCriticalSection(&pList->critSec);
		return true;
	}

	LeaveCriticalSection(&pList->critSec);
	return false;
}


clientWorkerStruct* RemoveFirstElement(listStruct* pList)
{
	clientWorkerStruct* networkData = NULL;

	node_t* next_node = NULL;

	EnterCriticalSection(&pList->critSec);

	if (pList->head == NULL) {
		LeaveCriticalSection(&pList->critSec);
		return networkData;
	}

	next_node = pList->head->next;
	networkData = pList->head->clientWorker;
	free(pList->head);
	pList->head = next_node;

	LeaveCriticalSection(&pList->critSec);

	return networkData;
}