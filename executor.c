# include "shell.h"
# include <sys/types.h>
# include <errno.h>
# include <stdio.h>
# include <sys/wait.h>
# include <string.h>
# include <unistd.h>

// 辅助函数：拆包status
static int report_wait_status(int status) {
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        fprintf(stderr, "mini-shell: terminated by signal %d\n", WTERMSIG(status));
        return 128 + WTERMSIG(status);
    }
    return 1;
}

int run_external(Shell *sh){
    fflush(NULL); // 冲掉缓冲区，避免重复输出父进程内容或提示词等缓冲区内容

    pid_t pid=fork();

    if(pid<0){
        int e=errno;
        fprintf(stderr,"mini-shell: fork: %s\n",strerror(e));
        return 1;
    }

    // 子进程：执行外部程序
    if(pid==0){
        execvp(sh->argv[0],sh->argv);
        // 处理失败
        int e=errno;
        fprintf(stderr,"mini-shell: %s : %s\n",sh->argv[0],strerror(e));

    }

    // 父进程：等待并回收子进程
    int status;
    while(1){
        pid_t r=waitpid(pid,&status,0);
        if (r==pid) {
            return report_wait_status(status);
        }
        if (r<0&&errno==EINTR) {
            continue; // 被信号打断后重试
        }
        int e=errno;
        fprintf(stderr,"mini-shell: waitpid: %s\n",strerror(e));
        return 1;
    }

}

