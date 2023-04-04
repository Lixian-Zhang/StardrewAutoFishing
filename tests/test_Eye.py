import time
from autofishing.Eye import Eye

def main():
    e = Eye()
    while True:
        last_time = time.time()
        print(e.get_game_info(), f"fps: {1 / (time.time() - last_time)}", end='\r')

if __name__ == '__main__':
    main()
