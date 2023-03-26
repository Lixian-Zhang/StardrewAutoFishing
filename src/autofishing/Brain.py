import win32file
import win32api
from time import sleep

handle = win32file.CreateFile(
    "\\\\.\\pipe\\stardrew_auto_fishing_pipe",
    win32file.GENERIC_WRITE,
    win32file.FILE_SHARE_READ,
    None,
    win32file.OPEN_EXISTING,
    win32file.FILE_ATTRIBUTE_NORMAL,
    None
)


print(handle)

content = 'test'.encode()

win32file.WriteFile(
    handle,
    content
)
sleep(5)
win32file.WriteFile(
    handle,
    content
)

win32api.CloseHandle(handle)