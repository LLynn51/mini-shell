# ifndef SHELL_H
# define SHELL_H
# include<stddef.h>
#define MAX_LINE 1024
#define MAX_ARGS 64
#define HIST_MAX 10

typedef struct {
    char line[MAX_LINE]; // 用户原始输入内容
    char *argv[MAX_ARGS]; // 用parser.c解析过的参数，指向line中的片段
    int argc; // 参数个数
    int last_status; // 上一条命令退出码

    char history[HIST_MAX][MAX_LINE]; // 使用环形数组保存历史指令
    int hist_len; // 环形数组当前存了几条
    int hist_next; // 环形下标
} Shell; // 存放一条Shell指令

// parser.c
int parse_line(Shell *sh); // 用sh->line填 argv/argc，空命令返回 0

// history.c
void history_add_str(Shell *sh, const char *raw); // 记录历史命令（带参数）
const char *history_get(Shell *sh, int n); // n从1开始
int history_expand(Shell *sh); // 处理 "!n"，返回1表示已改写 sh->line
void history_print(Shell *sh);

// builtin.c
typedef enum {
    BUILTIN_NOT_BUILTIN = 0,
    BUILTIN_OK,
    BUILTIN_ERROR,
    BUILTIN_EXIT
} BuiltinResult;

BuiltinResult run_builtin(Shell *sh);

// executor.c
int run_external(Shell *sh); // fork/execvp/waitpid，返回状态/错误码
# endif