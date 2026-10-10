#include "shell.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *ltrim(char *s) {
    while (*s&&isspace((unsigned char)*s)) s++;
    return s;
}

// history_add_str 将当前命令加入历史命令回环数组，并保留参数
void history_add_str(Shell *sh, const char *raw) {
    char tmp[MAX_LINE];
    snprintf(tmp, sizeof tmp, "%s", raw);

    size_t len=strlen(tmp);
    while (len> 0&&(tmp[len-1]=='\n'||tmp[len-1]=='\r')) {
        tmp[--len] = '\0';
    }

    char *cmd = tmp;
    while(*cmd==' '||*cmd=='\t')cmd++;
    if(*cmd=='\0') return;

    if(sh->hist_len>0) {
        const char *last= history_get(sh, sh->hist_len);
        if(last && strcmp(last, cmd) == 0) return;
    }

    int slot;
    if (sh->hist_len < HIST_MAX) {
        slot = sh->hist_len;
        sh->hist_len++;
        if (sh->hist_len == HIST_MAX) sh->hist_next = 0;
    } else {
        slot = sh->hist_next;
        sh->hist_next = (sh->hist_next + 1) % HIST_MAX;
    }

    snprintf(sh->history[slot], MAX_LINE, "%s", cmd);
}

const char *history_get(Shell *sh, int n) {
    if(n<1||n>sh->hist_len)return NULL;
    int slot=(sh->hist_next + n - 1) % HIST_MAX;
    return sh->history[slot];
}

void history_print(Shell *sh) {
    for (int i=1;i<=sh->hist_len;i++) {
        const char *cmd = history_get(sh, i);
        if (cmd) printf("%d %s\n", i, cmd);
    }
}

/*
 * 返回：
 *   1  成功把 "!n" 改写成历史命令，存回 sh->line
 *   0  本来就不是 ! 命令
 *  -1  是 ! 但格式错或 n 越界，已报错
 */
int history_expand(Shell *sh) {
    char *p = ltrim(sh->line);
    if (*p != '!') return 0;

    char *end = NULL;
    long n = strtol(p + 1, &end, 10);
    if(end==p + 1) {
        fprintf(stderr, "mini-shell: history expansion must be !n\n");
        return -1;
    }

    while(*end ==' '||*end=='\t'||*end=='\r'||*end =='\n') {
        end++;
    }

    if(*end!='\0') {
        fprintf(stderr, "mini-shell: history expansion must be !n\n");
        return -1;
    }
    if (n < 1 || n > sh->hist_len) {
        fprintf(stderr, "mini-shell: !%ld: event not found\n", n);
        return -1;
    }

    const char *cmd = history_get(sh, (int)n);
    if (!cmd) {
        fprintf(stderr, "mini-shell: !%ld: event not found\n", n);
        return -1;
    }

    snprintf(sh->line, MAX_LINE, "%s\n", cmd); // 末尾补回换行，方便再parse
    return 1;
}