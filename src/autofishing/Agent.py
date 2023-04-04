import win32file
import win32api
from time import sleep
from .Hand import Hand
from .Eye import Eye

class Agent:

    def __init__(self) -> None:
        self.hand = Hand()
        self.eye = Eye()

    def __del__(self):
        pass

    def run(self):
        while True:
            game_info = self.eye.get_game_info()
            if game_info is None:
                self.hand.stop()
                continue
            fish_y, fishing_bar_y, progress = game_info
            self.hand.click(0.50 + (fish_y - fishing_bar_y) / -300 * 1)