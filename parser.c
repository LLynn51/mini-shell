# include "shell.h"
# include <string.h>

int parse_line(Shell *sh){
    sh->argc=0;
    sh->argv[0]=NULL;

    // 切分字符串
    char *save = NULL;
    char *tok = strtok_r(sh->line, " \t\r\n", &save);
    // 处理错误（参数过多）
    // 虽然我肯定不会测试这个但还是写一下吧（
    while(tok!=NULL) {
        if (sh->argc>=MAX_ARGS-1) {
            // 太多参数：不静默截断，明确报错
            sh->argc=0;
            sh->argv[0]=NULL;
            return -1;
        }
        sh->argv[sh->argc++] = tok;
        tok=strtok_r(NULL," \t\r\n",&save);
    }
    // 末尾挂结束符
    sh->argv[sh->argc]=NULL;
    
    return sh->argc>0?1:0;
}