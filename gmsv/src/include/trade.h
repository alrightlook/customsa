#ifndef __TRADE_H__
#define __TRADE_H__
#include "version.h"
#include "common.h"
#include "util.h"
#include "net.h"
 
int TRADE_Search(int fd, int meindex, char* message);
    
void CHAR_Trade(int fd, int index, char* message);

#endif 
