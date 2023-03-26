import pyautogui
from time import monotonic_ns, sleep
import threading
import win32gui
import pyautogui
from utils import detect_game_window

POINT_IN_GAME_AREA_OFFSET = 10
CONTINUES_CLICK_UNIT_DURATION_IN_SECONDS = 0.1
SCANNING_RATE = 5 # Hz

mouse_position = None

class Hand:

    def __init__(self) -> None:
        self.game_window = detect_game_window()
        self._check_game_alive()
        self.game_area = win32gui.GetWindowRect(self.game_window)
        self.state = 'idle'
        self.duty_ratio = 0

    def mouse_long_press(self, duration_in_seconds) -> None:
        if duration_in_seconds == 0:
            return
        pyautogui.mouseDown()
        sleep(duration_in_seconds)
        pyautogui.mouseUp()

    def unit_click(self, duty_ratio: float) -> None:
        assert 0 <= duty_ratio <= 1

        press_duration = duty_ratio * CONTINUES_CLICK_UNIT_DURATION_IN_SECONDS
        release_duration = (1 - duty_ratio) * CONTINUES_CLICK_UNIT_DURATION_IN_SECONDS
        self.move_mouse_inside_game_area()
        self.mouse_long_press(press_duration)
        sleep(release_duration)

    def move_mouse_inside_game_area(self) -> None:
        self._check_game_alive()
        pyautogui.moveTo(self._get_point_in_game_area())

    def click_inside_game_area(self) -> None:
        # game_area: (left, top, right, bottom) i.e. (x_min, y_min, x_max, y_max) or None
        self._check_game_alive()
        self.click_at(self._get_point_in_game_area())

    def click_at(self, x, y) -> None:
        _save_mouse_position()
        pyautogui.moveTo(x, y)
        pyautogui.click()
        _restore_mouse_postion()

    def _get_point_in_game_area(self) -> tuple[int, int]:
        return (self.game_area[-2] - POINT_IN_GAME_AREA_OFFSET, 
                self.game_area[-1] - POINT_IN_GAME_AREA_OFFSET)

    def _check_game_alive(self) -> None:
        assert self.game_window is not None
        assert win32gui.IsWindow(self.game_window)

    def work(self):
        while True:
            if not self.is_running:
                return
            if self.state == 'idle':
                sleep(1 / SCANNING_RATE)
                continue
            if self.state == 'click':
                self.unit_click(self.duty_ratio)

    def start_clicking(self):
        if not self.is_running:
            return
        self.state = 'click'

    def stop_clicking(self):
        if not self.is_running:
            return
        self.state = 'idle'

    def set_duty_ratio(self, duty_ratio):
        self.duty_ratio = duty_ratio

    def run(self):
        self.is_running = True
        t = threading.Thread(target=self.work, args=())
        t.daemon = True
        t.start()

    def stop(self):
        self.is_running = False
        self.state = 'idle'
        self.duty_ratio = 0

def _save_mouse_position() -> None:
    global mouse_position
    assert mouse_position is None
    mouse_position = pyautogui.position()

def _restore_mouse_postion() -> None:
    global mouse_position
    assert mouse_position is not None
    pyautogui.moveTo(mouse_position)
    mouse_position = None
    
def _blocking_sleep(duration_in_seconds):
    assert duration_in_seconds <= 10
    start = monotonic_ns()
    while monotonic_ns() - start <= duration_in_seconds * 1e9:
        pass

def main():
    h = Hand()
    h.run()
    h.set_duty_ratio(0.75)
    sleep(1)
    h.start_clicking()
    sleep(3)
    h.stop()

if __name__ == '__main__':
    main()