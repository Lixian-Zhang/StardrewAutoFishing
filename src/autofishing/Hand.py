import win32file
import win32api
import subprocess
import sys
from time import sleep
from .utils import get_repo_bin_path
from .common import hand_bin_name, pipe_name

class Hand:

    def __init__(self) -> None:
        self._start_server()
        sleep(0.1)
        self._connect_to_pipe()
        sleep(0.1)

    def __del__(self):
        sleep(0.1)
        self._disconnect_to_pipe()
        self._kill_server()


    def click(self, duty_ratio: float):
        self._send_message(f'clicking {duty_ratio:.2f}')
    
    def stop(self):
        self._send_message('idle')

    def _start_server(self):
        print(str(get_repo_bin_path() / hand_bin_name))
        self.server_process = subprocess.Popen(
            [str(get_repo_bin_path() / hand_bin_name)], 
            stdout=sys.stdout
        )

    def _kill_server(self):
        self.server_process.kill()

    def _connect_to_pipe(self):
        self.pipe_handle = win32file.CreateFile(
            pipe_name,
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