# Generated using GPT to mimic the local test 
import chess
import chess.engine
import math
import time
import sys
import os
from datetime import datetime

# =========================================================
# LOGGING
# =========================================================

LOG_FILE = "STOCKFISH_TESTING_REPORT"
def log(msg):
    print(msg)
    # append the file 
    with open(LOG_FILE, "a", encoding="utf-8") as f:
        f.write(str(msg) + "\n")


# added env for custom queing 
STOCKFISH_ELO=int(os.getenv("STOCKFISH_ELO",1800))
STOCKFISH_DEPTH=int(os.getenv("STOCKFISH_DEPTH",8))
NUM_GAMES=int(os.getenv("NUM_GAMES",100))
GAME_DEPTH=int(os.getenv("GAME_DEPTH",4))
TIME_LIMIT=float(os.getenv("TIME_LIMIT",2.0))
MAX_PLIES=1000
with open(LOG_FILE, "a", encoding="utf-8") as f:
    f.write("\n")
    f.write("========================================\n")
    f.write(f"NEW TEST RUN : {datetime.now()}\n")
    f.write("========================================\n")
    f.write(f"NUM_GAMES       : {NUM_GAMES}\n")
    f.write(f"GAME_DEPTH      : {GAME_DEPTH}\n")
    f.write(f"TIME_LIMIT      : {TIME_LIMIT}\n")
    f.write(f"STOCKFISH_ELO   : {STOCKFISH_ELO}\n")
    f.write(f"STOCKFISH_DEPTH : {STOCKFISH_DEPTH}\n")
    f.write("\n")
# =========================================================
# MATERIAL COUNT
# =========================================================

PIECE_VALUES = {
    chess.PAWN: 1,
    chess.KNIGHT: 3,
    chess.BISHOP: 3,
    chess.ROOK: 5,
    chess.QUEEN: 9,
}


def material_difference(board):
    white = sum(
        len(board.pieces(pt, chess.WHITE)) * v for pt, v in PIECE_VALUES.items()
    )

    black = sum(
        len(board.pieces(pt, chess.BLACK)) * v for pt, v in PIECE_VALUES.items()
    )

    return white - black


# =========================================================
# INIT LOG FILE
# =========================================================
# append 
with open(LOG_FILE, "a", encoding="utf-8") as f:
    f.write("========== STOCKFISH TEST REPORT ==========\n")


# =========================================================
# LOAD ENGINES
# =========================================================

try:
    sf = chess.engine.SimpleEngine.popen_uci("/usr/games/stockfish")

except Exception as e:
    log(f"[FAIL] Could not start Stockfish: {e}")
    sys.exit(1)

try:
    my = chess.engine.SimpleEngine.popen_uci(["./chess", "uci"])

except Exception as e:
    log(f"[FAIL] Could not start your engine: {e}")

    try:
        sf.quit()
    except:
        pass

    sys.exit(1)


# =========================================================
# CONFIGURE STOCKFISH
# =========================================================

sf.configure({"UCI_LimitStrength": True, "UCI_Elo": STOCKFISH_ELO})


# =========================================================
# STATS
# =========================================================

wins = 0
losses = 0
draws = 0

my_total_time = 0.0
my_move_count = 0

sf_total_time = 0.0
sf_move_count = 0

all_total_time = 0.0
all_move_count = 0

all_nodes_search = 0


# =========================================================
# MAIN GAME LOOP
# =========================================================

for game_num in range(NUM_GAMES):

    board = chess.Board()

    my_is_white = game_num % 2 == 0

    total_moves = 0

    my_game_time = 0.0
    my_game_moves = 0
    game_nodes = 0

    log(f"\n================ GAME {game_num + 1} ================")

    while not board.is_game_over() and total_moves < MAX_PLIES:

        move_no = board.fullmove_number

        side_to_move = "WHITE" if board.turn == chess.WHITE else "BLACK"

        # =================================================
        # MY ENGINE TURN
        # =================================================

        my_turn = (board.turn == chess.WHITE and my_is_white) or (
            board.turn == chess.BLACK and not my_is_white
        )

        if my_turn:

            engine_name = "MY_ENGINE"

            my_game_moves += 1

            start = time.perf_counter()

            try:
                result = my.play(
                    board,
                    chess.engine.Limit(depth=GAME_DEPTH, time=TIME_LIMIT),
                    info=chess.engine.INFO_ALL,
                )

            except Exception as e:

                log(f"[FAIL] Engine crashed while searching: {e}")

                try:
                    sf.quit()
                    my.quit()
                except:
                    pass

                sys.exit(1)

            elapsed = time.perf_counter() - start

            my_total_time += elapsed
            my_game_time += elapsed
            my_move_count += 1

            # eval
            info = sf.analyse(
                board, chess.engine.Limit(depth=14), info=chess.engine.INFO_ALL
            )

            score = (
                info["score"].white()
                if board.turn == chess.WHITE
                else info["score"].black()
            )

            nodes = result.info.get("nodes", 0)

            all_nodes_search += nodes
            game_nodes += nodes

        # =================================================
        # STOCKFISH TURN
        # =================================================

        else:

            engine_name = "STOCKFISH"

            start = time.perf_counter()

            try:
                result = sf.play(
                    board,
                    chess.engine.Limit(depth=STOCKFISH_DEPTH),
                    info=chess.engine.INFO_ALL,
                )

            except Exception as e:

                log(f"[FAIL] Stockfish crashed: {e}")

                try:
                    sf.quit()
                    my.quit()
                except:
                    pass

                sys.exit(1)

            elapsed = time.perf_counter() - start

            sf_total_time += elapsed
            sf_move_count += 1

            info = sf.analyse(
                board, chess.engine.Limit(depth=14), info=chess.engine.INFO_ALL
            )

            score = (
                info["score"].white()
                if board.turn == chess.WHITE
                else info["score"].black()
            )

            nodes = result.info.get("nodes", 0)

        # =================================================
        # SCORE FORMAT
        # =================================================

        if score.is_mate():
            cp_str = f"M{score.mate()}"

        else:
            cp_str = f"{score.score()/100:+.2f}"

        all_total_time += elapsed
        all_move_count += 1

        # =================================================
        # SAFETY CHECKS
        # =================================================

        if result.move is None:

            log("[FAIL] Engine returned null move")
            log(board.fen())

            try:
                sf.quit()
                my.quit()
            except:
                pass

            sys.exit(1)

        if result.move not in board.legal_moves:

            log("[FAIL] Illegal move detected")
            log(f"Move: {result.move}")
            log(f"FEN : {board.fen()}")

            try:
                sf.quit()
                my.quit()
            except:
                pass

            sys.exit(1)

        # =================================================
        # LOG MOVE
        # =================================================

        log(
            f"Move {total_moves + 1:>3} | "
            f"FullMove={move_no:>3} | "
            f"Side={side_to_move:<5} | "
            f"Engine={engine_name:<9} | "
            f"Move={str(result.move):<5} | "
            f"Time={elapsed:.4f}s | "
            f"Cp_score={cp_str} | "
            f"Nodes={nodes}"
        )

        board.push(result.move)

        total_moves += 1

    # =====================================================
    # MAX PLY FAIL
    # =====================================================

    if total_moves >= MAX_PLIES:

        log("[FAIL] Max ply limit exceeded")

        try:
            sf.quit()
            my.quit()
        except:
            pass

        sys.exit(1)

    # =====================================================
    # GAME RESULT
    # =====================================================

    outcome_obj = board.outcome()
    outcome = board.result()

    termination = outcome_obj.termination.name

    winner_str = (
        "White"
        if outcome_obj.winner == chess.WHITE
        else "Black"
        if outcome_obj.winner == chess.BLACK
        else "Draw"
    )

    my_won = (outcome == "1-0" and my_is_white) or (
        outcome == "0-1" and not my_is_white
    )

    my_lost = (outcome == "0-1" and my_is_white) or (
        outcome == "1-0" and not my_is_white
    )

    diff = material_difference(board)

    color = "WHITE" if my_is_white else "BLACK"

    if my_won:
        wins += 1
        label = "WON"

    elif my_lost:
        losses += 1
        label = "LOST"

    else:
        draws += 1
        label = "DRAW"

    avg_nodes = game_nodes / my_game_moves if my_game_moves > 0 else 0

    log(board)

    log(
        f"Game {game_num + 1:>3} | "
        f"{label:<4} | "
        f"Engine={color:<5} | "
        f"Termination={termination:<22} | "
        f"Winner={winner_str:<5} | "
        f"MatDiff={diff:>+3} | "
        f"Moves={total_moves}"
    )

    log(f"Average Nodes searched : {avg_nodes:.2f}")


# =========================================================
# FINAL RESULTS
# =========================================================

score = (wins + 0.5 * draws) / NUM_GAMES

if score == 1.0:
    elo_diff = 1000

elif score == 0.0:
    elo_diff = -1000

else:
    elo_diff = -400 * math.log10((1 / score) - 1)

estimated_elo = STOCKFISH_ELO + elo_diff

my_avg_time = my_total_time / my_move_count if my_move_count > 0 else 0.0

sf_avg_time = sf_total_time / sf_move_count if sf_move_count > 0 else 0.0

all_avg_time = all_total_time / all_move_count if all_move_count > 0 else 0.0


# =========================================================
# FINAL STATS
# =========================================================

log("\n========================")
log("FINAL RESULTS")
log("========================")

log(f"Wins            : {wins}")
log(f"Losses          : {losses}")
log(f"Draws           : {draws}")
log(f"Score           : {score:.3f}")

log(f"MY_DEPTH        : {GAME_DEPTH}")

log(f"Estimated Elo ≈ {estimated_elo:.0f}")

log(f"Stockfish Elo   : {STOCKFISH_ELO}")
log(f"Stockfish Depth : {STOCKFISH_DEPTH}")

log("\n========================")
log("TIME STATS")
log("========================")

log(f"My engine moves      : {my_move_count}")
log(f"My engine total time : {my_total_time:.4f}s")
log(f"My engine avg time   : {my_avg_time:.4f}s/move")

log(f"\nStockfish moves      : {sf_move_count}")
log(f"Stockfish total time : {sf_total_time:.4f}s")
log(f"Stockfish avg time   : {sf_avg_time:.4f}s/move")

log(f"\nAll moves            : {all_move_count}")
log(f"All total time       : {all_total_time:.4f}s")
log(f"All avg time         : {all_avg_time:.4f}s/move")

log("\n[PASS] All tests completed successfully")


# =========================================================
# CLEANUP
# =========================================================

try:
    sf.quit()
    my.quit()

except:
    pass
