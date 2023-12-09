// Code in the main program to receive messages from the named pipe
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    const char *fifo_path = "/tmp/myfifo";
    char buffer[100];

    // Open the named pipe for reading
    int fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Read data from the named pipe
    ssize_t bytes_read = read(fifo_fd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Received message: %s\n", buffer);
    }

    // Close the named pipe
    close(fifo_fd);

    return 0;
}
