import chess
import chess.engine
import math
import time
STOCKFISH_ELO=2000
print("----------ENTER THE ELO OF THE STOCKFISH ENGINE---------------")
STOCKFISH_ELO=int(input())
NUM_GAMES=20
print("----------ENTER THE DEPTH OF THE STOCKFISH ENGINE---------------")
STOCKFISH_DEPTH=int(input())
print("----------------ENTER THE NUMBER OF GAMES----------------------")
NUM_GAMES=int(input())
# to find the final difference 
PIECE_VALUES={chess.PAWN:1,chess.KNIGHT: 3,chess.BISHOP:3,chess.ROOK:5,chess.QUEEN:9}
def material_difference(board):
    #difference present in the last of the board
    white=sum(len(board.pieces(pt,chess.WHITE))*v for pt,v in PIECE_VALUES.items())
    black=sum(len(board.pieces(pt,chess.BLACK))*v for pt,v in PIECE_VALUES.items())
    return white-black
# enter the location of ur sf engine
sf=chess.engine.SimpleEngine.popen_uci("/usr/games/stockfish")
sf.configure({"UCI_LimitStrength":True,"UCI_Elo": STOCKFISH_ELO})
my=chess.engine.SimpleEngine.popen_uci(["./chess","uci"])
wins=0
losses=0
draws=0
# Time tracking
my_total_time=0.0
my_move_count=0
sf_total_time=0.0
sf_move_count=0
all_total_time=0.0
all_nodes_search=0
all_move_count=0
print("-------------ENTER GAME DEPTH--------------")
GAME_DEPTH=int(input())
print("-------------ENTER TIME LIMIT IN SECONDS--------------")
TIME_LIMIT=float(input())
for game_num in range(NUM_GAMES):
    # init board
    my_game_time=0
    game_node=0
    board=chess.Board()
    my_is_white=(game_num%2==0)
    total_moves=0
    my_game_move=0
    move_limit_hit=False
    print(f"\n================ GAME {game_num + 1} ================")
    # print("./chess.getdepth()")
    # 
    while not board.is_game_over():
        move_no=board.fullmove_number
        side_to_move="WHITE" if board.turn == chess.WHITE else "BLACK"
        if board.turn==chess.WHITE:
            if my_is_white:
                engine_name="MY_ENGINE"
                my_game_move+=1
                start=time.perf_counter()
                result=my.play(board, chess.engine.Limit(depth=GAME_DEPTH,time=TIME_LIMIT),info=chess.engine.INFO_ALL)
                elapsed=time.perf_counter() - start
                my_total_time+=elapsed
                my_game_time+=elapsed
                my_move_count+=1
                info=sf.analyse(board, chess.engine.Limit(depth=14),info=chess.engine.INFO_ALL)
                score=info["score"].white()
                nodes=result.info.get("nodes", 0)
                all_nodes_search+=nodes 
                game_node+=nodes
                if score.is_mate():
                    cp_str=f"M{score.mate()}"
                else:
                    cp_str=f"{score.score()/100:+.2f}"
            else:
                engine_name = "STOCKFISH"
                start = time.perf_counter()
                result = sf.play(board, chess.engine.Limit(depth=STOCKFISH_DEPTH),info=chess.engine.INFO_ALL)
                elapsed = time.perf_counter() - start
                sf_total_time += elapsed
                sf_move_count += 1
                info=sf.analyse(board, chess.engine.Limit(depth=14),info=chess.engine.INFO_ALL)
                score=info["score"].white()
                nodes=result.info.get("nodes", 0)
                if score.is_mate():
                    cp_str=f"M{score.mate()}"
                else:
                    cp_str=f"{score.score()/100:+.2f}"
        else:
            if my_is_white:
                engine_name = "STOCKFISH"
                start = time.perf_counter()
                result = sf.play(board, chess.engine.Limit(depth=STOCKFISH_DEPTH),info=chess.engine.INFO_ALL)
                elapsed = time.perf_counter() - start
                sf_total_time += elapsed
                sf_move_count += 1
                info=sf.analyse(board, chess.engine.Limit(depth=14))
                score=info["score"].black()
                nodes=result.info.get("nodes", 0)
                if score.is_mate():
                    cp_str=f"M{score.mate()}"
                else:
                    cp_str=f"{score.score()/100:+.2f}"
            else:
                engine_name = "MY_ENGINE"
                my_game_move+=1
                start = time.perf_counter()
                result = my.play(board, chess.engine.Limit(depth=GAME_DEPTH,time=TIME_LIMIT),info=chess.engine.INFO_ALL)
                elapsed = time.perf_counter() - start
                my_total_time+=elapsed
                my_game_time+=elapsed
                my_move_count+=1
                info=sf.analyse(board, chess.engine.Limit(depth=14),info=chess.engine.INFO_ALL)
                nodes=result.info.get("nodes", 0)
                score=info["score"].black()
                all_nodes_search+=nodes 
                game_node+=nodes
                if score.is_mate():
                    cp_str=f"M{score.mate()}"
                else:
                    cp_str=f"{score.score()/100:+.2f}"
        all_total_time += elapsed
        all_move_count += 1
        # score=info["score"]
        # cp = score.white().score()
        # safety checks
        if result.move is None or result.move not in board.legal_moves:
            print(f"  [!] Illegal/null move from engine. FEN: {board.fen()}")
            move_limit_hit = True
            break
        print(f"Move {total_moves + 1:>3} | "f"FullMove={move_no:>3} | "f"Side={side_to_move:<5} | "f"Engine={engine_name:<9} | "f"Move={str(result.move):<5} | "f"Time={elapsed:.4f}s | "f"Cp_score={cp_str} | "f"Nodes_Searched={nodes}")
        board.push(result.move)
        total_moves+=1
    # ── outcome ──────────────────────────────────────────────
    outcome_obj=board.outcome()
    outcome=board.result()
    average_nodes_per_move=game_node/(my_game_move)
    termination=outcome_obj.termination.name
    winner_str=("White" if outcome_obj.winner == chess.WHITE else "Black" if outcome_obj.winner == chess.BLACK else"Draw")
    my_won=(outcome=="1-0" and my_is_white) or (outcome == "0-1" and not my_is_white)
    my_lost=(outcome=="0-1" and my_is_white) or (outcome == "1-0" and not my_is_white)
    diff=material_difference(board)
    color="WHITE" if my_is_white else "BLACK"
    if my_won:
        wins+=1
        label="WON"
    elif my_lost:
        losses+=1
        label="LOST"
    else:
        draws+=1
        label="DRAW"
    print(board)
    #align width 3 as moves 3digit 
    print(f"Game {game_num + 1:>3} | {label:<4} | Engine={color:<5} | "f"Termination={termination:<22} | Winner={winner_str:<5} | "f"MatDiff={diff:>+3} | Moves={total_moves}")
    print(f"Average Nodes searched : {average_nodes_per_move}")
score = (wins + 0.5 * draws) / NUM_GAMES
# prevent log 0 err
if score==1.0:
    elo_diff=1000
elif score==0.0:
    elo_diff=-1000
else:
    elo_diff=-400*math.log10((1 / score) - 1)
estimated_elo=STOCKFISH_ELO+elo_diff
my_avg_time=my_total_time/my_move_count if my_move_count > 0 else 0.0
sf_avg_time=sf_total_time/sf_move_count if sf_move_count > 0 else 0.0
all_avg_time=all_total_time/all_move_count if all_move_count > 0 else 0.0
print("\n========================")
print("FINAL RESULTS")
print("========================")
print(f"Wins            : {wins}")
print(f"Losses          : {losses}")
print(f"Draws           : {draws}")
print(f"Score           : {score:.3f}")
print(f"MY_DEPTH        : {GAME_DEPTH}")
print(f"Estimated Elo ≈ {estimated_elo:.0f}")
print(f"StocFishElo : {STOCKFISH_ELO}")
print(f"StocFishDepth: {STOCKFISH_DEPTH}")

print("\n========================")
print("TIME STATS")
print("========================")
print(f"My engine moves      : {my_move_count}")
print(f"My engine total time : {my_total_time:.4f}s")
print(f"My engine avg time   : {my_avg_time:.4f}s/move")
print(f"\nStockfish moves      : {sf_move_count}")
print(f"Stockfish total time : {sf_total_time:.4f}s")
print(f"Stockfish avg time   : {sf_avg_time:.4f}s/move")
print(f"\nAll moves            : {all_move_count}")
print(f"All total time       : {all_total_time:.4f}s")
print(f"All avg time         : {all_avg_time:.4f}s/move")
sf.quit()
my.quit()
