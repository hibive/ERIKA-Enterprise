#include "ee.h"


//////////////////////////////////////////////////////////////////////////////
//  
//   Kernel ( CPU 0 )
//  
/////////////////////////////////////////////////////////////////////////////
    // Definition of task's body
    DeclareTask(Task1);
    DeclareTask(Task2);
    DeclareTask(Task3);
    DeclareTask(Task4);
    DeclareTask(Task5);
    DeclareTask(Task6);

    const EE_FADDR EE_hal_thread_body[EE_MAX_TASK] = {
        (EE_FADDR)EE_oo_thread_stub,		 // thread Task1
        (EE_FADDR)EE_oo_thread_stub,		 // thread Task2
        (EE_FADDR)EE_oo_thread_stub,		 // thread Task3
        (EE_FADDR)EE_oo_thread_stub,		 // thread Task4
        (EE_FADDR)EE_oo_thread_stub,		 // thread Task5
        (EE_FADDR)EE_oo_thread_stub 		 // thread Task6

    };

    EE_UINT32 EE_terminate_data[EE_MAX_TASK];

    // ip of each thread body (ROM)
    const EE_FADDR EE_terminate_real_th_body[EE_MAX_TASK] = {
        (EE_FADDR)FuncTask1,
        (EE_FADDR)FuncTask2,
        (EE_FADDR)FuncTask3,
        (EE_FADDR)FuncTask4,
        (EE_FADDR)FuncTask5,
        (EE_FADDR)FuncTask6
    };
    // ready priority
    const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK] = {
        0x1,		 // thread Task1
        0x2,		 // thread Task2
        0x4,		 // thread Task3
        0x8,		 // thread Task4
        0x10,		 // thread Task5
        0x20 		 // thread Task6
    };

    const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK] = {
        0x1,		 // thread Task1
        0x2,		 // thread Task2
        0x4,		 // thread Task3
        0x8,		 // thread Task4
        0x10,		 // thread Task5
        0x20 		 // thread Task6
    };

    // thread status
    EE_TYPESTATUS EE_th_status[EE_MAX_TASK] = {
        SUSPENDED,
        SUSPENDED,
        SUSPENDED,
        SUSPENDED,
        SUSPENDED,
        SUSPENDED
    };

    // next thread
    EE_TID EE_th_next[EE_MAX_TASK] = {
        EE_NIL,
        EE_NIL,
        EE_NIL,
        EE_NIL,
        EE_NIL,
        EE_NIL
    };

    // The first stacked task
    EE_TID EE_stkfirst = EE_NIL;

    // system ceiling
    EE_TYPEPRIO EE_sys_ceiling= 0x0000;

    // remaining nact: init= maximum pending activations of a Task
    // MUST BE 1 for BCC1 and ECC1
    EE_TYPENACT   EE_th_rnact[EE_MAX_TASK] =
        { 1, 1, 1, 1, 1, 1};

    // First task in the ready queue (initvalue = EE_NIL)
    EE_TID EE_rq_first  = EE_NIL;

    #ifndef __OO_NO_CHAINTASK__
        // The next task to be activated after a ChainTask. initvalue=all EE_NIL
        EE_TID EE_th_terminate_nextask[EE_MAX_TASK] = {
            EE_NIL,
            EE_NIL,
            EE_NIL,
            EE_NIL,
            EE_NIL,
            EE_NIL
        };
    #endif



//////////////////////////////////////////////////////////////////////////////
//  
//   Event handling
//  
/////////////////////////////////////////////////////////////////////////////
    EE_TYPEEVENTMASK EE_th_event_active[EE_MAX_TASK] =
        { 0, 0, 0, 0, 0, 0};    // thread event already active

    EE_TYPEEVENTMASK EE_th_event_waitmask[EE_MAX_TASK] =
        { 0, 0, 0, 0, 0, 0};    // thread wait mask

    EE_TYPEBOOL EE_th_waswaiting[EE_MAX_TASK] =
        { 0, 0, 0, 0, 0, 0};

    EE_TYPEPRIO EE_th_is_extended[EE_MAX_TASK] =
        { 0, 0, 1, 0, 0, 0};



//////////////////////////////////////////////////////////////////////////////
//  
//   Mutex
//  
/////////////////////////////////////////////////////////////////////////////
    // This is the last mutex that the task has locked. This array
    //    contains one entry for each task. Initvalue= all -1. at runtime,
    //    it points to the first item in the EE_resource_stack data structure.
    EE_UREG EE_th_resource_last[EE_MAX_TASK] =
        { (EE_UREG) -1, (EE_UREG) -1, (EE_UREG) -1, (EE_UREG) -1, (EE_UREG) -1, (EE_UREG) -1};

    // This array is used to store a list of resources locked by a
    //    task. there is one entry for each resource, initvalue = -1. the
    //    list of resources locked by a task is ended by -1.
    EE_UREG EE_resource_stack[EE_MAX_RESOURCE] = { 0 };

    // Only in extended status or when using ORTI with resources; for
    //    each resource, a flag is allocated to see if the resource is locked or
    //    not.  Note that this information cannot be easily knew from the previous
    //    two data structures. initvalue=0
    EE_TYPEBOOL EE_resource_locked[EE_MAX_RESOURCE] = { 0 };

    const EE_TYPEPRIO EE_resource_ceiling[EE_MAX_RESOURCE]= { 0 };

    EE_TYPEPRIO EE_resource_oldceiling[EE_MAX_RESOURCE] = { 0 };

//////////////////////////////////////////////////////////////////////////////
//  
//   AppMode
//  
/////////////////////////////////////////////////////////////////////////////
    EE_TYPEAPPMODE EE_ApplicationMode;


#include "ee.h"


//////////////////////////////////////////////////////////////////////////////
//  
//   Stack definition for Lattice mico 32
//  
/////////////////////////////////////////////////////////////////////////////
    #define STACK_1_SIZE 128 // size = 512 bytes 
    #define STACK_2_SIZE 128 // size = 512 bytes 

#ifndef DEBUG_STACK
    int EE_e200z7_stack_1[STACK_1_SIZE];	/* Task 1 (Task2) */
    int EE_e200z7_stack_2[STACK_2_SIZE];	/* Task 2 (Task3) */
#else
    int EE_e200z7_stack_1[STACK_1_SIZE + E200Z7_STACK_DEBUG_LEN] = 
            E200Z7_STACK_INIT(STACK_1_SIZE);	/* Task 1 (Task2) */
    int EE_e200z7_stack_2[STACK_2_SIZE + E200Z7_STACK_DEBUG_LEN] = 
            E200Z7_STACK_INIT(STACK_2_SIZE);	/* Task 2 (Task3) */
#endif

    const EE_UREG EE_std_thread_tos[EE_MAX_TASK+1] = {
        0,	 /* dummy*/
        0,	 /* Task1*/
        1,	 /* Task2*/
        2,	 /* Task3*/
        0,	 /* Task4*/
        0,	 /* Task5*/
        0 	 /* Task6*/
    };

    struct EE_TOS EE_e200z7_system_tos[3] = {
        {0},	/* Task   (dummy), Task 0 (Task1), Task 3 (Task4), Task 4 (Task5), Task 5 (Task6) */
        {(EE_ADDR)(&EE_e200z7_stack_1[STACK_1_SIZE - 2])},	/* Task 1 (Task2) */
        {(EE_ADDR)(&EE_e200z7_stack_2[STACK_2_SIZE - 2])} 	/* Task 2 (Task3) */
    };

    EE_UREG EE_e200z7_active_tos = 0; /* dummy */