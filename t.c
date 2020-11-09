#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <time.h>
#include <unistd.h>
#include <sys/time.h>



int main() {

    struct stat a_stat;
    struct stat b_stat;
    
    struct timespec a_times[2];
    struct timespec b_times[2];

    stat("a", &a_stat);
    stat("b", &b_stat);    
    
    // Index 0 ATIME
    a_times[0] = a_stat.st_mtim;
    b_times[0] = b_stat.st_mtim;

    // Index 1 MTIME
    a_times[1] = a_stat.st_mtim;
    b_times[1] = b_stat.st_mtim;
    
    printf("**** Current MODIFIED timestamps (atime behaves same):\n");    
    printf("stat a:           %12ld.%12ld\n", a_stat.st_mtim.tv_sec, a_stat.st_mtim.tv_nsec);
    printf("stat b:           %12ld.%12ld\n", b_stat.st_mtim.tv_sec, b_stat.st_mtim.tv_nsec);
        
        
    printf("\n**** Simulating 'touch -r a b'\n");    
    b_times[1].tv_sec = a_stat.st_mtim.tv_sec;
    b_times[1].tv_nsec = a_stat.st_mtim.tv_nsec;
    b_times[0] = b_times[1];

    printf("want set on  b:   %12ld.%12ld (value from stat a)\n",  b_times[1].tv_sec, b_times[1].tv_nsec);
    
    
    int fd = open("b",  O_WRONLY | O_NONBLOCK | O_NOCTTY);
    if (fd < 0) printf("!Error opening 'b'\n");
    
    int res =  futimens(fd, b_times);    
    if (res < 0) printf("!Futimes failed\n");
    close(fd);    
    
    stat("b", &b_stat);    
    printf("stat b:           %12ld.%12ld (from stat after update)\n",  b_stat.st_mtim.tv_sec, b_stat.st_mtim.tv_nsec); 
       
    b_times[0].tv_nsec = UTIME_NOW;
    b_times[1].tv_nsec = UTIME_NOW;
    
    printf("\n**** Test simulating normal 'touch b'\n");
    printf("want set b:      %12ld.%12ld (ns set for 'NOW')\n",  b_times[1].tv_sec, b_times[1].tv_nsec);
    
    fd = open("b",  O_WRONLY | O_NONBLOCK | O_NOCTTY);
    if (fd < 0) printf("!Error opening 'b'\n");

    futimens(fd, b_times);    
    if (res < 0) printf("!Futimes failed\n");
    close(fd);    

    stat("b", &b_stat);    
    printf("stat b:          %12ld.%12ld\n",  b_stat.st_mtim.tv_sec, b_stat.st_mtim.tv_nsec);  
    
    
}