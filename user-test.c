/**
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
// #include "bpmp.h"

#define BUFFER_LENGTH 256           ///< The buffer length (crude but fine)


#include <stdio.h>

// Usage:
//     hexDump(desc, addr, len, perLine);
//         desc:    if non-NULL, printed as a description before hex dump.
//         addr:    the address to start dumping from.
//         len:     the number of bytes to dump.
//         perLine: number of bytes on each output line.

void hexDump (
    const char * desc,
    const void * addr,
    const int len,
    int perLine
) {
    // Silently ignore silly per-line values.

    if (perLine < 4 || perLine > 64) perLine = 16;

    int i;
    unsigned char buff[perLine+1];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL) printf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of perLine means new or first line (with line offset).

        if ((i % perLine) == 0) {
            // Only print previous-line ASCII buffer for lines beyond first.

            if (i != 0) printf ("  %s\n", buff);

            // Output the offset of current line.

            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.

        printf (" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }

    // Pad out last line if not exactly perLine characters.

    while ((i % perLine) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    printf ("  %s\n", buff);
}



int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: user-test <path to device>\n");
        exit(0);
    }
    char *devicepath = argv[1];

    int ret, fd;
    char structToSend[BUFFER_LENGTH];


    printf("Starting device test code...\n");


    fd = open(devicepath, O_RDWR); // Open the device with read/write access
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return errno;
    }


    printf("Type in a short string to send to the kernel module:\n");
 
    /* fill structToSend with some real data there*/

    printf("Writing message to the device [%s].\n", structToSend);

  
    hexDump ("structToSend", &structToSend, BUFFER_LENGTH, 16);

    ret = write(fd, structToSend, BUFFER_LENGTH ); // Send the string to the LKM
    if (ret < 0)
    {
        perror("Failed to write the message to the device.");
        return errno;
    }

    // driver replaces the result structure in place

    hexDump ("receivef structToSend", &structToSend, BUFFER_LENGTH, 16);


    printf("End of the program\n");
    return 0;
}
