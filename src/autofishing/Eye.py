import mss
import win32gui
import cv2
import numpy
import time
import numpy as np

from .utils import detect_game_window, get_fishing_bar_length, get_repo_res_path
from .common import fish_pattern_name, fishing_UI_pattern_name

TEMPLATE_MATCHING_CONFIDENCE_THRESHOLD = 0.5

# use minimal zoom level (both UI and game)

class Eye:

    def __init__(self) -> None:
        self.game_window = detect_game_window()
        if self.game_window is None:
            raise RuntimeError('Game window not found, please check that the game is running.')
        self.game_area = win32gui.GetWindowRect(self.game_window)
        self.fish_pattern = np.load(str(get_repo_res_path() / fish_pattern_name))
        self.fishing_UI_pattern = np.load(str(get_repo_res_path() / fishing_UI_pattern_name))
        self.last_UI_loc = None

    @property
    def game_area(self):
        return win32gui.GetWindowRect(self.game_window)
    
    @game_area.setter
    def game_area(self, _):
        return

    def look(self):
        with mss.mss() as sct:
            img = sct.grab(self._get_mss_monitor())
        return np.array(img)

    def get_game_info(self):
        with mss.mss() as sct:
            monitor = self._get_mss_monitor()
            img = numpy.array(sct.grab(monitor))
            if img.shape[0] != monitor['height'] or img.shape[1] != monitor['width']:
                raise RuntimeError('Failed to capture game.')
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

            if self.is_fishing_UI_valid(self.last_UI_loc, img):
                UI_loc = self.last_UI_loc
            else:
                UI_loc = self.detect_fishing_UI(gray)
                if not self.is_fishing_UI_valid(UI_loc, img):
                    self.last_UI_loc = None
                    return None
                self.last_UI_loc = UI_loc

            gray = gray[UI_loc[1] : UI_loc[1] + self.fishing_UI_pattern.shape[0], 
                        UI_loc[0] : UI_loc[0] + self.fishing_UI_pattern.shape[1]]
            fish_loc = self.detect_fish(gray)
            if fish_loc is None:
                return None
            masked_img = img[UI_loc[1] : UI_loc[1] + self.fishing_UI_pattern.shape[0], 
                                    UI_loc[0] : UI_loc[0] + self.fishing_UI_pattern.shape[1]]
            top, bottom = self.detect_fishing_bar(masked_img, fish_loc)
            progress = self.detect_progress(masked_img)
        
        return fish_loc[1] + 20 , top + (bottom - top) // 2, progress

    def _get_mss_monitor(self):
        area = self.game_area
        monitor = {
                    "left": area[0], 
                    "top": area[1], 
                    "width": area[2] - area[0],
                    "height": area[3] - area[1]
                }
        return monitor

    def detect_fish(self, gray):
        confidence, loc = match_template(gray, self.fish_pattern)
        if confidence > TEMPLATE_MATCHING_CONFIDENCE_THRESHOLD:
            return loc
        else:
            return None
        
    def detect_fishing_UI(self, gray):
        if np.any(gray.shape < self.fishing_UI_pattern.shape):
            return None
        confidence, loc = match_template(gray, self.fishing_UI_pattern)
        if confidence > TEMPLATE_MATCHING_CONFIDENCE_THRESHOLD:
            return loc
        else:
            return None
        
    def detect_progress(self, masked_img):
        line = masked_img[18:451, 114, 0]
        return 1 - np.count_nonzero(line) / len(line)
    
    def is_fishing_UI_valid(self, UI_loc, img):
        if UI_loc is None:
            return False
        masked_img = img[UI_loc[1] : UI_loc[1] + self.fishing_UI_pattern.shape[0], 
                                            UI_loc[0] : UI_loc[0] + self.fishing_UI_pattern.shape[1]]
        return self.detect_progress(masked_img) != 0
    
    def detect_fishing_bar(self, masked_img, fish_loc) -> tuple[int, int]:
        # returns an index in fishing UI frame
        t = np.all(masked_img[22 : 445, 66 : 87, 0] < 200, axis=-1)

        bottom_part = masked_img[438 : 445, 66 : 87, 1]
        t[-bottom_part.shape[0]:] = np.all(bottom_part > 120, axis=-1)

        top = np.argwhere(t)
        bottom = np.argwhere(np.flip(t))

        if len(top) == 0 or len(bottom) == 0:
            return 0, 0
        
        top = top[0].item()
        bottom = len(t) - bottom[0].item()

        fish_y = fish_loc[1]
        if abs(fish_y - bottom) < abs(fish_y - top):
            bottom = top + get_fishing_bar_length()
        else:
            top = bottom - (get_fishing_bar_length())

        return top, bottom

def match_template(img, template) -> tuple[int, int]:
    mask = np.where(template == 255, 0, 1).astype(np.uint8)
    res = cv2.matchTemplate(img, template, cv2.TM_CCOEFF_NORMED, mask=mask)
    _, max_val, _, max_loc = cv2.minMaxLoc(res)
    return max_val, np.array(max_loc)# np.array(max_loc, dtype=np.int32) + np.flip(np.array(template.shape, dtype=np.int32) // 2)

