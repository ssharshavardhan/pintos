Notes - 
#Assignment 0
Inline Functions : We can instruct the compiler to insert the code of a function into the code of its callers, to the point where actually the call is to be made. Such functions are inline functions.

`objdump -f a.out` shows critical info about the executable
elf file system -> Executable & Linking Format

#T01
 Total changes required
 devices/timer.c       |   42 +++++-
 threads/fixed-point.h |  120 ++++++++++++++++++
 threads/synch.c       |   88 ++++++++++++-
 threads/thread.c      |  196 ++++++++++++++++++++++++++----
 threads/thread.h      |   23 +++
 5 files changed, 440 insertions(+), 29 deletions(-)


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


#T02 = related to blocking and unblocking of the threads through synchronization primitives

Task 1 : Implement functions thread_set_priority() and thread_get_priority()
Task 2 : Add code so that sema_up operation releases the highest priority thread first.
>> struct thread wake_up = list_entry (list_pop_front (&sema->waiters), struct thread, elem);

Task 3 : (same as Task 2) Use of conditional variable
>> lock_try_acquire -> 
      lock->holder = thread_current ();
      list_push_back (&lock->holder->locks, &lock->holder_elem);
>> lock_acquire and lock_release need to be modified.

Task 4 : Implement priorities in locks using a list of queues, One list(of the locks it acquired) for each thread. 
	& a special list of blocked threads waiting to acquire the lock (sleep_list)
	waiting to acquire the lock <=> seeking <=> pointer to a lock it wants to acquire.

Task 5 : Implement Priority Donation >> acquired the lock <=> holding <=> pointer to lock it has acquired.

Note : 
 - when a priority-donor thread receives the lock it was seeking, it ends the donation of its priority to all threads who might have benefitted from its donation
 - Each lock has a priority defined by the highest priority of the thread seeking (but not holding it) to acquire the lock.
 - We can define 'donation_benefit' for each thread = Highest priority of lock it holds


We implement following functions - 

+timer_sleep function
+void sort_thread_list (struct list *l);
+void thread_set_priority_other (struct thread *curr, int new_priority, bool forced);
+void thread_yield_head (struct thread *curr);

We change following structures - 

struct thread -

-    int old_priority;
-    int64_t wakeup_at;

+    struct alarm alrm;                  /* alarm object */
+    int base_priority;                  /* priority before donate, if nobody donates, then it should be same as priority */
+    struct list locks;                  /* the list of locks that it holds */
+    bool donated;                       /* whether the thread has been donated priority */
+    struct lock *blocked;               /* by which lock this thread is blocked */



Priority Donation -
	added bool member 'donated' to struct thread
Locking
	- A list of locks that the thread holds(as many resources it is using)
	-added struct lock *blocked to get the lock by which thread is blocked.
-

What is sema_up and sema_down?
They do the signal & wait respectively and also sema_up will wake up(unblocked) the next blocked thread 
	what is thread_yield_head?
	-> it is exactly thread_yield, except for the fact that it takes 'cur' as an arg instead of using thread_current()
>What is a Deadlock situation?
- Three threads with prio 1,2,3. 1 & 2 are in ready list (not running yet), 3 needs a resource which is locked by 1, so its in waiting list. But now 1 cannot run unless 2 is completed. So here 2 will run its complete code and then 1 will finish with its resource and then only 3 can be selected.

Solution to deadlock?
- We use priority inversion. threads 3 and 1 if swap their priorities until 1 executes completely, will prevent 2 from running and the wait time for 3 will be reduced.


> Why do we need mutually-exclusive (mutex) locks and semaphores (avl process counts) together?
- locks are about controlling access to shared resources, while Semaphores about access to processor.
- locks prevent (avoid, actually) race conditions
-> but the deadlock still may happen 
- semas manage which thread(s) should run

	
>> Qs to answer
why added struct alarm alrm in thread.h?
...
Some more Qs about T02 here
thread_mlfqs -> a bool that tells if --mlfqs option was passed as arg (init.c)

outstanding_priority (struct list_elem *lhs, *rhs) -> a comparator function comparing priorities of locks in the list pointers lhs and rhs. Used in lock_acquire to reorder after thread_current() acquires the lock.
.
cond_priority (struct list_elem *lhs, *rhs) -> comparator for sema_priorities
.
-  
> What does thread_yield_head do ?
It uses >= instead of > i.e. thread_insert_less_head for inserting current blocked thread into readylist. There is less_tail as well, which uses >

#T03
the lock related functions in sync.c (?) which were backed up before T02 will be used here.


#UserProg - 
 
Total changes required - 
 threads/thread.c     |   13 
 threads/thread.h     |   26 +
 userprog/exception.c |    8 
 userprog/process.c   |  247 ++++++++++++++--
 userprog/syscall.c   |  468 ++++++++++++++++++++++++++++++-
 userprog/syscall.h   |    1 
 6 files changed, 725 insertions(+), 38 deletions(-)


#PF (Page-Fault Exception) 
-> One possible cause is when user prog accesses a non addressed memory
-> Or in kernel mode if you access unmapped usinger address

difference about Arg-parsing in Unix-like systems vs Pintos-
Unix supports complex features such as "Redirection" and Pipelining (Because it separates the args from executable name. Also, Unix checks for unsafe commands at shell level only, before passing it to Kernel), which are rather hard to implement in pintos. The same with PATH variable, which is left to the shell (an external program) is a better approach, while pintos will require the Kernel to look into the directories for the file during Initialization of a process.

progress.c, sys-call.c 

/**/
Notes Merger
-
useful tool = debug_backtrace

C implementation of .split() = strtok_r
char *save;
file_name = strtok_r (fn, " ", &save); // separated by spaces
//interrupt handler for sys calls
intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
// Before using system calls, we used this function to run programs (one thread per process)
static void run_task (char **argv)
/**/
-

Reading the comments on top of functions - 
syscall.c -> mainly has the asm code for calling sys having 0-3 args
syscall.h -> declares the halt,exec and write etc functions

use of ALARM_MAGIC -> /* Check if the alrm is actually an alarm */
     return (alrm != NULL && alrm->magic == ALARM_MAGIC);
-     
Some Commits - 
T03 - 038ec24
T02 - 7001729ede
T01 - 89b0488f9f8
First Commit - b16d8de34




#UP03 - System calls for file operations
1. First handle files in the various syscalls mentioned at https://jeason.gitbooks.io/pintos-reference-guide-sysu/content/userprog-systemcall.html
syscall-nr.h contains all sys call numbers

Note : We'll later have to revise this defn -
The 128 entries limit, per-process open file table - struct list files;

After completion of UP03, for UP04 we are left with only -
exec() , wait() and Denying Writes to Executables.



Robustness file ->

Robustness of system calls:
- Test robustness of file descriptor handling.
2	close-stdin
2	close-stdout
2	close-bad-fd
2	close-twice
2	read-bad-fd
2	read-stdout
2	write-bad-fd
2	write-stdin
2	multi-child-fd

- Test robustness of pointer handling.
3	create-bad-ptr
3	exec-bad-ptr
3	open-bad-ptr
3	read-bad-ptr
3	write-bad-ptr

- Test robustness of buffer copying across page boundaries.
3	create-bound
3	open-boundary
3	read-boundary
3	write-boundary

- Test handling of null pointer and empty strings.
2	create-null
2	open-null
2	open-empty

- Test robustness of system call implementation.
3	sc-bad-arg
3	sc-bad-sp
5	sc-boundary
5	sc-boundary-2

- Test robustness of "exec" and "wait" system calls.
5	exec-missing
5	wait-bad-pid
5	wait-killed

- Test robustness of exception handling.
1	bad-read
1	bad-write
1	bad-jump
1	bad-read2
1	bad-write2
1	bad-jump2

