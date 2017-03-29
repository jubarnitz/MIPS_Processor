#ifndef FLUSH_REG_H_INCLUDED
#define FLUSH_REG_H_INCLUDED

#include <stdio.h>
#include "pipe_reg.h"

void Flush_IF_ID(IF_ID *);
void Flush_ID_EX(ID_EX *);
void Flush_EX_MEM(EX_MEM *);
void Flush_MEM_WB(MEM_WB *);

#endif
