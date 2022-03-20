/* $Revision: 1.2 $ */

#ifndef _SEM_H_
#define _SEM_H_

#include "common.h"
#include "kern.h"

void            sem_init(sem_t * sem);
void            sem_p(sem_t * sem);
int             sem_v(sem_t * sem);

#endif
