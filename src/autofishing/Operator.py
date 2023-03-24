import pyautogui
import time
import threading
import win32gui

class Operator:

    def __init__(self) -> None:
        pass

    def detect_game_area(self) -> tuple[int, int, int, int]:
        win32gui.EnumWindows(_handler, None)

    def click(self) -> None:

        pyautogui.click

def _handler(hwnd, ctx):
    print(hwnd, ctx)
