#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>

#define NO_OF_ARGS 3

int main(int argc, char *argv[])
{


    openlog("Assg2 logs", LOG_PID, LOG_USER);
    if(argc < NO_OF_ARGS){
        syslog(LOG_ERR, "Error: Insufficient number of arguments");
        closelog();
        return 1;
    }

    int fd;
    
    char* path = argv[1];
    char* strToWrite = argv[2];
    
    fd = creat(path, 0644);
    
    if(fd == -1){
        syslog(LOG_ERR, "Error: File could not be opened or created");
        closelog();
        return 1;
    }
    
    int writtenSize = 0;
    
    writtenSize = write(fd, strToWrite, strlen(strToWrite));
    
    if(writtenSize == -1){
        syslog(LOG_ERR, "Error: Writing the file was unsuccessful");
        closelog();
        return 1;
    }
    
    if(writtenSize != strlen(strToWrite)){
        syslog(LOG_ERR, "Error: Some parts could not be written to file");
        closelog();
        return 1;
    }
    
    syslog(LOG_DEBUG, "Writing %s to %s", strToWrite, path);
    
    close(fd);
    closelog();
     
    return 0;
}
