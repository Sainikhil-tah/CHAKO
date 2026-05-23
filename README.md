# CHAKO

CHAKO is a terminal-based chess engine and playable chess application written in C++.

The project was built to explore classical chess engine architecture, search algorithms, board evaluation techniques, and automated engine benchmarking. It supports both Human vs Human and Human vs Engine gameplay directly from the terminal, along with automated testing against Stockfish for performance evaluation and Elo estimation.

The engine currently uses a Negamax search framework with Alpha-Beta pruning, Quiescence search, material evaluation, and Piece-Square Tables (PSTs) to evaluate positions efficiently.

Apart from gameplay, the project also focuses on:
- engine experimentation,
- evaluation tuning,
- endgame heuristics,
- automated testing pipelines,
- and practical search optimizations.

CHAKO was developed as a systems-oriented project combining:
- algorithmic problem solving,
- chess engine architecture,
- search optimization techniques,
- game state management,
- and automated engine benchmarking.
# Terminal Chess Engine

A terminal-based chess game and chess engine written in C++.

The project supports both **Human vs Human** and **Human vs Engine** gameplay directly from the terminal, along with automated games between Stockfish and the custom engine for benchmarking and evaluation.

---

# Features

* Human vs Human mode
* Human vs Engine mode
* Undo moves
* Redo moves
* Save game support
* Load saved games
* Cancel move functionality
* Negamax based chess engine
* Alpha-Beta pruning
* Quiescence search
* Automated testing using Python scripts
* Stockfish benchmarking and evaluation
* Clean terminal interface
* Full legal chess move support,including castling,en passant,pawn promotion,check/checkmate detection and draw condition handling.
---

# Estimated Engine Strength

The engine was benchmarked using automated games against Stockfish under different search depths and time controls.

Tests were performed over approximately **500 games**.

| Search Depth | Time Limit  | Estimated Elo | Average Game Duration(tested for 250 games)|
| ------------ | ----------- | ------------- | --------------------- |
| Depth 4      | 0.5 seconds | ~1674 Elo     | ~28 seconds           |
| Depth 4      | 1 second    | ~1732 Elo     | ~33 seconds           |
| Depth 5      | 3 seconds   | ~1882 Elo     | ~65 seconds           |
> Elo values are approximate and may vary depending on hardware, openings, and testing conditions.

---

# Project Structure

```text
.
├── include/               # Header files
├── src/                   # Source files
├── automation/            # Python automation scripts
├── local_testing/         # Local testing utilities and reports
├── testing_reports/       # GitHub-based testing reports
├── saves/                 # Saved game history
└── README.md
```

---

# Build Instructions

## Linux / macOS

### Compile

```bash
make
```

### Run

```bash
./chess
```

---

## Windows (MinGW g++)

### Compile

```powershell
g++ .\src\*.cpp -I.\include -std=c++20 -g -o chess.exe
```

### Run

```powershell
./chess.exe
```

---

# Gameplay Screenshots

<img width="1034" height="853" alt="Screenshot 2026-05-20 162714" src="https://github.com/user-attachments/assets/8a0a0351-d907-4d47-94a8-64e7b685b697" />

<img width="1134" height="858" alt="Screenshot 2026-05-20 124447" src="https://github.com/user-attachments/assets/c2fc382b-fa8f-4939-b1c0-0d83a5a6d462" />

<img width="1009" height="907" alt="Screenshot 2026-05-22 182712" src="https://github.com/user-attachments/assets/5e3fc309-2f0e-4f4c-852d-d9f0150a192f" />

---

# Supported Commands

```text
move e2e4
undo
redo
save
load
cancel
```

---

# Automated Testing

The project includes Python scripts for automated matches between:

* Your engine
* Stockfish

This was used for:

* Elo estimation
* Regression testing
* Move quality analysis
* Engine benchmarking

Additional local engine testing and report generation were performed using custom testing utilities.

---

# Stockfish Setup

Before running the automation scripts, update the Stockfish executable path inside the automation Python file.

Example:

```python
sf = chess.engine.SimpleEngine.popen_uci("/usr/games/stockfish")
```

Replace it with the location of your local Stockfish executable.

Download Stockfish from:

[https://stockfishchess.org/download/](https://stockfishchess.org/download/)

---

# Engine Details

The engine currently includes:

* Negamax search
* Alpha-Beta pruning
* Quiescence search
* Basic board evaluation
* Piece-Square Tables (PST)
* Material-based evaluation
* Move generation
* Terminal-based game loop

---

# Failed / Experimental Implementations

## Time-Based Game Modes

A time-based engine mode was experimented with but was not fully integrated into the current engine architecture, as it required dynamically updating and rendering chess clocks inside the terminal interface.

---

## Endgame Mobility-Based Evaluation

An experimental mobility-based evaluation system was implemented for endgames.

The idea was to improve positions where the opponent only had a king remaining by rewarding restricting moves and increasing king mobility pressure.

However, in many positions the engine repeatedly selected the same “best safe move,” causing draw repetition even in theoretically winning positions.

Although partial heuristics were added initially, they were later removed because:

* the behavior was highly position-dependent,
* the heuristic was unreliable for all single-king endgames,
* and it sometimes reduced overall engine stability.

---

# Thought Process

A video explaining the problems and errors faced during development, along with how they were resolved:

[https://youtu.be/ZoS1G_1yc6E?si=BNMtl43rGniJVk_R](https://youtu.be/ZoS1G_1yc6E?si=BNMtl43rGniJVk_R)

---

# Gameplay

[https://youtu.be/2x2Zz14Lsas](https://youtu.be/2x2Zz14Lsas)

---

# Future Improvements
* Iterative deepening
* zobrist hashing
* Transposition tables
* Improved evaluation function 
---
---

# Web Arena Interface (Will Be Added Shortly)

An experimental browser-based arena system was also developed for automated engine testing, benchmarking, and live match visualization.

The frontend was implemented using:
- HTML
- CSS
- JavaScript

The backend was implemented using:
- Python
- Flask
- Flask-CORS
- Python subprocess management
- UCI-based engine communication

The arena system allows automated matches between:
- CHAKO
- Stockfish

Currently implemented features:
- configurable Stockfish Elo(cant change once the game starts),
- configurable Stockfish search depth (cant change once the game starts),
- configurable CHAKO search depth (cant change once the game starts),
- configurable move time controls (cant change once the game starts),
- live board visualization,
- real-time game updates,
- win/loss/draw tracking,
- estimated Elo calculation,
- engine statistics tracking,
- UCI command logging into terminal,
- 
The backend communicates with both engines using Python subprocess management and UCI protocol commands exposed through Flask API endpoints.

The frontend and backend systems are currently under further development and will be added to the repository shortly after additional improvements, testing, and stabilization.

Planned improvements:
- total game duration clock and live frontend time tracking,
- pause/resume arena during execution,
- stopping automation mid-match and changing depth,time limit,
- live move list , captured pieces visualization,
- PGN copy mid game,
- data save/load support.
