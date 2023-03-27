from autofishing.Hand import Hand
from time import sleep
from math import sin, pi

def test_send_message():
    h = Hand()

    assert h._send_message(f'idle')
    for i in range(100):
        assert h._send_message(f'clicking {0.5}')
        sleep(0.1)
    assert h._send_message(f'idle')

