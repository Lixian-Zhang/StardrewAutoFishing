from autofishing.Hand import Hand
from time import sleep

def test_send_message():
    h = Hand()
    for i in range(10):
        h._send_message(str(i))
        sleep(0.1)