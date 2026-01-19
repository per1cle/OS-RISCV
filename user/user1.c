#define SYS_WRITE   64
#define SYS_EXIT    93
#define SYS_YIELD   124
#define SYS_GETTIME 169
static inline long syscall(long num, long arg0, long arg1, long arg2) {
    register long a7 __asm__("a7") = num;
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    __asm__ __volatile__(
        "ecall"
        : "+r"(a0), "+r"(a7)
        : "r"(a7), "r"(a1), "r"(a2)
        : "memory"
    );
    return a0;
}
static inline long sys_write(int fd, const char *buf, long len) {
    return syscall(SYS_WRITE, fd, (long)buf, len);
}
static inline long sys_yield(void) {
    return syscall(SYS_YIELD, 0, 0, 0);
}
static inline long sys_gettime(void) {
    return syscall(SYS_GETTIME, 0, 0, 0);
}
static inline void sys_exit(int status) {
    syscall(SYS_EXIT, status, 0, 0);
    while(1);
}
long strlen(const char *s) {
    long len = 0;
    while(s[len]) len++;
    return len;
}
void puts(const char *s) {
    sys_write(1, s, strlen(s));
}
void _start(void) {
    puts("USER1: Hello from user program 1!\n");
    int count = 0;
    while(count < 100000) {  
        if(count % 1000 == 0) {
            puts("USER1: Running...  ");
            long time = sys_gettime() % 1000;  // Only last 3 digits
            char buf[10];
            buf[0] = '[';
            buf[1] = 't';
            buf[2] = '=';
            buf[3] = '0' + ((time / 100) % 10);
            buf[4] = '0' + ((time / 10) % 10);
            buf[5] = '0' + (time % 10);
            buf[6] = ']';
            buf[7] = '\n';
            sys_write(1, buf, 8);
        }
        for(volatile int i = 0; i < 50000; i++);
        sys_yield();  
        count++;
    }
    puts("USER1: Exiting...\n");
    sys_exit(0);  
}