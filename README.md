# UQUnscramble

NOTE: PROGRAM DOES NOT COMPILE, AS libcsse2310a1.so IS UNAVAILABLE. THIS IS PURELY FOR RECORD PURPOSES

Terminal based text game in C.

uqunscramble prints out a set of letters and asks the user to enter words that can be made from that set of letters and that have a certain minimum length. The program checks the entered word can be formed from the letters, and found in the supplied dictionary file. If so, the program accepts the word and increases the user's score, increasing by an additional 10 if the word uses all the letters. Parameters can be specified on the command line to customise the game.

./uqunscramble [--dict _dictfile_] [--lett _letters_] [--len-min _numchars_] 
