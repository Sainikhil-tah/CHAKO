# CHAKO

# Terminal Chess Engine

A terminal-based chess game and chess engine written in C++.

The project supports both **Human vs Human** and **Human vs Engine** gameplay directly from the terminal, along with automated games between Stockfish and the custom engine for benchmarking and evaluation.

---

# Features

* ♟️ Human vs Human mode
* 🤖 Human vs Engine mode
* ↩️ Undo moves
* ↪️ Redo moves
* 💾 Save game support
* 📂 Load saved games
* ❌ Cancel move functionality
* 🧠 Negamax based chess engine
* ⚡ Alpha-Beta pruning
* 🔍 Quiescence search
* 🐍 Automated testing using Python scripts
* 📊 Stockfish benchmarking and evaluation
* 📟 Clean terminal interface

---

# Estimated Engine Strength

The engine was benchmarked using automated games against Stockfish under different search depths and time controls.

Tests were performed over approximately **500 games**.

| Search Depth | Time Limit  | Estimated Elo |
| ------------ | ----------- | ------------- |
| Depth 4      | 0.5 seconds | ~1674 Elo     |
| Depth 5      | 3 seconds   | ~1882 Elo     |

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
.\chess.exe
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
* Transposition tables
* Improved evaluation function
* Better move ordering
* Opening book support
* UCI protocol support

---
