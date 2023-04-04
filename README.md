# StardrewAutoFishing
A Stardrew Valley fish bot, under development.

At the current stage, detection of fish position and fishing bar position is finished (there is a bug when treasure is present) 
as well as the action part (i.e. click to move the fishing bar).

A primitive controller is implemented. It is able to catch some easy fish perfectly and some middle hard fishes.

You can already try it out.

Next step is to fix the treasure bug.

# Usage
Prerequisites:
Python >= 3.6 (only tested on 3.10.7 should work for all versions after 3.6)
Windows10 (only windows suuported)
Steps:
1. Download 'autoFishing.zip' at 'https://github.com/Lixian-Zhang/StardrewAutoFishing/releases/tag/v0.1.0'
2. Unzip it
3. Open a windows terminal (cmd or powershell)
4. Change to the directory where you unzip the files using `cd <your directory>`, e.g. `cd C:\Users\some_user_name\Desktop\autoFishing`
5. Verify that you are successful with step 4 using `dir`, and you should see some outputs corresponding to files unzipped.
6. Run: `pip install -e .` (be careful not to miss the period at the end)
What this command does is to install a python package from the current directory. You can run `pip uninstall autoFishing` to undo this step.
7. Run the game
8. **Change both two zoom levels to the minimum (75%) and change the display mode to windowed then resize the game window to the minimum**.
9. Run: `python tests\test_Agent.py` you should see 'Game window detected.' being printed out.
10. Catch some fish!