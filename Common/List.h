#pragma once

#include "Common.h"

listStruct* InitList();

void PrintList(listStruct* pList);

void AddOnEnd(listStruct* pList, clientWorkerStruct* clientWorker);

void AddHead(listStruct* pList, node_t *node);

void UpdateNode(listStruct* pList, clientWorkerStruct* clientWorker);

clientWorkerStruct* RemoveByID(listStruct* pList, int clientId);

bool IsListEmpty(listStruct* pList);

clientWorkerStruct* RemoveFirstElement(listStruct* pList);

