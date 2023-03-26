import win32file
import win32api
from time import sleep
from .Hand import Hand
from .Eye import Eye

class Agent:

    def __init__(self) -> None:
        self.hand = Hand()
        self.Eye = Eye()

    def __del__(self):
        pass