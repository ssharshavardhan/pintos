Notes - 
#Assignment 0
Inline Functions : We can instruct the compiler to insert the code of a function into the code of its callers, to the point where actually the call is to be made. Such functions are inline functions.

`objdump -f a.out` shows critical info about the executable
elf file system -> Executable & Linking Format

#Assignment 1
At any given time, exactly one thread runs and the rest, if any, become inactive.

"loader.h"  & loader.S are the only files not part of the pintos kernel, cause they locates & loads the kernel on disc & jumps to start.S

Files we can Generally modify
"init.h" is the kernel main program : can initialize own vars/functions here

"thread.c" & "thread.h" :  Much of the work will take place in these files
"timer.h"
    System timer that ticks, by default, 100 times per second. We will modify this code in this project. 
    We will ignore all issues that concern with the creation, resource allocation and termination of a thread to focus on how a thread is scheduled to run on a computer processor.

 >> Use thread_yield() to make the running thread ready.

We also use "list.h"  

#cscopes & ctags - as we need some sort of intellisense like other IDEs like Eclipse, Netbeans 

Cscope builds a database of funtions and variables. One can navigate to declarations, function call, function definitions and all using cscope commands.  For using it through vi editor,we need ctags


Awesome function -> Struct[ elem ]
#define list_entry(LIST_ELEM, STRUCT, MEMBER)           \
        ((STRUCT *) ((uint8_t *) &(LIST_ELEM)->next     \
                     - offsetof (STRUCT, MEMBER.next)))