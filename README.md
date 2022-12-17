# 🕹 Command Line Tetris 🎮
### By Ant Nosaryev and Youngmin Kwon
An implementation of Classic Tetris that runs in your console window.

### Info 📰
- Instructions
  - In the repo directory, execute `$ make run`
    - If make is not available, see makefile for compile steps
- Controls
  - W / Up Arrow Key: Rotate Piece Clockwise
  - A / Left Arrow Key: Move Piece Left
  - D / Right Arrow Key: Move Piece Right
  - S / Down Arrow Key: Soft Drop
  - Spacebar: Hard Drop
  - Q: Quit
- Endless
  - No implemented final level or ending (aside from loss).
- Highscore
  - Saves highscore locally.

### Dependencies 🛒
- ncurses
- pthread
- gcc
- make
- Permission to write file (stores highscore)

### Known Bugs 👾
- Recurring bug causes the screen to fill with random terminal colour codes and text.
  - Moving active piece over bugged locations in board causes the location to update.
  - Possible causes:
    - Concurrent calls to print functions from multiple threads.
    - Faulty handling of memory
