#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define PIPE_NAME "\\\\.\\pipe\\stardrew_auto_fishing_pipe"


LPCTSTR pipe_name = TEXT(PIPE_NAME); 

const unsigned int buffer_size = 1024;
const unsigned int respond_interval_ms = 200;


enum operation_state { clicking, idle };

HANDLE establish_pipe();
void release_pipe(HANDLE pipe_handle);

int main() {
    bool is_running = false;
    HANDLE pipe = establish_pipe();
    printf("%llu\n", (UINT64)pipe);
    release_pipe(pipe);
    return 0;
}


HANDLE establish_pipe() {

    HANDLE pipe = CreateNamedPipeA(
        pipe_name,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
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