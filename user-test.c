/**
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256           ///< The buffer length (crude but fine)

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: test <path to device>\n");
        exit(0);
    }
    char *devicepath = argv[1];

    int ret, fd;
    char stringToSend[BUFFER_LENGTH];


    printf("Starting device test code...\n");


    fd = open(devicepath, O_RDWR); // Open the device with read/write access
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return errno;
    }


    printf("Type in a short string to send to the kernel module:\n");
    scanf("%[^\n]%*c", stringToSend); // Read in a string (with spaces)
    printf("Writing message to the device [%s].\n", stringToSend);

    ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
    if (ret < 0)
    {
        perror("Failed to write the message to the device.");
        return errno;
    }


    // driver replaces the result structure inplace

    printf("The received message is: [%s]\n", stringToSend);
    printf("End of the program\n");
    return 0;
}
