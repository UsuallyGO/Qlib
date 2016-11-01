// Text type: Unix/LF; TAB=4, tab-to-spaces; Oct.18, 2015 
/**@file exception.c
 * Implements routines defined in qexception.h
 *
 * @author Maxwell.Qin (QinLi qinliok@126.com)
 *
 * @code
 * PACKAGE:   qin
 * REVISION:  1.0
 * COPYRIGHT: Qin-domain
 * HISTORY:
 *     October, 18th, 2015  Rev1.0
 *             - Implement Except_raise function.
 * @endcode
 */
#include <stdlib.h> //abort()
#include <stdio.h> //fprintf
#include <setjmp.h> //longjmp()
#include "qlib/qexception.h"

Except_Frame *Except_stack = NULL;
const Except_Msg Exc_General = { "General exception." };

void Except_raise(const Except_Msg *msg, int line, const char* file)
{
    Except_Frame *p = Except_stack;

    if(msg == NULL)
        msg = &Exc_General;

    if(p == NULL) 
    {
        fprintf(stderr, "Uncaught exception");
        if(msg->m_msg)
            fprintf(stderr, " %s", msg->m_msg);
        else
            fprintf(stderr, " at 0x%p", msg);
        if(file && line > 0)
            fprintf(stderr, " raised at %s:%d\n", file, line);
        fprintf(stderr, "aborting...\n");
        fflush(stderr);
        exit(1);
    }
    p->m_excMsg = msg;
    p->m_file = file;
    p->m_line = line;
    Except_stack = Except_stack->m_prev; //once exception thrown, pops from stack
    longjmp(p->m_env, Except_raised);
}
