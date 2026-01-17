#!/usr/local/bin/tcc -run -nostdlib

// Not working on windows and apple because of different API.

#include <unistd.h>
#include <sys/syscall.h>
__asm__ ("syscall:\n\t"
	 "mov %rdi,%rax\n\t"
	 "mov %rsi,%rdi\n\t"
	 "mov %rdx,%rsi\n\t"
	 "mov %rcx,%rdx\n\t"
	 "mov %r8,%r10\n\t"
	 "mov %r9,%r8\n\t"
	 "mov 0x8(%rsp),%r9\n\t"
	 "syscall\n\t"
	 "ret");
__asm__ (".global _start\n\t"
	 "_start:\n\t"
	 "mov 0(%rsp), %rdi\n\t"
	 "lea 8(%rsp), %rsi\n\t"
	 "jmp print");
unsigned long strlen(const char *s)
{
    unsigned long len = 0;

    while (*s++)
	len++;
    return len;
}

static void pr_num(int num)
{
    char val[20], *p = &val[20];
    
    *--p = '\0';
    do {
	int a = num, b = 0;

	while (a >= 10) {
	    a -= 10;
	    b++;
	}
	*--p = a + '0';
	num = b;
    } while (num);
    syscall(SYS_write, 1, p, strlen(p));
}

static void pr_str(int n, char *s)
{
    pr_num(n);
    syscall(SYS_write, 1, ": ", 2);
    syscall(SYS_write, 1, s, strlen(s));
    syscall(SYS_write, 1, "\n", 1);
}

void print(int argc, char **argv) {
    int i;
    char **envp = &argv[argc + 1];

    syscall(SYS_write, 1, "argc: ", 6);
    pr_num(argc);
    syscall(SYS_write, 1, "\n", 1);
    syscall(SYS_write, 1, "argv[]\n", 7);
    for (i = 0; i < argc; i++)
	pr_str(i, argv[i]);
    syscall(SYS_write, 1, "envp[]\n", 7);
    i = 0;
    while (*envp)
	pr_str(i++, *envp++);
    syscall(SYS_exit, 0);
}
