import win32gui
from pathlib import Path

def detect_game_window() -> int:
    window_hwnds = []
    win32gui.EnumWindows(_window_enum_handler, window_hwnds)
    for window_hwnd in window_hwnds:
        title = win32gui.GetWindowText(window_hwnd)
        if title.startswith('Stardew Valley'):
            return window_hwnd

def _window_enum_handler(hwnd, results: list) -> None:
    results.append(hwnd)

def get_fishing_level():
    return 10

def get_fishing_bar_length():
    return get_fishing_level() * 6 + 72

def get_repo_root_path():
    return Path(__file__).parent.parent.parent

def get_repo_bin_path():
    return get_repo_root_path() / 'bin'

def get_repo_res_path():
    return get_repo_root_path() / 'res'