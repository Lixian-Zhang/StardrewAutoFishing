#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define PIPE_NAME "\\\\.\\pipe\\stardrew_auto_fishing_pipe"


LPCTSTR pipe_name = TEXT(PIPE_NAME); 

const unsigned int buffer_size = 1024;
const unsigned int respond_interval_ms = 200;
TCHAR buffer[buffer_size];

enum operation_state { clicking, idle };

HANDLE establish_pipe();
void release_pipe(HANDLE pipe_handle);

int main() {
    bool is_running = false;
    HANDLE pipe = establish_pipe();
    printf("Pipe established, handle id: %llu.\n", (uint64_t)pipe);
    DWORD num_bytes_read;
    bool connected = ConnectNamedPipe(pipe, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED); 
    
    while (!connected) {
        printf("No connection request, retrying...\n");
    }

    printf("Client connected, listening...\n");
    while (true) {
        bool success = ReadFile(
            pipe,
            buffer,
            sizeof(buffer),
            &num_bytes_read,
            NULL
        );
        if (!success || num_bytes_read == 0) {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                printf(TEXT("Client disconnected.\n")); 
            }
            else {
                printf(TEXT("ReadFile failed, error code: %lu.\n"), GetLastError()); 
            }
          break;
        }
        printf("Received: %s\n", buffer);
    }

    release_pipe(pipe);
    return 0;
}


HANDLE establish_pipe() {

    HANDLE pipe = CreateNamedPipeA(
        pipe_name,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES ,
        buffer_size,
        buffer_size,
        0,
        NULL
    );
    if (pipe == INVALID_HANDLE_VALUE) {
        printf("Can not create pipe.\n");
        exit(0);
    }
    return pipe;
}

void release_pipe(HANDLE pipe_handle) {
    CloseHandle(pipe_handle);
}