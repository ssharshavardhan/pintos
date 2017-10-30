#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
 /* My Implementation */
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/process.h"
// UP03 - 
#include <list.h>
#include "filesys/file.h" // file handling functions
#include "filesys/filesys.h" // init/create/done
#include "threads/palloc.h"
// UP03 +
#include "devices/input.h" // contains getchar and putchar eqv functions
#include "threads/synch.h"
 /* == My Implementation */


static void syscall_handler (struct intr_frame *);

/* My Implementation */

//static void sys_write (int *ret,int fd, const void *buffer, unsigned length);
//static void sys_exit (int *ret, int status);
//typedef void (*handler) (int *,uint32_t, uint32_t, uint32_t);
typedef int pid_t;

static int sys_write (int fd, const void *buffer, unsigned length);
static int sys_exit (int status);
static int sys_halt (void);
static int sys_create (const char *file, unsigned initial_size);
static int sys_open (const char *file);
static int sys_close (int fd);
static int sys_read (int fd, void *buffer, unsigned size);
static int sys_exec (const char * cmd);
static int sys_wait (pid_t pid);
static int sys_remove (const char *file);

// UP03 - 
static struct file *find_file_by_fd (int fd);
static struct fd_elem *find_fd_elem_by_fd (int fd);
static int alloc_fid (void);
static struct fd_elem *find_fd_elem_by_fd_in_process (int fd);
static struct file *find_file_by_fd_in_process (int fd);
// UP03 +
static int sys_filesize (int fd);
static int sys_tell (int fd);
static int sys_seek (int fd, unsigned pos);

typedef int (*handler) (uint32_t, uint32_t, uint32_t);
static handler syscall_vec[128];
static struct lock file_lock;
// UP03 - 
struct fd_elem
  {
    int fd;
    struct file *file;
    struct list_elem elem;
    struct list_elem thread_elem;
  };
  
static struct list file_list;


/* == My Implementation */

void syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
   
  /* My Implementation */
  // All the syscall numbers obtained from syscall-nr.h
  syscall_vec[SYS_CREATE] = (handler)sys_create;
  syscall_vec[SYS_CLOSE] = (handler)sys_close;
  syscall_vec[SYS_EXIT] = (handler)sys_exit;
  syscall_vec[SYS_EXEC] = (handler)sys_exec;
  syscall_vec[SYS_HALT] = (handler)sys_halt;
  syscall_vec[SYS_OPEN] = (handler)sys_open;
  syscall_vec[SYS_READ] = (handler)sys_read;
  syscall_vec[SYS_WAIT] = (handler)sys_wait;
  syscall_vec[SYS_WRITE] = (handler)sys_write;
// UP03 + 
  syscall_vec[SYS_FILESIZE] = (handler)sys_filesize;
  syscall_vec[SYS_SEEK] = (handler)sys_seek;
  syscall_vec[SYS_TELL] = (handler)sys_tell;
  syscall_vec[SYS_REMOVE] = (handler)sys_remove;

  list_init (&file_list);
  lock_init(&file_lock);
   /* == My Implementation */
}

static void syscall_handler (struct intr_frame *f )
{
    handler h;
    int *p;
    int ret;
    p = f->esp;
    if (!is_user_vaddr (p))
        goto terminate;  
    if (*p < SYS_HALT || *p > SYS_INUMBER)
        goto terminate;
    h = syscall_vec[*p];
    if (!(is_user_vaddr (p + 1) && is_user_vaddr (p + 2) && is_user_vaddr (p + 3)))
        goto terminate; 
    // Makes a call to the function at frame's stack pointer
    ret = h (*(p + 1), *(p + 2), *(p + 3));
    f->eax = ret;  
    return;  
    
    terminate:
    sys_exit (-1);
    }

static int sys_write (int fd, const void *buffer, unsigned length)
{
    // if (fd == 1)
  /* My Implementation */
// UP03 -
    if (fd == STDOUT_FILENO) //stdout
        putbuf (buffer, length);
    else if(fd == STDIN_FILENO) //stdin
        return -1;
// UP03 + 
    else if (!is_user_vaddr (buffer))
      sys_exit (-1);
// UP03 -
    else{
      struct file *f;
      f = find_file_by_fd(fd);
      if(!f)
        return -1;
      return file_write(f,buffer,length);
    }

    return length;
   /* == My Implementation */
}

static int sys_exit (int status)
{
  /* My Implementation */
  struct thread *t;
  struct list_elem *l;
  t = thread_current ();
  

  /* Close all the files */
  while (!list_empty (&t->files))
    {
      l = list_pop_front (&t->files);
      sys_close (list_entry (l, struct fd_elem, thread_elem)->fd);
    }
  
  t->ret_status = status;

  /* == My Implementation */
  thread_exit ();
  return -1;
}

static int
sys_halt (void)
{
  power_off ();
}

static int
sys_create (const char *file, unsigned initial_size)
{
  // return -1;
  /*  My Implementation */
  if (!file)
    return sys_exit (-1);
  return filesys_create (file, initial_size);
  /* == My Implementation */
}

static int
sys_open (const char *file)
{
  
  /*  My Implementation */

  struct file *f;
  struct fd_elem *fde;
  
  if (!file) /* file == NULL */
    return -1;
  
  f = filesys_open (file);
  if (!f) /* Bad file name */
    return -1;
    
  fde = (struct fd_elem *)palloc_get_page (0);
  if (!fde) /* Not enough memory */
    {
      file_close (f);
      return -1;
    }
    
  fde->file = f;
  fde->fd = alloc_fid ();
  list_push_back (&file_list, &fde->elem);
  list_push_back (&thread_current ()->files, &fde->thread_elem);
  return fde->fd;
  /* == My Implementation */
  
  // return -1;
}

static int
sys_close(int fd)
{
  /*  My Implementation */
   struct fd_elem *f;
  
  f = find_fd_elem_by_fd (fd);
  
  if (!f) /* Bad fd */
    return -1;
  // free memory used by f's file
  file_close (f->file);

  // remove the thread from file 
  list_remove (&f->elem);
  list_remove (&f->thread_elem);
  
  // free memory used by fd_elem
  palloc_free_page (f);
  return 0;
  /* == My Implementation */
  
  // return -1;
}

static int
sys_read (int fd, void *buffer, unsigned size)
{
  // return -1;
  /*
Reads size bytes from the file open as fd into buffer.
Returns the number of bytes actually read (0 at end of file), 
or -1 if the file could not be read (due to a condition other than end of file).
Fd 0 reads from the keyboard using input_getc().

  */
  /* My Implementation */
  struct file * f;
  unsigned i;
  
  if (fd == STDIN_FILENO) /* stdin */
    {
      for (i = 0; i != size; ++i)
        *(uint8_t *)(buffer + i) = input_getc ();
      return size;
    }
  else if (fd == STDOUT_FILENO) /* stdout */
    return -1;
  else if (!is_user_vaddr (buffer)) /* bad ptr */
    sys_exit (-1);
  else
    {
      f = find_file_by_fd (fd);
      if (!f)
        return -1;
      return file_read (f, buffer, size);
    }
    return -1; //shouldn't reach here
  /* == My Implementation */
}
static int
sys_exec (const char * cmd)
{
  if (!cmd)
    return -1;
  return process_execute (cmd);
}

static int
sys_wait (pid_t pid)
{
  return process_wait (pid);
}

static int sys_remove (const char *file)
 {
   if (!file)
     return false;
   if (!is_user_vaddr (file))
     sys_exit (-1);
     
   return filesys_remove (file);
 }
//custom functions -


//linear search for fd in the (global) file_list 
static struct fd_elem *
find_fd_elem_by_fd (int fd)
{
  struct fd_elem *ret;
  struct list_elem *l;
  
  for (l = list_begin (&file_list); l != list_end (&file_list); l = list_next (l))
    {
      ret = list_entry (l, struct fd_elem, elem);
      if (ret->fd == fd)
        return ret;
    }
    
  return NULL;
}


// search for fd, then return its -> file.
static struct file *
find_file_by_fd (int fd)
{
  struct fd_elem *ret;
  
  ret = find_fd_elem_by_fd (fd);
  if (!ret)
    return NULL;
  return ret->file;
}


// give a unique file id.
static int
alloc_fid (void)
{
  static int fid = 2; //initialize to 2
  return fid++;
}


static int
sys_filesize (
int fd)
{
  struct file *f;
  
  f = find_file_by_fd (fd);
  if (!f)
    return -1;
  return file_length (f);
}

static int
sys_tell (int fd)
{
  struct file *f;
  
  f = find_file_by_fd (fd);
  if (!f)
    return -1;
  return file_tell (f);
}

static int
sys_seek (int fd, unsigned pos)
{
  struct file *f;
  
  f = find_file_by_fd (fd);
  if (!f)
    return -1;
  file_seek (f, pos);
  return 0; /* Not used */
}

static struct fd_elem *
 find_fd_elem_by_fd_in_process (int fd)
 {
   struct fd_elem *ret;
   struct list_elem *l;
   struct thread *t;
   t = thread_current ();
   for (l = list_begin (&t->files); l != list_end (&t->files); l = list_next (l))
     {
       ret = list_entry (l, struct fd_elem, elem);
       if (ret->fd == fd)
         return ret;
     }
     
   return NULL;
 }
 
 static struct file *
 find_file_by_fd_in_process (int fd)
 {
   struct fd_elem *ret; 
   ret = find_fd_elem_by_fd_in_process (fd);
   if (!ret)
     return NULL;
   return ret->file;
 }