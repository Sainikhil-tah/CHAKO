#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
Game::Game()
    : engine(DEFAULT_DEPTH), gameMode(GameMode::HumanVsHuman), humanSide(WHITE)
{}
int Game::getDepth() const{
    return engine.getDepth();
}
void Game::run(){
    showMainMenu();
    board.initStartPosition();
    std::string status="Game started. Type 'help' for commands.";
    while (true) {
        std::string result;
        if (isGameOver(result)){
            Utils::clearScreen();
            printFullUI();
            std::cout << "\n  *** " << result << " ***\n";
            std::cout << "  Play again? (y/n): ";
            std::string ans;
            std::getline(std::cin,ans);
            ans = Utils::trim(ans);
            if (ans == "y" || ans == "Y"){
                board.initStartPosition();
                undoMgr=UndoManager();
                status="New game started.";
                continue;
            }
            break;
        }
        Utils::clearScreen();
        printFullUI();
        printStatus(status);
        status = "";
        // as we used enum class 
        bool engineTurn=(gameMode==GameMode::HumanVsEngine&&board.sideToMove!=humanSide);
        if (engineTurn){
            doEngineTurn(status);
        }else{
            doHumanTurn(status);
        }
    }
}
void Game::showMainMenu() {
    Utils::clearScreen();
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════╗\n";
    std::cout << "  ║       C++ Terminal Chess         ║\n";
    std::cout << "  ╚══════════════════════════════════╝\n\n";
    std::cout << "  Game Mode:\n";
    std::cout << "    1. Human vs Human\n";
    std::cout << "    2. Human vs Engine\n\n";
    std::cout << "  Choose [1/2]: ";
    std::string line;
    std::getline(std::cin, line);
    line = Utils::trim(line);
    // selected the engine mode
    if (line == "2") {
        gameMode = GameMode::HumanVsEngine;
        std::cout << "\n  Choose your side:\n";
        std::cout << "    1. White\n";
        std::cout << "    2. Black\n";
        std::cout << "  Choose [1/2]: ";
        std::getline(std::cin, line);
        line = Utils::trim(line);
        humanSide = (line == "2") ? BLACK : WHITE;
        std::cout << "\n  Engine search depth [1-7, default " << DEFAULT_DEPTH << "]: ";
        std::getline(std::cin, line);
        line = Utils::trim(line);
        if(!line.empty() && std::isdigit(static_cast<unsigned char>(line[0]))){
            int d=std::stoi(line);
            d=std::max(1, std::min(7, d));
            engine.setDepth(d);
        }
        std::cout << "\n  Engine time limit per move in seconds [default 60]: ";
        std::getline(std::cin, line);
        line = Utils::trim(line);
        if (!line.empty() && std::isdigit(static_cast<unsigned char>(line[0]))) {
            long long seconds = std::stoll(line);
            seconds = std::max(1LL, seconds);
            engine.setTimeLimit(seconds * 1000);
        } else {
            engine.setTimeLimit(60000);
        }
        std::cout << "\n  Engine depth: " << engine.getDepth() << "\n";
        std::cout << "  Engine time limit: " << engine.getTimeLimit() / 1000 << " seconds per move\n";
    }else{
        gameMode = GameMode::HumanVsHuman;
    }
}
// does not interchange 
// void Game::printBoard()const{
//     std::cout << "\n";
//     std::cout << "     a   b   c   d   e   f   g   h\n";
//     std::cout << "   +---+---+---+---+---+---+---+---+\n";
//     for (int r = 0;r<BOARD_SIZE;++r) {
//         int rank = 8 - r;
//         std::cout << " " << rank << " |";
//         for (int c = 0; c < BOARD_SIZE; ++c) {
//             char p = board.board[r][c];
//             std::string sym = Utils::unicode(p);
//             std::cout << " " << sym << " |";
//         }
//         std::cout << " " << rank << "\n";
//         std::cout << "   +---+---+---+---+---+---+---+---+\n";
//     }
//     std::cout << "     a   b   c   d   e   f   g   h\n";
// }
void Game::printBoard() const {
    std::cout << "\n";
    bool whitePerspective = (board.sideToMove == WHITE);
    // Files
    std::cout << "     ";
    if (whitePerspective) {
        for (char f = 'a'; f <= 'h'; ++f) {
            std::cout << f << "   ";
        }
    }
    else {
        for (char f = 'h'; f >= 'a'; --f) {
            std::cout << f << "   ";
        }
    }
    std::cout << "\n";
    std::cout << "   +---+---+---+---+---+---+---+---+\n";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        int r = whitePerspective ? i : (7 - i);
        int rank = 8 - r;
        std::cout << " " << rank << " |";
        for (int j = 0; j < BOARD_SIZE; ++j) {
            int c = whitePerspective ? j : (7 - j);
            char p = board.board[r][c];
            std::string sym = Utils::unicode(p);
            std::cout << " " << sym << " |";
        }
        std::cout << " " << rank << "\n";
        std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }
    // Bottom files
    std::cout << "     ";
    if (whitePerspective) {
        for (char f = 'a'; f <= 'h'; ++f) {
            std::cout << f << "   ";
        }
    }
    else {
        for (char f = 'h'; f >= 'a'; --f) {
            std::cout << f << "   ";
        }
    }
    std::cout << "\n";
}
void Game::printMoveHistory() const {
    std::cout << "\n";
    std::cout << "  #    White         Black\n";
    std::cout << "  ---  ----------    ----------\n";

    size_t count = std::max(board.whiteMoves.size(), board.blackMoves.size());
    for (size_t i = 0; i < count; ++i) {
        std::cout << "  " << std::setw(2) << (i + 1) << ".  ";
        if (i < board.whiteMoves.size()) {
            std::cout << std::left << std::setw(14)
                      << board.whiteMoves[i].toAlgebraic();
        } else {
            std::cout << std::left << std::setw(14) << "";
        }
        if (i < board.blackMoves.size()) {
            std::cout << board.blackMoves[i].toAlgebraic();
        }

        std::cout << "\n";
    }
}
void Game::printCaptured() const {
    std::cout << "\n  Captured by White: ";
    for (char c : board.capturedByWhite) {
        std::cout << Utils::unicode(c) << " ";
    }
    std::cout << "\n";
    std::cout << "  Captured by Black: ";
    for (char c : board.capturedByBlack) {
        std::cout << Utils::unicode(c) << " ";
    }
    std::cout << "\n";
}
// print after every turn 
void Game::printFullUI() const {
    std::string turn = (board.sideToMove == WHITE) ? "White" : "Black";
    std::cout << "\n  ═══════════════ Terminal Chess ═══════════════\n";
    std::cout << "  Turn: " << turn;
    if (board.isInCheck(board.sideToMove)) {
        std::cout << "  *** CHECK! ***";
    }
    std::cout << "    Move #" << board.fullMoveNumber;
    std::cout << "    50-move clock: " << board.halfMoveClock << "\n";
    printBoard();
    printCaptured();
    printMoveHistory();
    std::cout << "\n  Commands: <square> | undo | redo | save <file> | load <file> | resign | help\n";
}
void Game::printStatus(const std::string& msg) const {
    if (!msg.empty()) {
        std::cout << "\n  > " << msg << "\n";
    }
}
// for human vs human and human in engine 
// lets add flip Board
// not required see according to sidetomoveand print board
bool Game::doHumanTurn(std::string& statusMsg) {
    std::string prompt = (board.sideToMove == WHITE) ? "White" : "Black";
    std::cout << "\n  " << prompt << " > Select piece (or command): ";
    std::string line;
    std::getline(std::cin, line);
    line = Utils::trim(line);
    if (line.empty()) {
        return false;
    }
    if (handleCommand(line,statusMsg)) {
        return false;
    }
    return processSquareInput(line,statusMsg);
}
bool Game::processSquareInput(const std::string& line, std::string& statusMsg) {
    int fromRow, fromCol;
    if (!parseSquare(line,fromRow,fromCol)) {
        statusMsg = "Invalid input '" + line + "'. Use square format like e2.";
        return false;
    }
    char p=board.board[fromRow][fromCol];
    if (p==EMPTY) {
        statusMsg="No piece on " +line+ ".";
        return false;
    }
    bool ours=(board.sideToMove == WHITE)? Board::isWhite(p): Board::isBlack(p);
    if (!ours) {
        statusMsg = "That is not your piece.";
        return false;
    }
    std::vector<Move> legal = movesFrom(fromRow, fromCol);
    if (legal.empty()) {
        statusMsg = "No legal moves for that piece.";
        return false;
    }

    std::cout << "\n  Possible moves from " << line << ": ";

    for (const Move& m : legal) {
        std::cout << Utils::colToChar(m.toCol);
        std::cout << Utils::rowToRank(m.toRow);

        if (m.promotion != EMPTY) {
            std::cout << "=" << m.promotion;
        }

        std::cout << " ";
    }

    std::cout << "\n";

    std::cout << "  Destination (or 'cancel'): ";

    std::string dest;
    std::getline(std::cin, dest);
    dest = Utils::trim(dest);

    if (dest == "cancel" || dest == "c" || dest.empty()) {
        statusMsg = "Move cancelled.";
        return false;
    }

    int toRow, toCol;

    if (!parseSquare(dest, toRow, toCol)) {
        statusMsg = "Invalid destination.";
        return false;
    }

    std::vector<Move> candidates;

    for (const Move& m : legal) {
        if (m.toRow == toRow && m.toCol == toCol) {
            candidates.push_back(m);
        }
    }

    if (candidates.empty()) {
        statusMsg = "Illegal destination.";
        return false;
    }

    Move chosen = candidates.front();
    // normally from a place to place we have only one possible 
    // but if more than one possible that is a promotion move 
    if (candidates.size() > 1) {
        bool white = (board.sideToMove == WHITE);

        std::cout << "\n  Choose promotion:\n";
        std::cout << "    1. Queen  (" << Utils::unicode(white ? W_QUEEN  : B_QUEEN)  << ")\n";
        std::cout << "    2. Rook   (" << Utils::unicode(white ? W_ROOK   : B_ROOK)   << ")\n";
        std::cout << "    3. Bishop (" << Utils::unicode(white ? W_BISHOP : B_BISHOP) << ")\n";
        std::cout << "    4. Knight (" << Utils::unicode(white ? W_KNIGHT : B_KNIGHT) << ")\n";
        std::cout << "  Choose [1-4, default 1]: ";

        std::string pchoice;
        std::getline(std::cin, pchoice);
        pchoice = Utils::trim(pchoice);

        char promoChar = white ? W_QUEEN : B_QUEEN;

        if (pchoice == "2") {
            promoChar = white ? W_ROOK : B_ROOK;
        }

        if (pchoice == "3") {
            promoChar = white ? W_BISHOP : B_BISHOP;
        }

        if (pchoice == "4") {
            promoChar = white ? W_KNIGHT : B_KNIGHT;
        }

        for (const Move& m : candidates) {
            if (m.promotion == promoChar) {
                chosen = m;
                break;
            }
        }
    }
    undoMgr.saveSnapshot(board);
    undoMgr.clearRedo();
    // Real human move.
    // makeMove() should push this into whiteMoves/blackMoves.
    char captured=EMPTY;
    if (chosen.isEnPassant){
        int epRow=(board.sideToMove==WHITE)?chosen.toRow+1:chosen.toRow - 1;
        captured=board.board[epRow][chosen.toCol];
    }else{
        captured=board.board[chosen.toRow][chosen.toCol];
    }
    if(board.sideToMove==WHITE) {
        board.whiteMoves.push_back(chosen);
        if (captured!=EMPTY) {
            board.capturedByWhite.push_back(captured);
        }
    }else{
        board.blackMoves.push_back(chosen);
        if (captured!=EMPTY){
            board.capturedByBlack.push_back(captured);
        }
    }
    // board.makeMove(chosen);
    board.makeMove(chosen);
    statusMsg = "Moved: " + chosen.toAlgebraic();
    return true;
}
void Game::doEngineTurn(std::string& statusMsg) {
    std::cout << "\n  Engine thinking" << "  [depth " << engine.getDepth() << ", limit " << engine.getTimeLimit() / 1000 << "s]" << "...\n";
    std::cout.flush();
    undoMgr.saveSnapshot(board);
    Move best = engine.findBestMove(board);
    if (best.piece == EMPTY) {
        statusMsg = "Engine has no moves.";
        return;
    }
    // Real engine move.
    // makeMove() should push this into whiteMoves/blackMoves.
    char captured=EMPTY;
    if (best.isEnPassant) {
        int epRow=(board.sideToMove==WHITE)?best.toRow+1:best.toRow-1;
        captured=board.board[epRow][best.toCol];
    }else{
        captured=board.board[best.toRow][best.toCol];
    }
    if (board.sideToMove==WHITE) {
        board.whiteMoves.push_back(best);
        if (captured!=EMPTY)
            board.capturedByWhite.push_back(captured);
    }else{
        board.blackMoves.push_back(best);
        if (captured!=EMPTY)
            board.capturedByBlack.push_back(captured);
    }
    // in make no update in captured,moves made 
    // only board castling all those changes
    board.makeMove(best);
    statusMsg = "Engine played: " + best.toAlgebraic()+ "  [depth: " + std::to_string(engine.getDepth())+ ", nodes: " + std::to_string(engine.nodesSearched)+ ", time: " + std::to_string(engine.TimeMs) + " ms"+ ", score: " + std::to_string(engine.cp_score) + " cp]";
}

bool Game::handleCommand(const std::string& cmd, std::string& statusMsg) {
    std::istringstream iss(cmd);
    std::string token;
    iss >> token;
    std::string ltoken = token;
    std::transform(ltoken.begin(),ltoken.end(),ltoken.begin(),[](unsigned char c) {return std::tolower(c);});
    if (ltoken=="undo") {
        if (!undoMgr.canUndo()) {
            statusMsg = "Nothing to undo.";
            return true;
        }
        undoMgr.undo(board);
        // In Human vs Engine mode, undo engine move also.
        if (gameMode == GameMode::HumanVsEngine && undoMgr.canUndo()) {
            undoMgr.undo(board);
        }
        statusMsg = "Undo applied.";
        return true;
    }
    if (ltoken == "redo") {
        if (!undoMgr.canRedo()) {
            statusMsg = "Nothing to redo.";
            return true;
        }
        undoMgr.redo(board);
        // In Human vs Engine mode, redo engine move also.
        if (gameMode == GameMode::HumanVsEngine && undoMgr.canRedo()) {
            undoMgr.redo(board);
        }
        statusMsg="Redo applied.";
        return true;
    }
    if (ltoken == "save") {
        std::string filename;
        if (!(iss >> filename)) {
            filename = "game.save";
        }
        saveGame(filename, statusMsg);
        return true;
    }
    if (ltoken == "load") {
        std::string filename;
        if (!(iss >> filename)) {
            filename = "game.save";
        }
        loadGame(filename, statusMsg);
        return true;
    }
    if (ltoken == "resign") {
        std::string side = (board.sideToMove == WHITE) ? "White" : "Black";
        std::cout << "\n  " << side << " resigns. Game over.\n";
        board.halfMoveClock = 200;
        statusMsg = side + " resigned.";
        return true;
    }
    if (ltoken == "help") {
        std::cout << "\n  ╔════════════ Commands ════════════╗\n";
        std::cout << "  ║  <square>       Select piece     ║\n";
        std::cout << "  ║  undo           Undo last move   ║\n";
        std::cout << "  ║  redo           Redo last move   ║\n";
        std::cout << "  ║  save <file>    Save game        ║\n";
        std::cout << "  ║  load <file>    Load game        ║\n";
        std::cout << "  ║  resign         Resign           ║\n";
        std::cout << "  ║  help           Show this        ║\n";
        std::cout << "  ╚══════════════════════════════════╝\n";
        std::cout << "\n  Press Enter to continue...";
        std::string dummy;
        std::getline(std::cin, dummy);
        return true;
    }
    return false;
}
bool Game::isGameOver(std::string& result) {
    if (board.halfMoveClock>=100) {
        result="Draw by fifty-move rule.";
        return true;
    }
    MoveGenerator mg;
    // using func from movegeneratoe 
    std::vector<Move> legal=mg.generateLegalMoves(board);
    if (legal.empty()){
        if (board.isInCheck(board.sideToMove)) {
            std::string winner=(board.sideToMove==WHITE)?"Black":"White";
            result="Checkmate! "+winner+" wins!";
        } else {
            result="Stalemate — draw.";
        }
        return true;
    }
    return false;
}
void Game::saveGame(const std::string& filename,std::string& statusMsg) {
    std::string path="saves/"+filename;
    std::ofstream ofs(path);
    if (!ofs.is_open()){
        statusMsg="Cannot open: "+path;
        return;
    }
    ofs << board.serialize();
    statusMsg="Saved to"+path;
}

void Game::loadGame(const std::string& filename, std::string& statusMsg) {
    std::string path="saves/"+filename;
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        statusMsg="Cannot open:"+path;
        return;
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    if (board.deserialize(oss.str())) {
        undoMgr=UndoManager();
        statusMsg="Loaded from " + path;
    } else {
        statusMsg="Failed to parse save file.";
    }
}
bool Game::parseSquare(const std::string& s,int& row,int& col) {
    if(s.size()<2){
        return false;
    }
    char cChar=static_cast<char>(
        std::tolower(static_cast<unsigned char>(s[0]))
    );
    char rChar=s[1];
    if (cChar<'a'||cChar>'h') {
        return false;
    }
    if(rChar<'1'||rChar>'8') {
        return false;
    }
    col=cChar-'a';
    row='8'-rChar;
    return true;
}
std::vector<Move> Game::movesFrom(int row, int col) {
    MoveGenerator mg;
    std::vector<Move> all = mg.generateLegalMoves(board);
    std::vector<Move> result;
    for (const Move& m : all) {
        if (m.fromRow == row && m.fromCol == col) {
            result.push_back(m);
        }
    }
    return result;
}
