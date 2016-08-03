/* 
 * File:   log_writing.h
 * Author: u1
 *
 * Created on November 26, 2014, 12:04 PM
 */

#ifndef LOG_WRITING_H
#define	LOG_WRITING_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <time.h>    
#include <sys/types.h>   
#include <sys/stat.h>  
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
    
#define LOG_PATH "./log_multiserver_"

    
static int logging=0;
static char fullpath[100];   
int logfile;
void startlogging(); //sdtout > logfile
void iferr(const char * msg, int msg_stat); //error > exit program
//int writelog(const char *msg);

void startlogging()
{
    /*
char logname[60];
memset(fullpath, 0,  sizeof(fullpath));
strcat(fullpath, LOG_PATH);
time_t t; 
struct tm *tmp;
t = time(NULL);
tmp = localtime(&t);
if(tmp==NULL) {
    perror("localtime");
    return;
}
if(strftime(logname, sizeof(logname), "%c.log", tmp)==0) {
    printf("strfile returned 0\n");
    return;
}
strcat(fullpath, logname);
/* char *ret=NULL;
do{
   ret = strchr(fullpath, ' ');
   if(ret!=NULL)
       ret[0]='_';   
} while(ret!=NULL);

printf("%s\n", fullpath); 
logfile = open(fullpath, O_WRONLY|O_APPEND|O_CREAT, 0666);
if(logfile==-1)
 perror("error opening/creating file");
else {
    //dup2(logfile, 1);
    logging=1;
    close(logfile);    
} */
    openlog("Multipoll daemon", LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_LOCAL7);
}
    
/*int writelog(const char * msg, int msg_stat)
{
    logfile = open(fullpath, O_WRONLY|O_APPEND|O_CREAT);
    if(logfile==-1) {
        perror("error opening file");
        return -1;
    }
    
    write(logfile, msg, strlen(msg));
    fprintf(logfile, "ERROR: %s\n", strerror(errno));
    close(logfile);
    return 0;
}*/
void iferr(const char * msg, int msg_stat)
{
    /*
if(msg_stat==STAT_INT)
{
    logfile = open(fullpath, O_WRONLY|O_APPEND);
      if(logfile==-1)
        perror("error opening file");
      else {
      write(logfile, msg, strlen(msg));
      close(logfile); 
      }
    exit(EXIT_SUCCESS); 
}
if(msg_stat==STAT_ERR)
{
    if(logging!=1)
        perror(msg);      
    else {
        logfile = open(fullpath, O_WRONLY|O_APPEND|O_CREAT);
        if(logfile==-1)
            perror("error opening file");
        else {
           // dup2(logfile, 1);
            fprintf(logfile, "ERROR: %s\n", strerror(errno));
            close(logfile);
        }
    }
    exit(EXIT_FAILURE);   
}
if(msg_stat==STAT_INF)
{
    if(logging!=1)
        printf("%s\n", msg);
    else
    {
      logfile = open(fullpath, O_WRONLY|O_APPEND);
      if(logfile==-1)
        perror("error opening file");
      else {
      write(logfile, msg, strlen(msg));
      close(logfile); 
      }
    }
} */
    if(msg_stat==LOG_CRIT)
{
        syslog(LOG_INFO, msg);
    exit(EXIT_SUCCESS); 
}
if(msg_stat==LOG_ERR)
{
    syslog(LOG_ERR, msg);
    exit(EXIT_FAILURE);   
}
if(msg_stat==LOG_INFO)
{
    syslog(LOG_INFO, msg);
}
}

#ifdef	__cplusplus
}
#endif

#endif	/* LOG_WRITING_H */

