#include "shell.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// BuiltinResult 执行内置指令
BuiltinResult run_builtin(Shell *sh) {
    const char *cmd = sh->argv[0];

    // cd
    if(strcmp(cmd,"cd")==0) {
        // 核验参数个数
        if(sh->argc>2){
            fprintf(stderr, "cd: too many arguments\n");
            return BUILTIN_ERROR;
        }

        const char *dir=sh->argc ==2?sh->argv[1]:getenv("HOME");
        if(!dir){
            fprintf(stderr, "cd: HOME not set\n");
            return BUILTIN_ERROR;
        }
        if(chdir(dir)!=0) {
            fprintf(stderr, "cd: %s: %s\n", dir, strerror(errno));
            return BUILTIN_ERROR;
        }
        return BUILTIN_OK;
    }

    // pwd
    if (strcmp(cmd,"pwd")==0) {
        // 核验参数个数
        if (sh->argc != 1) {
            fprintf(stderr, "pwd: too many arguments\n");
            return BUILTIN_ERROR;
        }

        char buf[1024];
        if (getcwd(buf, sizeof buf)) {
            printf("%s\n", buf);
            return BUILTIN_OK;
        }
        // 路径太长时扩容再来一次
        if (errno == ERANGE) {
            size_t sz = 4096;
            char *dyn = malloc(sz);
            while (dyn) {
                if (getcwd(dyn, sz)) {
                    printf("%s\n", dyn);
                    free(dyn);
                    return BUILTIN_OK;
                }
                if (errno != ERANGE) break;
                sz *= 2;
                char *p = realloc(dyn, sz);
                if (!p) break;
                dyn = p;
            }
            free(dyn);
        }

        fprintf(stderr, "pwd: %s\n", strerror(errno));
        return BUILTIN_ERROR;
    }

    // history
    if (strcmp(cmd, "history") == 0) {
        // 核验参数个数
        if (sh->argc != 1) {
            fprintf(stderr, "history: too many arguments\n");
            return BUILTIN_ERROR;
        }
        // 调用自制的历史命令打印机
        history_print(sh);
        return BUILTIN_OK;
    }

    // exit
    if (strcmp(cmd, "exit") == 0) {
        // 核验参数个数
        if (sh->argc > 2) {
            fprintf(stderr, "exit: too many arguments\n");
            return BUILTIN_ERROR;
        }
        if (sh->argc == 2) {
            char *end = NULL;
            long code = strtol(sh->argv[1], &end, 10);
            if (end == sh->argv[1] || *end != '\0' || code < 0 || code > 255) {
                fprintf(stderr, "exit: %s: numeric argument required\n", sh->argv[1]);
                return BUILTIN_ERROR;
            }
            sh->last_status = (int)code;
        }
        return BUILTIN_EXIT;
    }

    return BUILTIN_NOT_BUILTIN;
}