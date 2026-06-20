from flask import Flask, jsonify, send_file, request
from flask_cors import CORS

import subprocess
import chess
import time

app = Flask(__name__)

CORS(app)

# =========================================================
# UCI LOG
# =========================================================

log_file = open(
    "uci_log.txt",
    "a",
    buffering=1
)

def log(msg):

    t = time.strftime("%H:%M:%S")

    s = f"[{t}] {msg}"

    print(s)

    log_file.write(s + "\n")

def send_cmd(engine, cmd, name):

    log(f"[{name} <<] {cmd}")

    engine.stdin.write(cmd + "\n")

    engine.stdin.flush()

def read_line(engine, name):

    line = engine.stdout.readline().strip()

    log(f"[{name} >>] {line}")

    return line

# =========================================================
# ENGINES
# =========================================================

engine1 = subprocess.Popen(

    ["./chess", "uci"],

    stdin=subprocess.PIPE,

    stdout=subprocess.PIPE,

    text=True,

    bufsize=1
)

engine2 = subprocess.Popen(

    ["stockfish"],

    stdin=subprocess.PIPE,

    stdout=subprocess.PIPE,

    text=True,

    bufsize=1
)

# =========================================================
# GLOBALS
# =========================================================

board = chess.Board()

wins = 0
losses = 0
draws = 0

game_number = 1

SF_ELO = 1200
SF_DEPTH = 8
MY_DEPTH = 4

MOVE_TIME = 3000

NUM_GAMES = 10

running = False

game_start_time = 0

# =========================================================
# ENGINE STATS
# =========================================================

my_total_nodes = 0
my_total_time = 0.0
my_total_moves = 0

# =========================================================
# ENGINE INIT
# =========================================================

def init_engine1():

    send_cmd(
        engine1,
        "uci",
        "MY_ENGINE"
    )

    while True:

        line = read_line(
            engine1,
            "MY_ENGINE"
        )

        if line == "uciok":
            break

def setup_stockfish():

    send_cmd(
        engine2,
        "uci",
        "STOCKFISH"
    )

    while True:

        line = read_line(
            engine2,
            "STOCKFISH"
        )

        if line == "uciok":
            break

    send_cmd(

        engine2,

        "setoption name UCI_LimitStrength value true",

        "STOCKFISH"
    )

    send_cmd(

        engine2,

        f"setoption name UCI_Elo value {SF_ELO}",

        "STOCKFISH"
    )

    send_cmd(

        engine2,

        "isready",

        "STOCKFISH"
    )

    while True:

        line = read_line(
            engine2,
            "STOCKFISH"
        )

        if line == "readyok":
            break

# =========================================================
# INIT
# =========================================================

init_engine1()

setup_stockfish()

# =========================================================
# GET BEST MOVE
# =========================================================

def get_bestmove(
    engine,
    depth,
    name
):

    global my_total_nodes
    global my_total_time
    global my_total_moves

    moves = " ".join(
        [m.uci() for m in board.move_stack]
    )

    cmd = f"position startpos moves {moves}"

    send_cmd(
        engine,
        cmd,
        name
    )

    start_time = time.perf_counter()

    send_cmd(
        engine,
        f"go depth {depth} movetime {MOVE_TIME}",
        name
    )

    bestmove = None

    nodes = 0

    while True:

        line = read_line(
            engine,
            name
        )

        # =================================================
        # PARSE INFO LINES
        # =================================================

        if line.startswith("info"):

            parts = line.split()

            if "nodes" in parts:

                try:

                    idx = parts.index("nodes")

                    nodes = int(parts[idx + 1])

                except:
                    pass

        # =================================================
        # BESTMOVE
        # =================================================

        if line.startswith("bestmove"):

            parts = line.split()

            if len(parts) >= 2:

                bestmove = parts[1]

            break

    elapsed = (
        time.perf_counter() - start_time
    )

    nps = (
        nodes / max(elapsed, 1e-9)
    )

    # =====================================================
    # TRACK MY ENGINE ONLY
    # =====================================================

    if name == "MY_ENGINE":

        my_total_nodes += nodes

        my_total_time += elapsed

        my_total_moves += 1

    # =====================================================
    # LOG PERFORMANCE
    # =====================================================

    log(
        f"{name} | "
        f"Time={elapsed:.4f}s | "
        f"Nodes={nodes} | "
        f"NPS={nps:.0f}"
    )

    return bestmove

# =========================================================
# ROUTES
# =========================================================

@app.route("/")
def home():

    return send_file("index.html")

@app.route("/app.js")
def js():

    return send_file("app.js")

# =========================================================
# START
# =========================================================

@app.route("/start", methods=["POST"])

def start():

    global SF_ELO
    global SF_DEPTH
    global MY_DEPTH
    global MOVE_TIME
    global NUM_GAMES

    global wins
    global losses
    global draws

    global game_number

    global board

    global running

    global game_start_time

    global my_total_nodes
    global my_total_time
    global my_total_moves

    data = request.json

    SF_ELO = int(data["sf_elo"])

    SF_DEPTH = int(data["sf_depth"])

    MY_DEPTH = int(data["my_depth"])

    MOVE_TIME = int(data["movetime"])

    NUM_GAMES = int(data["games"])

    wins = 0
    losses = 0
    draws = 0

    my_total_nodes = 0
    my_total_time = 0.0
    my_total_moves = 0

    game_number = 1

    board = chess.Board()

    game_start_time = time.time()

    setup_stockfish()

    running = True

    log("====================================")
    log("ARENA STARTED")
    log("====================================")

    return jsonify({
        "ok": True
    })

# =========================================================
# NEXT MOVE
# =========================================================

@app.route("/next_move")

def next_move():

    global wins
    global losses
    global draws

    global game_number

    global board

    global running

    global game_start_time

    global my_total_nodes
    global my_total_time
    global my_total_moves

    if not running:

        return jsonify({
            "waiting": True
        })

    # =====================================================
    # FINISHED
    # =====================================================

    if game_number > NUM_GAMES:

        running = False

        avg_nodes = (
            my_total_nodes / max(my_total_moves, 1)
        )

        avg_nps = (
            my_total_nodes / max(my_total_time, 1e-9)
        )

        log("====================================")
        log("ARENA FINISHED")
        log("====================================")

        log("====================================")
        log("ENGINE PERFORMANCE")
        log("====================================")

        log(f"TOTAL NODES : {my_total_nodes}")

        log(f"TOTAL TIME  : {my_total_time:.4f}s")

        log(f"TOTAL MOVES : {my_total_moves}")

        log(f"AVG NODES   : {avg_nodes:.0f}")

        log(f"AVG NPS     : {avg_nps:.0f}")

        log("====================================")

        return jsonify({

            "finished": True,

            "wins": wins,

            "losses": losses,

            "draws": draws
        })

    # =====================================================
    # GAME OVER
    # =====================================================

    if board.is_game_over():

        result = board.result()

        if result == "1-0":

            wins += 1

            status = "WON"

        elif result == "0-1":

            losses += 1

            status = "LOST"

        else:

            draws += 1

            status = "DRAW"

        finished_game_number = game_number

        total_moves = len(board.move_stack)

        game_time = (
            time.time() - game_start_time
        )

        avg_time = 0

        if total_moves > 0:

            avg_time = (
                game_time / total_moves
            )

        piece_values = {

            chess.PAWN: 1,

            chess.KNIGHT: 3,

            chess.BISHOP: 3,

            chess.ROOK: 5,

            chess.QUEEN: 9
        }

        white_material = 0
        black_material = 0

        for piece_type, value in piece_values.items():

            white_material += (

                len(
                    board.pieces(
                        piece_type,
                        chess.WHITE
                    )
                ) * value
            )

            black_material += (

                len(
                    board.pieces(
                        piece_type,
                        chess.BLACK
                    )
                ) * value
            )

        material_diff = (
            white_material - black_material
        )

        log("====================================")
        log(f"GAME {finished_game_number} FINISHED")
        log(f"RESULT: {status}")
        log(f"MOVES : {total_moves}")
        log(f"TIME  : {game_time:.2f}")
        log("====================================")

        board = chess.Board()

        game_number += 1

        game_start_time = time.time()

        return jsonify({

            "reset": True,

            "wins": wins,

            "losses": losses,

            "draws": draws,

            "game": finished_game_number,

            "status": status,

            "result": status,

            "material_diff": material_diff,

            "total_moves": total_moves,

            "game_time": game_time,

            "avg_time": avg_time
        })

    # =====================================================
    # ENGINE MOVE
    # =====================================================

    if board.turn == chess.WHITE:

        move = get_bestmove(
            engine1,
            MY_DEPTH,
            "MY_ENGINE"
        )

    else:

        move = get_bestmove(
            engine2,
            SF_DEPTH,
            "STOCKFISH"
        )

    # =====================================================
    # INVALID MOVE
    # =====================================================

    if move is None:

        running = False

        log("ENGINE RETURNED NONE MOVE")

        return jsonify({

            "error": True,

            "message":
                "Engine returned invalid move"
        })

    try:

        board.push_uci(move)

        log("")
        log(str(board))
        log(f"MOVE PLAYED: {move}")
        log("")

    except Exception as e:

        running = False

        log(f"INVALID MOVE: {move}")
        log(str(e))

        return jsonify({

            "error": True,

            "message": str(e)
        })

    return jsonify({

        "reset": False,

        "move": move,

        "wins": wins,

        "losses": losses,

        "draws": draws,

        "game": game_number
    })

# =========================================================
# RUN
# =========================================================

if __name__ == "__main__":

    app.run(
        port=5001,
        debug=True
    )
