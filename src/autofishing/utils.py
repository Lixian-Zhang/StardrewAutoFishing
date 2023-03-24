import win32gui


def detect_game_window() -> int:
    window_hwnds = []
    win32gui.EnumWindows(_window_enum_handler, window_hwnds)
    for window_hwnd in window_hwnds:
        title = win32gui.GetWindowText(window_hwnd)
        if title.startswith('Stardew Valley'):
            return window_hwnd

def _window_enum_handler(hwnd, results: list) -> None:
    results.append(hwnd)
