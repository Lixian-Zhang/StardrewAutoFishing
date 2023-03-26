# compile with 'clang Hand.c -std=c99 -o hand.exe' and move the output to the right place
hand_bin_name = 'hand.exe' 
# make sure this one is identical to PIPE_NAME in Hand.c
pipe_name = '\\\\.\\pipe\\stardrew_auto_fishing_pipe' 

fish_pattern_name = 'fish.npy'
fishing_UI_pattern_name = 'fishing_UI.npy'