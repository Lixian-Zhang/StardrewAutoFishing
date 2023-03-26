#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define PIPE_NAME "\\\\.\\pipe\\stardrew_auto_fishing_pipe"
#define BUFFER_SIZE 1024

LPCTSTR pipe_name = TEXT(PIPE_NAME); 

const unsigned int respond_interval_ms = 200;
TCHAR buffer[BUFFER_SIZE];

enum operation_state { clicking, idle };

HANDLE establish_pipe();
void connect_to_pipe(HANDLE pipe_handle);
bool read_from_pipe(HANDLE pipe_handle);
void release_pipe(HANDLE pipe_handle);


int main() {
    HANDLE pipe_handle = establish_pipe();

    connect_to_pipe(pipe_handle);    
    while (true) {
        if (!read_from_pipe(pipe_handle)) break;
    }

    release_pipe(pipe_handle);
    return 0;
}

HANDLE establish_pipe() {
    HANDLE pipe_handle = CreateNamedPipeA(
        pipe_name,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES ,
        BUFFER_SIZE,
        BUFFER_SIZE,
        0,
        NULL
    );
    if (pipe_handle == INVALID_HANDLE_VALUE) {
        printf("Can not create pipe.\n");
        exit(0);
    }
    printf("Pipe established, handle id: %llu.\n", (uint64_t)pipe_handle);
    return pipe_handle;
}

void connect_to_pipe(HANDLE pipe_handle) {
    bool connected = false;
    do {
        printf("Waiting for connection...\n");
        connected = ConnectNamedPipe(pipe_handle, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED); 
    } while (!connected);
    printf("Client connected, listening...\n");
}

void release_pipe(HANDLE pipe_handle) {
    CloseHandle(pipe_handle);
}

bool read_from_pipe(HANDLE pipe_handle) {
    DWORD num_bytes_read;
    bool success = ReadFile(
        pipe_handle,
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
        return false;
    }
    printf("Received: %s\n", buffer);
    return true;
}