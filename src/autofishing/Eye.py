import mss
import win32gui
import cv2
import numpy
import time
import numpy as np

from utils import detect_game_window

class Eye:

    def __init__(self) -> None:
        self.game_window = detect_game_window()
        self.game_area = win32gui.GetWindowRect(self.game_window)

    @property
    def game_area(self):
        return win32gui.GetWindowRect(self.game_window)
    
    @game_area.setter
    def game_area(self, value):
        return

    def look(self):
        with mss.mss() as sct:
            img = sct.grab(self._get_mss_monitor())
        return np.array(img)
    

    def run(self):
        with mss.mss() as sct:
            while "Screen capturing":
                last_time = time.time()

                # Get raw pixels from the screen, save it to a Numpy array
                img = numpy.array(sct.grab(self._get_mss_monitor()))

                # Display the picture
                cv2.imshow("OpenCV/Numpy normal", img)

                # Display the picture in grayscale
                # cv2.imshow('OpenCV/Numpy grayscale',
                #            cv2.cvtColor(img, cv2.COLOR_BGRA2GRAY))

                print(f"fps: {1 / (time.time() - last_time)}")

                # Press "q" to quit
                if cv2.waitKey(25) & 0xFF == ord("q"):
                    cv2.destroyAllWindows()
                    break

    def _get_mss_monitor(self):
        monitor = {
                    "left": self.game_area[0], 
                    "top": self.game_area[1], 
                    "width": self.game_area[2] - self.game_area[0],
                    "height": self.game_area[3] - self.game_area[1]
                }
        return monitor

img = Eye().look()
cv2.imshow('test', img)
cv2.waitKey(0)
cv2.destroyAllWindows()
