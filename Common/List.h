#pragma once

#include "Common.h"

listStruct* InitList();

void PrintList(listStruct* pList);

void AddOnEnd(listStruct* pList, clientWorkerStruct* clientWorker);

void AddHead(listStruct* pList, node_t *node);

clientWorkerStruct* RemoveByID(listStruct* pList, int clientId);

