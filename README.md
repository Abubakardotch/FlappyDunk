# Flappy Dunk

A basketball-themed Flappy-style game made in C++ with SFML.
Bounce the ball, aim it through the moving hoops, and score as many points as you can.

## Project Info

| | |
|---|---|
| **Project Name** | Flappy Dunk |
| **Institution** | FAST-NUCES Lahore Campus |
| **Course Instructor** | Sir Zeeshan Nazar |
| **Teaching Assistant** | Hamza Naveed (GitHub: [hamzanaxeed](https://github.com/hamzanaxeed)) |

## Collaborators

| Name | Roll No |
|---|---|
| Abu Bakar | 26L-2598 |
| Ali Nadir | 26L-2589 |
| Fahad Khalid | 26L-2599 |

## How to Clone

```bash
git clone https://github.com/Abubakardotch/FlappyDunk.git
cd FlappyDunk
```

## How to Run

1. Open the cloned project folder in your IDE (for example Visual Studio or VS Code).
2. Run `CMakeLists.txt`. This sets up the project and links SFML.
3. Run `FlappyDunk.cpp`. The game window will open.

## How to Play

- **Space / Left click**: bounce the ball upward.
- Pass **down through each hoop** to score 1 point.
- Missing a hoop or hitting the floor ends the game.
- **S**: slow motion - 3 uses per game.
- **R**: restart the game.
- **Esc**: return to the main menu.

## Notes

- The game loads the font from `C:/Windows/Fonts/arial.ttf`, so it is meant for Windows.
- Your best score is saved in `best_score.txt`, which is created automatically next to the game.
