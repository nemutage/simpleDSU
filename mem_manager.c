#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/stat.h>
#include <unistd.h>
#include "tool.h"  



long int alloc_mmap_area(pid_t pid, int size){
  int status;
  long int rax;
  struct user_regs_struct regs, oregs;
  char cord[] = {0x0f, 0x05 ,0xcc};
  char original_cord[4]; 

  ptrace(PTRACE_GETREGS, pid, NULL, &oregs);
  regs = oregs;
  get_data(pid, regs.rip, original_cord, 3);

  regs.r9 = 0;
  regs.r8 = -1;
  regs.r10 = 34;
  regs.rax = 9;
  regs.rdx = 7;
  regs.rsi = size;
  regs.rdi = 0;
  
  ptrace(PTRACE_SETREGS, pid, NULL, &regs);
  set_data(pid, regs.rip, cord, 3);

  ptrace(PTRACE_CONT, pid, NULL, NULL);
  waitpid(pid, &status, 0);

  ptrace(PTRACE_GETREGS, pid, NULL, &regs);
  rax = (long int)regs.rax;

  ptrace(PTRACE_SETREGS, pid, NULL, &oregs);
  set_data(pid, oregs.rip, original_cord, 3);
  
  return rax;  
}


