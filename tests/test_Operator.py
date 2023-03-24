from autofishing.Hand import Hand

op = Hand()

def test_detect_game_window():
    res = op.detect_game_window()
    assert(res is None or res > 0)
    
def test_continues_click():
    op.move_mouse_inside_game_area()
    op.unit_click(0.5)

"""
def test_mouse_long_press_inside_game_area():
    op.mouse_long_press_inside_game_area(3)
"""
