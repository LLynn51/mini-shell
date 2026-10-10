# include<stdio.h>
# include<stdlib.h>
# include "shell.h"

static void prompt(){
    printf("llynn@mini-shell$");
    fflush(stdout);
}

int main(){
    Shell sh={.last_status=0};
    while(1){
        prompt();
        if(!fgets(sh.line,sizeof sh.line,stdin)){
            putchar('\n');// 处理用户直接Ctrl+D退出时的细节
            break;
        }
        
        char raw[MAX_LINE];
        snprintf(raw, sizeof raw, "%s", sh.line); // 备份原始行，以备history命令使用

        int expanded=0;
        {
            char *p = sh.line;
            while(*p==' '||*p=='\t') p++;
            if(*p=='!') {
                int r=history_expand(&sh);
                if(r<0){ // !x或!999
                    sh.last_status = 1;
                    continue;
                }
                expanded=(r==1);
            }
        }


        // 解析当前输入行
        int parsed=parse_line(&sh);
        if(!parsed)continue; // 空输入直接进入下一行
        if(parsed<0){ // 参数过多
            fprintf(stderr,"mini-shell: too many arguments\n");
            sh.last_status=1;
            continue;
        }

        if (!expanded) {
            history_add_str(&sh, raw); // 记录原始有效命令；!n展开结果不重复记
        }

        BuiltinResult b = run_builtin(&sh);
        if (b == BUILTIN_EXIT) break;
        if (b != BUILTIN_NOT_BUILTIN) {
            sh.last_status = (b == BUILTIN_OK) ? 0 : 1;
            continue;
        }

        sh.last_status=run_external(&sh);
    }
    return sh.last_status;
}