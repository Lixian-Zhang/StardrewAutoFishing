// compile with 'clang Hand.c -std=c99 -o hand.exe' and move the output to bin folder
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define PIPE_NAME "\\\\.\\pipe\\stardrew_auto_fishing_pipe"
#define BUFFER_SIZE 1024


LPCTSTR pipe_name = TEXT(PIPE_NAME); 

const unsigned int respond_interval_ms = 100;
const unsigned int respond_interval_clock = 100; 
TCHAR buffer[BUFFER_SIZE];

typedef enum tag_operation_state { clicking, idle } operation_state;
typedef struct tag_command {
    operation_state state;
    double duty_ratio;
} command;

HANDLE establish_pipe();
void connect_to_pipe(HANDLE pipe_handle);
int get_num_bytes_in_pipe(HANDLE pipe_handle);
bool read_from_pipe(HANDLE pipe_handle);
void release_pipe(HANDLE pipe_handle);
command parse_message();

int main() {
    HANDLE pipe_handle = establish_pipe();
    connect_to_pipe(pipe_handle);
    command com = { idle, 0.0 };

    while (true) {
        clock_t start = clock();
        int num_bytes_in_pipe = get_num_bytes_in_pipe(pipe_handle);
        if (num_bytes_in_pipe == -1) {
            break; // client disconnected, should terminate
        }
        if (num_bytes_in_pipe > 0) {
            bool success = read_from_pipe(pipe_handle);
            if (!success) break; // client disconnected, should terminate
            com = parse_message();
        }
        switch (com.state) {
            case idle:
                break;
            case clicking:
                clock_t num_clocks_mouse_down = (clock_t)lround(com.duty_ratio * respond_interval_clock);
                clock_t mouse_down_start = clock();
                // mouse down
                while (clock() - mouse_down_start < num_clocks_mouse_down); // waste some time
                // mouse up
                break;
        }
        while(clock() - start < respond_interval_clock); // waste some time
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
        } else {
            printf(TEXT("ReadFile failed, error code: %lu.\n"), GetLastError()); 
        }
        return false;
    }
    printf("Received: %s\n", buffer);
    return true;
}

int get_num_bytes_in_pipe(HANDLE pipe_handle) {
    DWORD num_bytes_available = 0;
    bool success = PeekNamedPipe(
        pipe_handle,
        NULL,
        0,
        NULL,
        &num_bytes_available,
        NULL
    );
    if (!success) {
        if (GetLastError() == ERROR_BROKEN_PIPE) {
            printf(TEXT("Client disconnected.\n"));
        } else {
            printf(TEXT("ReadFile failed, error code: %lu.\n"), GetLastError());
        }
        return -1;
    } else return num_bytes_available % INT32_MAX; 
}

command parse_message() {
    command com = { idle, 0.0 };
    if (strcmp(buffer, "idle")) {
        com.state = idle;
        com.duty_ratio = 0.0;
    } else if (strncmp(buffer, "clicking", 8) == 0) {
        com.state = clicking;
        com.duty_ratio = atof(buffer + 8);
        if (com.duty_ratio > 1) com.duty_ratio = 1;
    }
    return com;
}