import win32file
import win32api
import subprocess
import sys
from time import sleep

class Hand:

    def __init__(self) -> None:
        self._start_server()
        sleep(0.1)
        self._connect_to_pipe()

    def __del__(self):
        self._disconnect_to_pipe()
        self._kill_server()

    def _start_server(self):
        self.server_process = subprocess.Popen(
            ['C:\\Users\\Max-win10\\source\\repos\\autoFishing\\bin\\hand.exe'], 
            stdout=sys.stdout
        )

    def _kill_server(self):
        self.server_process.kill()

    def _connect_to_pipe(self):
        self.pipe_handle = win32file.CreateFile(
            "\\\\.\\pipe\\stardrew_auto_fishing_pipe",
            win32file.GENERIC_WRITE,
            win32file.FILE_SHARE_READ,
            None,
            win32file.OPEN_EXISTING,
            win32file.FILE_ATTRIBUTE_NORMAL,
            None
        )

        if self.pipe_handle is None:
            raise RuntimeError('Failed to connect to pipe, please check whether pipe is created.')

    def _disconnect_to_pipe(self):
        win32api.CloseHandle(self.pipe_handle)

    def _send_message(self, message: str) -> bool:
        error_code, num_bytes_written = win32file.WriteFile(self.pipe_handle, message.encode())
        if error_code != 0 or num_bytes_written == 0:
            return False
        else:
            return True