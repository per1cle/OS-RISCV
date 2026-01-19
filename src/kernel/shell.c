#include "kernel_lib.h"
#include "proc.h"
#include "mm.h"
#define CMD_BUFFER_SIZE 128
static int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}
static int atoi(const char *s) {
    int result = 0;
    int sign = 1;
    if (*s == '-') {
        sign = -1;
        s++;
    }
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return result * sign;
}
static void print_int(int n) {
    if (n < 0) {
        kernel_putchar('-');
        n = -n;
    }
    if (n == 0) {
        kernel_putchar('0');
        return;
    }
    char buf[16];
    int i = 0;
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i > 0) {
        kernel_putchar(buf[--i]);
    }
}
static void cmd_help(void) {
    kernel_putstring("\nComenzile disponibile:\n");
    kernel_putstring("  help          - Afiseaza acest mesaj\n");
    kernel_putstring("  ps            - Afiseaza procesele active\n");
    kernel_putstring("  uptime        - Afiseaza tick counter si uptime\n");
    kernel_putstring("  mem           - Afiseaza statistici memorie (heap)\n");
    kernel_putstring("  kill <pid>    - Termina procesul cu PID-ul specificat\n");
    kernel_putstring("  exec <prog>   - Lanseaza un program (user1/user2)\n");
    kernel_putstring("\n");
}
static void cmd_ps(void) {
    extern struct proc proc_table[];
    extern int proc_actual;
    kernel_putstring("\nPID  STATE    CPU_TIME\n");
    kernel_putstring("---  -------  --------\n");
    for (int i = 0; i < PROC_MAX; i++) {
        if (proc_table[i].state != UNUSED) {
            print_int(proc_table[i].pid);
            kernel_putstring("    ");
            if (proc_table[i].state == READY) {
                kernel_putstring("READY  ");
            } else if (proc_table[i].state == RUNNING) {
                kernel_putstring("RUNNING");
            }
            kernel_putstring("  ");
            print_int(proc_table[i].cpu_time);
            if (i == proc_actual) {
                kernel_putstring(" <--");
            }
            kernel_putchar('\n');
        }
    }
    kernel_putchar('\n');
}
static void cmd_uptime(void) {
    extern long counter_tick;
    kernel_putstring("\nSystem Uptime:\n");
    kernel_putstring("  Tick Counter: ");
    print_int(counter_tick);
    kernel_putstring("\n");
    long seconds = counter_tick / 100;
    kernel_putstring("  Uptime: ");
    print_int(seconds);
    kernel_putstring(" seconds\n\n");
}
static void cmd_mem(void) {
    extern void mm_stats(size_t *total, size_t *used, size_t *free_mem, int *blocks);
    size_t total, used, free_mem;
    int blocks;
    mm_stats(&total, &used, &free_mem, &blocks);
    kernel_putstring("\nMemory Statistics (Heap):\n");
    kernel_putstring("  Total size:  ");
    print_int(total);
    kernel_putstring(" bytes\n");
    kernel_putstring("  Used:        ");
    print_int(used);
    kernel_putstring(" bytes\n");
    kernel_putstring("  Free:        ");
    print_int(free_mem);
    kernel_putstring(" bytes\n");
    kernel_putstring("  Blocks:      ");
    print_int(blocks);
    kernel_putstring("\n\n");
}
static void cmd_kill(const char *arg) {
    extern struct proc proc_table[];
    if (!arg || *arg == '\0') {
        kernel_putstring("Eroare: Specificati un PID\n");
        kernel_putstring("Utilizare: kill <pid>\n");
        return;
    }
    int pid = atoi(arg);
    if (pid < 0 || pid >= PROC_MAX) {
        kernel_putstring("Eroare: PID invalid (range: 0-");
        print_int(PROC_MAX - 1);
        kernel_putstring(")\n");
        return;
    }
    if (proc_table[pid].state == UNUSED) {
        kernel_putstring("Eroare: Procesul cu PID ");
        print_int(pid);
        kernel_putstring(" nu exista\n");
        return;
    }
    if (pid == 0) {
        kernel_putstring("Eroare: Nu poti termina idle task-ul!\n");
        return;
    }
    proc_table[pid].state = UNUSED;
    proc_table[pid].cpu_time = 0;
    kernel_putstring("Procesul ");
    print_int(pid);
    kernel_putstring(" a fost terminat\n");
}
static void cmd_exec(const char *arg) {
    extern void exec(const char *name);
    if (!arg || *arg == '\0') {
        kernel_putstring("Eroare: Specificati un program\n");
        kernel_putstring("Utilizare: exec <user1|user2>\n");
        return;
    }
    if (strcmp(arg, "user1") != 0 && strcmp(arg, "user2") != 0) {
        kernel_putstring("Eroare: Program necunoscut '");
        kernel_putstring(arg);
        kernel_putstring("'\n");
        kernel_putstring("Programe disponibile: user1, user2\n");
        return;
    }
    kernel_putstring("\nLansare ");
    kernel_putstring(arg);
    kernel_putstring("...\n");
    exec(arg);
    kernel_putstring("\n");  
}
static void process_command(char *cmd) {
    while (*cmd == ' ') cmd++;
    if (*cmd == '\0') {
        return;
    }
    char *arg = cmd;
    while (*arg && *arg != ' ') arg++;
    if (*arg) {
        *arg = '\0';  
        arg++;
        while (*arg == ' ') arg++;  
    }
    if (strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "ps") == 0) {
        cmd_ps();
    } else if (strcmp(cmd, "uptime") == 0) {
        cmd_uptime();
    } else if (strcmp(cmd, "mem") == 0) {
        cmd_mem();
    } else if (strcmp(cmd, "kill") == 0) {
        cmd_kill(arg);
    } else if (strcmp(cmd, "exec") == 0) {
        cmd_exec(arg);
    } else {
        kernel_putstring("Comanda necunoscuta: '");
        kernel_putstring(cmd);
        kernel_putstring("'\n");
        kernel_putstring("Tastati 'help' pentru lista de comenzi\n");
    }
}
void shell_run(void) {
    char cmd_buffer[CMD_BUFFER_SIZE];
    int cmd_pos = 0;
    kernel_putstring("\n");
    kernel_putstring("========================================\n");
    kernel_putstring("  OS-RISCV Shell Monitor v1.0\n");
    kernel_putstring("========================================\n");
    kernel_putstring("Tastati 'help' pentru lista de comenzi\n");
    kernel_putstring("\n");
    while (1) {
        kernel_putstring("> ");
        cmd_pos = 0;
        while (1) {
            int c = kernel_getchar_nonblock();
            if (c == -1) {
                for (volatile int i = 0; i < 1000; i++);
                continue;
            }
            if (c == '\r' || c == '\n') {
                kernel_putchar('\n');
                cmd_buffer[cmd_pos] = '\0';
                process_command(cmd_buffer);
                break;
            }
            else if (c == 0x7F || c == '\b') {
                if (cmd_pos > 0) {
                    cmd_pos--;
                    kernel_putstring("\b \b");  
                }
            }
            else if (c >= 32 && c < 127 && cmd_pos < CMD_BUFFER_SIZE - 1) {
                cmd_buffer[cmd_pos++] = (char)c;
                kernel_putchar((char)c);  
            }
        }
    }
}
static char shell_cmd_buffer[CMD_BUFFER_SIZE];
static int shell_cmd_pos = 0;
static int shell_initialized = 0;
void shell_init(void) {
    shell_cmd_pos = 0;
    shell_initialized = 0;
    kernel_putstring("\n");
    kernel_putstring("========================================\n");
    kernel_putstring("  OS-RISCV Shell Monitor v1.0\n");
    kernel_putstring("========================================\n");
    kernel_putstring("Tastati 'help' pentru lista de comenzi\n");
    kernel_putstring("\n");
    kernel_putstring("> ");
    shell_initialized = 1;
}
void shell_poll(void) {
    if (!shell_initialized) {
        return;
    }
    int c = kernel_getchar_nonblock();
    if (c == -1) {
        return;
    }
    if (c == '\r' || c == '\n') {
        kernel_putchar('\n');
        shell_cmd_buffer[shell_cmd_pos] = '\0';
        if (shell_cmd_pos > 0) {  
            process_command(shell_cmd_buffer);
        }
        shell_cmd_pos = 0;
        kernel_putstring("> ");
    }
    else if (c == 0x7F || c == '\b') {
        if (shell_cmd_pos > 0) {
            shell_cmd_pos--;
            kernel_putstring("\b \b");  
        }
    }
    else if (c >= 32 && c < 127 && shell_cmd_pos < CMD_BUFFER_SIZE - 1) {
        shell_cmd_buffer[shell_cmd_pos++] = (char)c;
        kernel_putchar((char)c);  
    }
}
