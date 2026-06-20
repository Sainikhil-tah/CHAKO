const game = new Chess();

const board = Chessboard('board', {

    position:'start',

    pieceTheme:
        'https://chessboardjs.com/img/chesspieces/wikipedia/{piece}.png'
});

let started = false;

let waitingReset = false;

let lastLoggedGame = -1;

// =========================
// MATCH HISTORY
// =========================

function addGameHistory(
    gameNumber,
    result,
    materialDiff,
    totalMoves,
    gameTime,
    avgTime
){

    let cls = "draw";

    if(result === "WON")
        cls = "win";

    if(result === "LOST")
        cls = "loss";

    const div = document.createElement("div");

    div.className =
        `history-item ${cls}`;

    div.innerText =
`Game ${gameNumber}: ${result} | Material diff = ${materialDiff} | TOTAL_MOVES = ${totalMoves}
Game Time          : ${gameTime.toFixed(2)} sec
Average Time/Move  : ${avgTime.toFixed(4)} sec`;

    document
        .getElementById("history")
        .prepend(div);
}

// =========================
// START ARENA
// =========================

async function startArena(){

    if(started)
        return;

    const startBtn =
        document.getElementById("startBtn");

    const sf_elo = parseInt(
        document.getElementById("sf_elo").value
    );

    const sf_depth = parseInt(
        document.getElementById("sf_depth").value
    );

    const my_depth = parseInt(
        document.getElementById("my_depth").value
    );

    const movetime = parseInt(
        document.getElementById("movetime").value
    );

    const games = parseInt(
        document.getElementById("games").value
    );

    if(

        sf_elo < 1320 ||

        sf_depth < 1 ||

        my_depth < 1 ||

        movetime < 1 ||

        games < 1
    ){

        alert(
            "Please enter valid values"
        );

        return;
    }

    started = false;

    waitingReset = false;

    lastLoggedGame = -1;

    startBtn.disabled = true;

    document
        .getElementById("history")
        .innerHTML = "";

    document
        .getElementById("wins")
        .innerHTML = 0;

    document
        .getElementById("losses")
        .innerHTML = 0;

    document
        .getElementById("draws")
        .innerHTML = 0;

    document
        .getElementById("estimated_elo")
        .innerHTML = "---";

    document
        .getElementById("status")
        .innerHTML = "STARTING...";

    game.reset();

    board.start();

    try{

        const response = await fetch(
            "/start",
            {

                method:"POST",

                headers:{
                    "Content-Type":"application/json"
                },

                body:JSON.stringify({

                    sf_elo:sf_elo,

                    sf_depth:sf_depth,

                    my_depth:my_depth,

                    movetime:movetime,

                    games:games
                })
            }
        );

        const data =
            await response.json();

        if(data.ok){

            started = true;

            document
                .getElementById("status")
                .innerHTML = "RUNNING";

            loop();
        }

    }catch(error){

        console.error(error);

        alert(error.message);

        startBtn.disabled = false;
    }
}

// =========================
// LOOP
// =========================

async function loop(){

    if(!started || waitingReset)
        return;

    try{

        const response =
            await fetch("/next_move");

        const data =
            await response.json();

        if(data.finished){

            started = false;

            document
                .getElementById("status")
                .innerHTML = "FINISHED";

            document
                .getElementById("startBtn")
                .disabled = false;

            return;
        }

        if(data.waiting){

            setTimeout(loop,100);

            return;
        }

        document
            .getElementById("wins")
            .innerHTML = data.wins;

        document
            .getElementById("losses")
            .innerHTML = data.losses;

        document
            .getElementById("draws")
            .innerHTML = data.draws;

        document
            .getElementById("game")
            .innerHTML =
                "Game " + data.game;

        // =========================
        // ELO
        // =========================

        const totalGames =
            data.wins +
            data.losses +
            data.draws;

        if(totalGames > 0){

            const score =
                (
                    data.wins +
                    0.5 * data.draws
                ) / totalGames;

            let eloDiff;

            if(score === 1)
                eloDiff = 200;

            else if(score === 0)
                eloDiff = -300;

            else{

                eloDiff =
                    -400 *
                    Math.log10(
                        (1 / score) - 1
                    );
            }

            const estimatedElo =
                Math.round(

                    parseInt(
                        document
                            .getElementById("sf_elo")
                            .value
                    ) + eloDiff
                );

            document
                .getElementById("estimated_elo")
                .innerHTML =
                    estimatedElo;
        }

        // =========================
        // GAME RESET
        // =========================

        if(data.reset){

            if(
                data.game !==
                lastLoggedGame
            ){

                lastLoggedGame =
                    data.game;

                addGameHistory(

                    data.game,

                    data.result,

                    data.material_diff,

                    data.total_moves,

                    data.game_time,

                    data.avg_time
                );
            }

            waitingReset = true;

            document
                .getElementById("status")
                .innerHTML =
                    data.status;

            setTimeout(()=>{

                game.reset();

                board.start();

                document
                    .getElementById("status")
                    .innerHTML =
                        "RUNNING";

                waitingReset = false;

                setTimeout(loop,50);

            },500);

            return;
        }

        // =========================
        // PLAY MOVE
        // =========================

        const move = data.move;

        const moveObj = game.move({

            from:move.substring(0,2),

            to:move.substring(2,4),

            promotion:
                move.length >= 5
                    ? move[4]
                    : 'q'
        });

        if(moveObj){

            board.position(
                game.fen()
            );
        }

        setTimeout(loop,50);

    }catch(error){

        console.error(error);

        started = false;

        document
            .getElementById("status")
            .innerHTML = "ERROR";

        document
            .getElementById("startBtn")
            .disabled = false;
    }
}
