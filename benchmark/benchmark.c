#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <npheap.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int i=0,number_of_processes = 1, number_of_objects=1024, max_size_of_objects = 8192 ,j; 
    int a;
    int pid;
    int size;
    char data[8192];
    char filename[256];
    char *mapped_data;
    int devfd;
    unsigned long long msec_time;
    FILE *fp;
    struct timeval current_time;
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s number_of_objects max_size_of_objects number_of_processes\n",argv[0]);
        exit(1);
    }
    number_of_objects = atoi(argv[1]);
    max_size_of_objects = atoi(argv[2]);
    number_of_processes = atoi(argv[3]);
    devfd = open("/dev/npheap",O_RDWR);
    if(devfd < 0)
    {
        fprintf(stderr, "Device open failed");
        exit(1);
    }
    srand((int)time(NULL)+(int)getpid());
    // Writing to objects
    for(i=0;i<(number_of_processes-1) && pid != 0;i++)
    {
        pid=fork();
    }
    sprintf(filename,"npheap.%d.log",(int)getpid());
    printf("Line 47\n");
    fp = fopen(filename,"w");
    printf("Line 49\n");
for(i = 0; i < number_of_objects; i++)
    {
       //npheap_lock(devfd,i);
        do 
        {
            size = rand() % max_size_of_objects;
        }
        while(size ==0 || size <= 10);
        printf("Line 58\n");
        mapped_data = (char *)npheap_alloc(devfd,i,size);
        if(!mapped_data)
        {
            printf("Line 62\n");
            fprintf(stderr,"Failed in npheap_alloc()\n");
            exit(1);
        }
//        memset(mapped_data, 0, 4096);
        a = rand()+1;
        gettimeofday(&current_time, NULL);
        for(j = 0; j < size-10; j=strlen(mapped_data))
        {printf("Line 70\n");
            sprintf(mapped_data,"%s%d",mapped_data,a);
        }
        printf("Line 73\n");
        fprintf(fp,"S\t%d\t%ld\t%d\t%lu\t%s\n",pid,current_time.tv_sec * 1000000 + current_time.tv_usec,i,strlen(mapped_data),mapped_data);
       // npheap_unlock(devfd,i);
        printf("Line 76\n");
    }
/*    
    // try delete something
    i = rand()%256;
    npheap_lock(devfd,i);
    npheap_delete(devfd,i);
    fprintf(fp,"D\t%d\t%ld\t%d\t%lu\t%s\n",pid,current_time.tv_sec * 1000000 + current_time.tv_usec,i,strlen(mapped_data),mapped_data);
    npheap_unlock(devfd,i);*/
    close(devfd);
    printf("Line 86\n");
    if(pid != 0)
        wait(NULL);
    return 0;
}

