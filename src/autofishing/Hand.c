// compile with 'clang Hand.c -std=c99 -luser32 -o hand.exe' and move the output to bin folder
#include <windows.h>
#include <winuser.h>   
#include <windowsx.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <math.h>

#define PIPE_NAME "\\\\.\\pipe\\stardrew_auto_fishing_pipe"
#define BUFFER_SIZE 1024
#define INSIDE_GAME_REGION_OFFSET 30


LPCTSTR pipe_name = TEXT(PIPE_NAME); 

const unsigned int respond_interval_ms = 20;
const unsigned int respond_interval_clock = 20; 
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
bool starts_with(const TCHAR* s, const TCHAR* pre_fix);
void get_game_window_handle(HWND* result);
BOOL CALLBACK window_enum_helper(HWND window_handle, LPARAM result);
RECT get_game_window_region(HWND window_handle);
void _send_mouse_message(HWND game_window_handle, bool is_mouse_down);
void mouse_down(HWND game_window_handle);
void mouse_up(HWND game_window_handle);

int main() {
    HWND game_window_handle = NULL;
    get_game_window_handle(&game_window_handle);
    if (!game_window_handle) {
        printf("Can not find game window, please check whether the game is running. Exiting...\n");
        return 0;
    }
    RECT game_region = get_game_window_region(game_window_handle);
    printf("Game window detected.\n");
    printf("top: %ld bottom: %ld left: %ld right: %ld\n", game_region.top, game_region.bottom, game_region.left, game_region.right);

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
        printf("com: %d %f\r", (unsigned char)com.state, com.duty_ratio);
        switch (com.state) {
            case idle:
                break;
            case clicking:
                clock_t num_clocks_mouse_down = (clock_t)lround(com.duty_ratio * respond_interval_clock);
                clock_t mouse_down_start = clock();
                mouse_down(game_window_handle);
                while (clock() - mouse_down_start < num_clocks_mouse_down); // waste some time
                mouse_up(game_window_handle);
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
    // printf("Received: %s\n", buffer);
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
    
    if (starts_with(buffer, "idle")) {
        com.state = idle;
        com.duty_ratio = 0.0;
    } else if (starts_with(buffer, "clicking")) {
        com.state = clicking;
        com.duty_ratio = atof(buffer + 9);
        if (com.duty_ratio > 1) com.duty_ratio = 1;
        if (com.duty_ratio < 0) com.duty_ratio = 0;
    }
    return com;
}

bool starts_with(const TCHAR* s, const TCHAR* pre_fix) {
    unsigned int l = 0;
    while (*(s + l) && *(pre_fix + l)) ++l;
    if (l == 0) return false; // if any of them is empty

    return strncmp(s, pre_fix, l) == 0;
}

void get_game_window_handle(HWND* result) {
    EnumWindows(window_enum_helper, (LPARAM)result);
}

BOOL CALLBACK window_enum_helper(HWND window_handle, LPARAM result) {
    TCHAR buffer[100];
    GetWindowTextA(window_handle, buffer, 100);
    if (starts_with(buffer, TEXT("Stardew Valley"))) {
        *((HWND*)result) = window_handle;
        return false;
    }
    return true;
}

RECT get_game_window_region(HWND window_handle) {
    RECT region = { 0, 0, 0, 0 };
    GetWindowRect(window_handle, &region);
    return region;
}

void mouse_down(HWND game_window_handle) {
    _send_mouse_message(game_window_handle, true);
}

void mouse_up(HWND game_window_handle) {
    _send_mouse_message(game_window_handle, false);
}

void _send_mouse_message(HWND game_window_handle, bool is_mouse_down) {
    RECT game_region = get_game_window_region(game_window_handle);

    int x_screen = GetSystemMetrics(SM_CXSCREEN);
    int y_screen = GetSystemMetrics(SM_CYSCREEN);
    
    long click_x = lround((double)(game_region.right  - INSIDE_GAME_REGION_OFFSET) / x_screen * 65536);
    long click_y = lround((double)(game_region.bottom - INSIDE_GAME_REGION_OFFSET) / y_screen * 65536);

    MOUSEINPUT move_inside_game_region = {
        click_x,
        click_y,
        0,
        MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE,
        0,
        0
    };
    INPUT ipt_move_inside;
    ipt_move_inside.type = INPUT_MOUSE;
    ipt_move_inside.mi = move_inside_game_region;

    UINT num_success = SendInput(1, &ipt_move_inside, sizeof(ipt_move_inside));
    if (num_success != 1) {
        printf("Failed to send input.\n");
    }
    if (!SetForegroundWindow(game_window_handle)) {
        printf("Failed to activate game window.\n");
    }

    MOUSEINPUT left_down = { 0, 0, 0, MOUSEEVENTF_LEFTDOWN, 0, 0 };
    MOUSEINPUT left_up = { 0, 0, 0, MOUSEEVENTF_LEFTUP, 0, 0 };
    INPUT ipt = {
        INPUT_MOUSE,
        is_mouse_down ? left_down : left_up
    };
    num_success = SendInput(1, &ipt, sizeof(ipt));
    if (num_success != 1) {
        printf("Failed to send input.\n");
    }
}
