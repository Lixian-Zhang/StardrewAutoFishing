from .Hand import Hand
from .Eye import Eye
from .utils import get_fishing_bar_length

class Agent:

    def __init__(self) -> None:
        self.hand = Hand()
        self.eye = Eye()

    def __del__(self):
        pass

    def run(self):
        last_e = None
        sum_u = 0
        sum_e = 0
        kp = -0.05
        kd = -0.02
        ki = -0.002
        kiu = -0.4

        while True:
            game_info = self.eye.get_game_info()
            if game_info is None:
                last_e = None
                sum_u = 0
                sum_e = 0
                self.hand.stop()
                continue
            fish_y, fishing_bar_y, progress = game_info
            fish_y = min(fish_y, 444 - get_fishing_bar_length() // 2)
            fish_y = max(fish_y, 21 + get_fishing_bar_length() // 2)
            maintaining_duty_ratio = 0.52
            e = fish_y - fishing_bar_y
            
            duty_cycle = maintaining_duty_ratio + kp * e
            if last_e is not None:
                duty_cycle += kd * (e - last_e)
            duty_cycle += sum_u * kiu
            duty_cycle += sum_e * ki

            self.hand.click(duty_cycle)
            sum_u += duty_cycle - maintaining_duty_ratio
            sum_e += e
            last_e = e
