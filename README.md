# 🕹 Command Line Tetris 🎮
### By Ant Nosaryev and Youngmin Kwon
An implementation of Classic Tetris that runs in your console window.

### Info 📰
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

### Known Bugs 👾
- Frequent bug causes the screen to fill with random terminal colour codes and text.
  - Moving active piece over bugged locations in board causes the location to update.
