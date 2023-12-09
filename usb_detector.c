// Code for creating a named pipe (FIFO) in the USB detection program
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main() {
    const char *fifo_path = "/tmp/myfifo";

    // Create a named pipe
    if (mkfifo(fifo_path, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    // Open the named pipe for writing
    int fifo_fd = open(fifo_path, O_WRONLY);
    if (fifo_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
	printf("Starting writing...\n");
    // Write data to the named pipe
    const char *message = "new_usb!!";
    write(fifo_fd, message, strlen(message));

    // Close the named pipe
    close(fifo_fd);
    unlink(fifo_path);  // Remove the named pipe

    return 0;
}
