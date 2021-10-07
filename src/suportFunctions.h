/*
	autor: David Triska, xtrisk05
*/

#ifndef SUPORTFUNCTIONS_H_INCLUDE
#define SUPORTFUNCTIONS_H_INCLUDE

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include "dString.h"
#include "hashTable.h"
#include <netdb.h>


int isNBitOfWordSet(char, int);
int analyzeDnsPacket(char*, int, dStr*);
int buildErrResponsePacket(char*, char*, int);
int handleArguments(int, char**, int*, dStr*, dStr*);

#endif