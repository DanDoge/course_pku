/*
 * tsh - A tiny shell program with job control
 *
 * Name     : 
 * NickName :
 * userID   :
 * score    :  8  / 100 16:30 24 nov
 *            56  / 100 18:05 24 nov
 *                      (block all signals invloved...
 *                       replace "printf" with "sio_xxx")
 *            88  / 100 18:42 24 nov
 *                      (remove "deletejob" in sigint,sigtstp handler)
 *            100 / 100 19:23 24 nov
 *                      (maybe because check before use a null pointer?)
 *            100 / 100 19:40 24 nov
 *                      (comments added, and modified sigint,sigtstp handler)
 *            100 / 100 21:56 27 nov
 *                      (comments added, moved function "gotjob")
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF         0   /* undefined */
#define FG            1   /* running in foreground */
#define BG            2   /* running in background */
#define ST            3   /* stopped */

/*
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Parsing states */
#define ST_NORMAL   0x0   /* next token is an argument */
#define ST_INFILE   0x1   /* next token is the input file */
#define ST_OUTFILE  0x2   /* next token is the output file */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t job_list[MAXJOBS]; /* The job list */

struct cmdline_tokens {
    int argc;               /* Number of arguments */
    char *argv[MAXARGS];    /* The arguments list */
    char *infile;           /* The input file */
    char *outfile;          /* The output file */
    enum builtins_t {       /* Indicates if argv[0] is a builtin command */
        BUILTIN_NONE,
        BUILTIN_QUIT,
        BUILTIN_JOBS,
        BUILTIN_BG,
        BUILTIN_FG} builtins;
};

/* End global variables */

/* Function prototypes */
void eval(char *cmdline);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* some function(s) used in eval */
struct job_t * getjob(char *name);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, struct cmdline_tokens *tok);
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *job_list);
int maxjid(struct job_t *job_list);
int addjob(struct job_t *job_list, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *job_list, pid_t pid);
pid_t fgpid(struct job_t *job_list);
struct job_t *getjobpid(struct job_t *job_list, pid_t pid);
struct job_t *getjobjid(struct job_t *job_list, int jid);
int pid2jid(pid_t pid);
void listjobs(struct job_t *job_list, int output_fd);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
ssize_t sio_puts(char s[]);
ssize_t sio_putl(long v);
void sio_error(char s[]);

typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine
 */
int
main(int argc, char **argv)
{
    char c;
    char cmdline[MAXLINE];    /* cmdline for fgets */
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
            break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
            break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
            break;
        default:
            usage();
        }
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
    Signal(SIGTTIN, SIG_IGN);
    Signal(SIGTTOU, SIG_IGN);

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler);

    /* Initialize the job list */
    initjobs(job_list);

    /* Execute the shell's read/eval loop */
    while (1) {

        if (emit_prompt) {
            printf("%s", prompt);
            fflush(stdout);
        }
        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");
        if (feof(stdin)) {
            /* End of file (ctrl-d) */
            printf ("\n");
            fflush(stdout);
            fflush(stderr);
            exit(0);
        }

        /* Remove the trailing newline */
        cmdline[strlen(cmdline)-1] = '\0';

        /* Evaluate the command line */
        eval(cmdline);

        fflush(stdout);
        fflush(stdout);
    }

    exit(0); /* control never reaches here */
}

/*
 * eval - Evaluate the command line that the user has just typed in
 *
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.
 */
void
eval(char *cmdline)
{
    int bg;              /* should the job run in bg or fg? */
    struct cmdline_tokens tok;

    /* Parse command line */
    bg = parseline(cmdline, &tok);

    if (bg == -1) /* parsing error */
        return;
    if (tok.argv[0] == NULL) /* ignore empty lines */
        return;

    /* I/O redirection */
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;
    if (tok.infile){
        input_fd = open(tok.infile, O_RDONLY, 0);
    }
    if (tok.outfile){
        output_fd = open(tok.outfile, O_WRONLY, 0);
    }

    switch(tok.builtins){
        case BUILTIN_QUIT: {
            /* close files before exit */
            if(tok.infile){
                close(input_fd);
            }
            if(tok.outfile){
                close(output_fd);
            }
            exit(0);
            /* autually wont be executed, (but looks beautiful */
            break;
        };
        case BUILTIN_JOBS: {
            /* list jobs */
            listjobs(job_list, output_fd);
            break;
        };
        case BUILTIN_BG: {
            struct job_t* job = getjob(tok.argv[1]);
            job->state = BG;
            /* tok.argv[1] could be a JID or a PID */
            kill(-job->pid, SIGCONT);
            printf("[%d] (%d) %s\n", job->jid, job->pid, job->cmdline);
            break;
        }
        case BUILTIN_FG: {
            struct job_t* job = getjob(tok.argv[1]);
            /* save a copy of pid,
             * notice that other components in job might change after "kill()"
             * so "job->state"(line 286) won't be the true state at that time
             * but pid stays the same, and we can find out its true state in
             * job_list
             * maybe also because job may become an invalid pointer?
             * (added 27 nov) must because it would become an invalid pointer,
             * for "job" itself is a pointer too
             */
            pid_t pid = job->pid;
            job->state = FG;
            kill(-job->pid, SIGCONT);

            /* seems better to mask all the signals invloved in tsh */
            sigset_t mask_all, mask_one, prev_one;
            sigfillset(&mask_all);
            sigemptyset(&mask_one);
            sigaddset(&mask_one, SIGCHLD);//CHLD....not CHILD
            sigdelset(&mask_all, SIGCHLD);
            sigaddset(&mask_one, SIGINT);
            sigdelset(&mask_all, SIGINT);
            sigaddset(&mask_one, SIGTSTP);
            sigdelset(&mask_all, SIGTSTP);

            /* won't be interrupted */
            sigprocmask(SIG_BLOCK, &mask_one, &prev_one);

            /* wait for SIGCHID, SIGINT, SIGTSTP using sigsusped,
             * recommended in hints
             */
            while((job = getjobpid(job_list, pid)) && job->state != ST){
                sigsuspend(&mask_all);
            }

            /* reset mask to previous one */
            sigprocmask(SIG_SETMASK, &prev_one, NULL);
            break;
        };
        case BUILTIN_NONE: {
            sigset_t mask_all, mask_one, prev_one;
            sigfillset(&mask_all);
            sigemptyset(&mask_one);
            /* make sure we will add a job before delete it */
            sigaddset(&mask_one, SIGCHLD);
            sigaddset(&mask_one, SIGINT);
            sigaddset(&mask_one, SIGTSTP);

            sigprocmask(SIG_BLOCK, &mask_one, &prev_one);

            pid_t pid;
            if((pid = fork()) == 0){
                /* child here */
                /* reset mask to previous one */
                sigprocmask(SIG_SETMASK, &prev_one, NULL);
                /* create a new group which gid = pid */
                setpgid(0, 0);
                /* redirect I/O */
                dup2(input_fd, STDIN_FILENO);
                dup2(output_fd, STDOUT_FILENO);
                /* set environ... */
                execve(tok.argv[0], tok.argv, environ);
            }

            /* parent here */

            /* addjob protected */
            sigprocmask(SIG_BLOCK, &mask_all, NULL);
            if(bg){
                addjob(job_list, pid, BG, cmdline);
            }else{
                addjob(job_list, pid, FG, cmdline);
            }
            sigprocmask(SIG_SETMASK, &prev_one, NULL);

            if(bg){
                /* don't need to wait for child */
                printf("[%d] (%d) %s\n", pid2jid(pid), pid, cmdline);
            }else{
                sigset_t mask_all, mask_one, prev_one;
                sigfillset(&mask_all);
                sigemptyset(&mask_one);
                sigaddset(&mask_one, SIGCHLD);
                sigdelset(&mask_all, SIGCHLD);
                sigaddset(&mask_one, SIGINT);
                sigdelset(&mask_all, SIGINT);
                sigaddset(&mask_one, SIGTSTP);
                sigdelset(&mask_all, SIGTSTP);

                sigprocmask(SIG_BLOCK, &mask_one, &prev_one);

                struct job_t* job;
                /* "pid" here is child's pid */
                while((job = getjobpid(job_list, pid)) != NULL
                       && job->state != ST){
                    sigsuspend(&mask_all);
                }

                sigprocmask(SIG_SETMASK, &prev_one, NULL);
            }
            break;
        }
        default:{
            /* what to do here...? */
            exit(1);
        }
    }

    /* close files if there are any */
    if(tok.infile){
        close(input_fd);
    }
    if(tok.outfile){
        close(output_fd);
    }
    return ;
}

/* some function(s) used in eval */
struct job_t* getjob(char *name){
    /* handout: JIDs should be denoted on the command line
     * by the prefix '%'.
     */
    if (name[0] == '%'){
        return getjobjid(job_list, atoi(name + 1));
    }else{
        return getjobpid(job_list, atoi(name));
    }
}

/*
 * parseline - Parse the command line and build the argv array.
 *
 * Parameters:
 *   cmdline:  The command line, in the form:
 *
 *                command [arguments...] [< infile] [> oufile] [&]
 *
 *   tok:      Pointer to a cmdline_tokens structure. The elements of this
 *             structure will be populated with the parsed tokens. Characters
 *             enclosed in single or double quotes are treated as a single
 *             argument.
 * Returns:
 *   1:        if the user has requested a BG job
 *   0:        if the user has requested a FG job
 *  -1:        if cmdline is incorrectly formatted
 *
 * Note:       The string elements of tok (e.g., argv[], infile, outfile)
 *             are statically allocated inside parseline() and will be
 *             overwritten the next time this function is invoked.
 */
int
parseline(const char *cmdline, struct cmdline_tokens *tok)
{

    static char array[MAXLINE];          /* holds local copy of command line */
    const char delims[10] = " \t\r\n";   /* argument delimiters (white-space) */
    char *buf = array;                   /* ptr that traverses command line */
    char *next;                          /* ptr to the end of the current arg */
    char *endbuf;                        /* ptr to end of cmdline string */
    int is_bg;                           /* background job? */

    int parsing_state;                   /* indicates if the next token is the
                                            input or output file */

    if (cmdline == NULL) {
        (void) fprintf(stderr, "Error: command line is NULL\n");
        return -1;
    }

    (void) strncpy(buf, cmdline, MAXLINE);
    endbuf = buf + strlen(buf);

    tok->infile = NULL;
    tok->outfile = NULL;

    /* Build the argv list */
    parsing_state = ST_NORMAL;
    tok->argc = 0;

    while (buf < endbuf) {
        /* Skip the white-spaces */
        buf += strspn (buf, delims);
        if (buf >= endbuf) break;

        /* Check for I/O redirection specifiers */
        if (*buf == '<') {
            if (tok->infile) {
                (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
                return -1;
            }
            parsing_state |= ST_INFILE;
            buf++;
            continue;
        }
        if (*buf == '>') {
            if (tok->outfile) {
                (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
                return -1;
            }
            parsing_state |= ST_OUTFILE;
            buf ++;
            continue;
        }

        if (*buf == '\'' || *buf == '\"') {
            /* Detect quoted tokens */
            buf++;
            next = strchr (buf, *(buf-1));
        } else {
            /* Find next delimiter */
            next = buf + strcspn (buf, delims);
        }

        if (next == NULL) {
            /* Returned by strchr(); this means that the closing
               quote was not found. */
            (void) fprintf (stderr, "Error: unmatched %c.\n", *(buf-1));
            return -1;
        }

        /* Terminate the token */
        *next = '\0';

        /* Record the token as either the next argument or the i/o file */
        switch (parsing_state) {
        case ST_NORMAL:
            tok->argv[tok->argc++] = buf;
            break;
        case ST_INFILE:
            tok->infile = buf;
            break;
        case ST_OUTFILE:
            tok->outfile = buf;
            break;
        default:
            (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
            return -1;
        }
        parsing_state = ST_NORMAL;

        /* Check if argv is full */
        if (tok->argc >= MAXARGS-1) break;

        buf = next + 1;
    }

    if (parsing_state != ST_NORMAL) {
        (void) fprintf(stderr,
                       "Error: must provide file name for redirection\n");
        return -1;
    }

    /* The argument list must end with a NULL pointer */
    tok->argv[tok->argc] = NULL;

    if (tok->argc == 0)  /* ignore blank line */
        return 1;

    if (!strcmp(tok->argv[0], "quit")) {                 /* quit command */
        tok->builtins = BUILTIN_QUIT;
    } else if (!strcmp(tok->argv[0], "jobs")) {          /* jobs command */
        tok->builtins = BUILTIN_JOBS;
    } else if (!strcmp(tok->argv[0], "bg")) {            /* bg command */
        tok->builtins = BUILTIN_BG;
    } else if (!strcmp(tok->argv[0], "fg")) {            /* fg command */
        tok->builtins = BUILTIN_FG;
    } else {
        tok->builtins = BUILTIN_NONE;
    }

    /* Should the job run in the background? */
    if ((is_bg = (*tok->argv[tok->argc-1] == '&')) != 0)
        tok->argv[--tok->argc] = NULL;//!!!

    return is_bg;
}


/*****************
 * Signal handlers
 *****************/

/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP, SIGTSTP, SIGTTIN or SIGTTOU signal. The
 *     handler reaps all available zombie children, but doesn't wait
 *     for any other currently running children to terminate.
 */
void
sigchld_handler(int sig)
{
    /* save olderrno */
    int errorno = errno;
    int status;
    pid_t pid;

    /* recommended in tshlab-handout hint part */
    while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0){
        sigset_t mask_all, prev_one;
        sigfillset(&mask_all);
        /* mask all signals while trying to visit a global variable */
        sigprocmask(SIG_BLOCK, &mask_all, &prev_one);
        struct job_t* job = getjobpid(job_list, pid);
        sigprocmask(SIG_SETMASK, &prev_one, NULL);

        if(WIFSTOPPED(status)){
            job->state = ST;
            /* ummm...printf's not safe... */
            //printf("Job [%d] (%d) stopped by signal %d\n"
            //       , job->jid, pid, WSTOPSIG(status));
            /* no function called "sio_printf" */
            //sio_printf("Job [%d] (%d) stopped by signal %d\n"
            //           , job->jid, pid, WSTOPSIG(status));
            /* sio_puts, sio_putl, sio_error available */
            sio_puts("Job [");
            sio_putl((long)job->jid);
            sio_puts("] (");
            sio_putl((long)pid);
            sio_puts(") stopped by signal ");
            sio_putl((long)WSTOPSIG(status));
            sio_puts("\n");
            /* no need to deletejob here */
            /* STOPPED means a child is suspended, not terminated */
            continue;
        }
        if(WIFSIGNALED(status)){
            sio_puts("Job [");
            sio_putl((long)job->jid);
            sio_puts("] (");
            sio_putl((long)pid);
            sio_puts(") terminated by signal ");
            sio_putl((long)WTERMSIG(status));
            sio_puts("\n");
            /* mask all signals while trying to visit a global variable */
            sigprocmask(SIG_BLOCK, &mask_all, &prev_one);
            deletejob(job_list, pid);
            sigprocmask(SIG_SETMASK, &prev_one, NULL);
            /* again dont delete the child here */
            continue;
        }
        if(WIFEXITED(status)){
            /* mask all signals while trying to visit a global variable */
            sigprocmask(SIG_BLOCK, &mask_all, &prev_one);
            deletejob(job_list, pid);
            sigprocmask(SIG_SETMASK, &prev_one, NULL);
            continue;
        }
    }
    /* and reset it */
    errno = errorno;
    return;
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
void
sigint_handler(int sig)
{
    int old_errno = errno;
    sigset_t mask_all, prev_one;
    sigfillset(&mask_all);
    /* mask all signals while trying to visit a global variable */
    sigprocmask(SIG_BLOCK, &mask_all, &prev_one);
    pid_t pid = fgpid(job_list);
    sigprocmask(SIG_SETMASK, &prev_one, NULL);
    if(pid){
        kill(-pid, SIGINT);
        /* just send SIGINT to fg job...don't delete it... */
        //deletejob(job_list, pid);
    }
    errno = old_errno;
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void
sigtstp_handler(int sig)
{
    int old_errno = errno;
    sigset_t mask_all, prev_one;
    sigfillset(&mask_all);
    /* mask all signals while trying to visit a global variable */
    sigprocmask(SIG_BLOCK, &mask_all, &prev_one);
    pid_t pid = fgpid(job_list);
    sigprocmask(SIG_SETMASK, &prev_one, NULL);
    if(pid){
        kill(-pid, SIGTSTP);
        /* don't delete it now... */
        //deletejob(job_list, pid);
    }


    errno = old_errno;
    return;
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void
sigquit_handler(int sig)
{
    sio_error("Terminating after receipt of SIGQUIT signal\n");
}



/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void
clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void
initjobs(struct job_t *job_list) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
        clearjob(&job_list[i]);
}

/* maxjid - Returns largest allocated job ID */
int
maxjid(struct job_t *job_list)
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].jid > max)
            max = job_list[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int
addjob(struct job_t *job_list, pid_t pid, int state, char *cmdline)
{
    int i;

    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == 0) {
            job_list[i].pid = pid;
            job_list[i].state = state;
            job_list[i].jid = nextjid++;
            if (nextjid > MAXJOBS)
                nextjid = 1;
            strcpy(job_list[i].cmdline, cmdline);
            if(verbose){
                printf("Added job [%d] %d %s\n",
                       job_list[i].jid,
                       job_list[i].pid,
                       job_list[i].cmdline);
            }
            return 1;
        }
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int
deletejob(struct job_t *job_list, pid_t pid)
{
    int i;

    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == pid) {
            clearjob(&job_list[i]);
            nextjid = maxjid(job_list)+1;
            return 1;
        }
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t
fgpid(struct job_t *job_list) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].state == FG)
            return job_list[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t
*getjobpid(struct job_t *job_list, pid_t pid) {
    int i;

    if (pid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].pid == pid)
            return &job_list[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *job_list, int jid)
{
    int i;

    if (jid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].jid == jid)
            return &job_list[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int
pid2jid(pid_t pid)
{
    int i;

    if (pid < 1)
        return 0;
    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].pid == pid) {
            return job_list[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void
listjobs(struct job_t *job_list, int output_fd)
{
    int i;
    char buf[MAXLINE];

    for (i = 0; i < MAXJOBS; i++) {
        memset(buf, '\0', MAXLINE);
        if (job_list[i].pid != 0) {
            sprintf(buf, "[%d] (%d) ", job_list[i].jid, job_list[i].pid);
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
            memset(buf, '\0', MAXLINE);
            switch (job_list[i].state) {
            case BG:
                sprintf(buf, "Running    ");
                break;
            case FG:
                sprintf(buf, "Foreground ");
                break;
            case ST:
                sprintf(buf, "Stopped    ");
                break;
            default:
                sprintf(buf, "listjobs: Internal error: job[%d].state=%d ",
                        i, job_list[i].state);
            }
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
            memset(buf, '\0', MAXLINE);
            sprintf(buf, "%s\n", job_list[i].cmdline);
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
        }
    }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void
usage(void)
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void
unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void
app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/* Private sio_functions */
/* sio_reverse - Reverse a string (from K&R) */
static void sio_reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* sio_ltoa - Convert long to base b string (from K&R) */
static void sio_ltoa(long v, char s[], int b)
{
    int c, i = 0;

    do {
        s[i++] = ((c = (v % b)) < 10)  ?  c + '0' : c - 10 + 'a';
    } while ((v /= b) > 0);
    s[i] = '\0';
    sio_reverse(s);
}

/* sio_strlen - Return length of string (from K&R) */
static size_t sio_strlen(char s[])
{
    int i = 0;

    while (s[i] != '\0')
        ++i;
    return i;
}

/* Public Sio functions */
ssize_t sio_puts(char s[]) /* Put string */
{
    return write(STDOUT_FILENO, s, sio_strlen(s));
}

ssize_t sio_putl(long v) /* Put long */
{
    char s[128];

    sio_ltoa(v, s, 10); /* Based on K&R itoa() */
    return sio_puts(s);
}

void sio_error(char s[]) /* Put error message and exit */
{
    sio_puts(s);
    _exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t
*Signal(int signum, handler_t *handler)
{
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}
