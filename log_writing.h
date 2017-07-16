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


void startlogging()
{
   openlog("Multipoll daemon", LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_LOCAL7);
}
    
void iferr(const char * msg, int msg_stat)
{
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

