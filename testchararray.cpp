//here is wehre I got this from:
//https://www.ibm.com/developerworks/community/blogs/58e72888-6340-46ac-b488-d31aa4058e9c/entry/understanding_linux_open_system_call?lang=en

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    //file descriptor
    int fd;

    //if no no arguments, then print something
    if(2 != argc)
    {
        printf("\n Usage :  \n");
        return 1;
    }

    //error number
    errno = 0;

    //file descriptor = open
    fd = open(argv[1],O_RDWR);  
    //this is erroe catching, if its false, the file opens
    if(-1 == fd)            
    {
        printf("\n open() failed with error [%s]\n",strerror(errno));
        return 1;
    }
    else
    {
        printf("\n Open() Successful\n");
        /* open() succeeded, now one can do read operations
           on the file opened since we opened it in read-only
           mode. Also once done with processing, the file needs
           to be closed. Closing a file can be achieved using
           close() function. */

        int charArraylength = 5;
        char charArray[charArraylength];
        read(fd, charArray, sizeof(charArray));

        for (int i = 0; i < charArraylength; i++) {
            cout << charArray[i];
        }
        cout << string(charArray);
        
    }

    return 0;
}
