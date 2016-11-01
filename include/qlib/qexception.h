// Text type: Unix/LF; TAB=4, tab-to-spaces; Oct.18, 2015 
/**@file qexception.h
 * @ingroup qin
 * Implementations of exception related data structs and functions.
 *
 * @author Maxwell.Qin (QinLi qinliok@126.com)
 *
 * @code
 * PACKAGE:   qin
 * REVISION:  1.0
 * COPYRIGHT: Qin-domain
 * HISTORY:
 *     October, 18th, 2015  Rev1.0
 *             - Implement of try-catch, try_end and Except_raise.
 * @endcode
 */

#ifndef _QLIB_EXCEPTION_H_
#define _QLIB_EXCEPTION_H_

#include <setjmp.h> //for jmp_buf()
#include <stdio.h> //for fprintf()
#include <stdlib.h> //for abort()

typedef struct Message_T  Except_Msg;
typedef struct ExpFrame_T Except_Frame;

extern const Except_Msg Mem_AllocFailed;
extern const Except_Msg Mem_FreeFailed;
extern const Except_Msg Exc_General;

struct Message_T{
    char* m_msg;
};

struct ExpFrame_T{
    Except_Frame *m_prev;
    const Except_Msg *m_excMsg;
    jmp_buf       m_env;
    const char   *m_file;
    int           m_line;
};

enum { Except_entered = 0, Except_raised, Except_handled};

extern Except_Frame *Except_stack;
#define RERAISE Except_raise(Except_frame.m_excMsg, \
        Except_frame.m_file, Except_frame.m_line)

//Except_stack is a critical resource, there should be lock.
#define try do{\
    volatile int Except_flag; \
    Except_Frame Except_frame; \
    Except_frame.m_prev = Except_stack; \
    Except_stack = &Except_frame; \
    Except_flag = setjmp(Except_frame.m_env);\
    if(Except_flag == Except_entered){
#define catch_excp(e) \
        if(Except_flag == Except_entered) \
            Except_stack = Except_stack->m_prev;\
    }else if(Except_frame.m_excMsg == &(e)){\
        Except_flag = Except_handled;
#define uncaught_excp \
        if(Except_flag == Except_entered)\
            Except_stack = Except_stack->m_prev; \
    }else{ \
        Except_flag = Except_handled;
#define try_end \
        if(Except_flag == Except_entered) \
            Except_stack = Except_stack->m_prev; \
        }\
        if(Except_flag == Except_raised){\
            fprintf(stderr, "Unhandled Exception");\
            if(Except_frame.m_file && Except_frame.m_line >0);\
                fprintf(stderr, " raised at %s %d line",\
                    Except_frame.m_file, Except_frame.m_line);\
            fprintf(stderr, ".\nProgram exit().\n");\
            exit(1);}\
}while(0)

void Except_raise(const Except_Msg *msg, int line, const char* file);

#endif //!_QLIB_EXCEPTION_H_
