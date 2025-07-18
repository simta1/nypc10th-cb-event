let board = [], owner = [];
let rows = 11, cols = 10;
let cellSize = 60;
const ME = 1;
const PINKBIN = 2;
let player = ME;

let easyMode = true;
let highlight = [];
let pos = [];

let flipTimers = [];

let historyStack = [];
let redoStack = [];

// 마우스 드래그
let startX, startY, endX, endY;
let dragging = false;

let predefinedBoard = [];

async function loadBoard() {
	let response = await fetch('board.txt');
	let text = await response.text();
	let lines = text.trim().split('\n');
	predefinedBoard = lines.map(line => line.trim());
}

async function setup() {
	await loadBoard();

	try {
		const res = await fetch('solution.txt');
		if (res.ok) {
			const data = await res.text();
			init(data);
		}
		else console.warn('solution.txt 열지 못함');
	}
	catch (e) {
		console.warn('solution.txt 로딩 중 예외 발생, 무시함:', e);
	}

	createCanvas(cols * cellSize, rows * cellSize + 40);
	textAlign(CENTER, CENTER);
	textSize(24);
	rectMode(CORNER);
	angleMode(DEGREES);
	noStroke();

	for (let i = 0; i < rows; i++) {
		board[i] = [];
		owner[i] = [];
		highlight[i] = [];
		flipTimers[i] = [];
		for (let j = 0; j < cols; j++) {
			board[i][j] = predefinedBoard[i][j] - '0';
			owner[i][j] = 0;
			flipTimers[i][j] = 0;
		}
	}
	updateHighlight();
}

function draw() {
	background(25, 30, 50);
    
    let aCount = 0, bCount = 0;
	for (let i = 0; i < rows; i++) {
		for (let j = 0; j < cols; j++) {
			if (owner[i][j] === 1) ++aCount;
			else if (owner[i][j] === 2) ++bCount;
		}
    }
	let diff = aCount - bCount;
	let playerName = player === 1 ? "A (Green)" : "B (Pink)";
	let statusText = `Current Player: ${playerName}   (A: ${aCount}, B: ${bCount}, Diff: ${diff})`;

	fill(255);
	textAlign(LEFT, CENTER);
	textSize(20);
	text(statusText, 10, height - 20);
	textAlign(CENTER, CENTER);
	textSize(24);

	for (let i = 0; i < rows; i++) {
		for (let j = 0; j < cols; j++) {
			let px = j * cellSize;
			let py = i * cellSize;
			let val = board[i][j];

			if (flipTimers[i][j] > 1) flipTimers[i][j] -= 1;
			let flipRatio = flipTimers[i][j] / 10.0;

			push();
			translate(px + cellSize / 2, py + cellSize / 2);
			scale(1, abs(cos(flipRatio * 180)));

			if (owner[i][j] === 1) fill(0, 180, 120);
			else if (owner[i][j] === 2) fill(255, 120, 180);
			else fill(40, 45, 90);
			rectMode(CENTER);
			rect(0, 0, cellSize - 4, cellSize - 4, 8);
			pop();

			stroke(80, 100, 180);
			strokeWeight(2);
			noFill();
			rect(px, py, cellSize, cellSize, 8);
			noStroke();

			if (easyMode && highlight[i][j] && owner[i][j] === 0 && board[i][j] !== 0) {
				fill(180, 180, 180, 100);
				rect(px, py, cellSize, cellSize, 8);
			}

			if (val !== 0) {
				fill(255);
				text(val, px + cellSize / 2, py + cellSize / 2);
			}
		}
        
        if (easyMode) {
            push();
            noFill();
            stroke(255, 0, 0, 150);

            for (let {i1, i2, j1, j2} of pos) {
                let j = j1 * cellSize;
                let i = i1 * cellSize;
                let w = (j2 - j1 + 1) * cellSize;
                let h = (i2 - i1 + 1) * cellSize;
                rect(j, i, w, h, 8);
            }
            
            if (pos.length !== 0) {
                let {i1, i2, j1, j2} = pos.reduce((a, b) => { // TODO 아직 정확한 기준 모르긴 함
                    if (a.i1 < b.i1) return a;
                    if (a.i1 > b.i1) return b;
                    return a.j1 <= b.j1 ? a : b;
                });
                let j = j1 * cellSize;
                let i = i1 * cellSize;
                let w = (j2 - j1 + 1) * cellSize;
                let h = (i2 - i1 + 1) * cellSize;
                fill(255, 255, 127, 30);
                rect(j, i, w, h, 8);
            }

            pop();
        }
	}

	if (dragging) {
        let j1 = max(0, floor(min(startX, mouseX) / cellSize));
        let i1 = max(0, floor(min(startY, mouseY) / cellSize));
        let j2 = min(cols - 1, floor(max(startX, mouseX) / cellSize));
        let i2 = min(rows - 1, floor(max(startY, mouseY) / cellSize));

        fill(255, 255, 255, 70);
		for (let i = i1; i <= i2; i++) {
			for (let j = j1; j <= j2; j++) {
                rect(j * cellSize, i * cellSize, cellSize, cellSize, 8);
			}
		}

		stroke(255, 255, 255, 120);
		strokeWeight(2);
		noFill();
		rect(j1 * cellSize, i1 * cellSize, (j2 - j1 + 1) * cellSize, (i2 - i1 + 1) * cellSize, 8);
		noStroke();
	}
}

function mousePressed() {
	startX = mouseX;
	startY = mouseY;
	dragging = true;
}

function mouseReleased() {
	dragging = false;
	endX = mouseX;
	endY = mouseY;

	let j1 = max(0, floor(min(startX, endX) / cellSize));
	let i1 = max(0, floor(min(startY, endY) / cellSize));
	let j2 = min(cols - 1, floor(max(startX, endX) / cellSize));
	let i2 = min(rows - 1, floor(max(startY, endY) / cellSize));

	let sum = 0;
	for (let i = i1; i <= i2; i++) {
		for (let j = j1; j <= j2; j++) {
            sum += board[i][j];
		}
	}
    if (sum !== 10) return;

    flip(i1, i2, j1, j2);
}

function flip(i1, i2, j1, j2) {
    saveState();
	for (let i = i1; i <= i2; i++) {
		for (let j = j1; j <= j2; j++) {
            owner[i][j] = player;
            board[i][j] = 0;
            flipTimers[i][j] = 10;
        }
    }
    player = 3 - player;
    updateHighlight();
}

function keyPressed() {
    if (key === 'h' || key === 'H') {
        easyMode = !easyMode;
        updateHighlight();
    }
    if (key === 'z' || key === 'Z') {
        undo();
        updateHighlight();
    }
    if (key === 'y' || key === 'Y') {
        redo();
        updateHighlight();
    }
    if (key === ' ') {
        player = 3 - player;
        updateHighlight();
    }
    if (key === 'b' || key === 'B') {
        if (pos.length === 0) return;
        let {i1, i2, j1, j2} = pos.reduce((a, b) => { // TODO
            if (a.i1 < b.i1) return a;
            if (a.i1 > b.i1) return b;
            return a.j1 <= b.j1 ? a : b;
        });
        flip(i1, i2, j1, j2);
    }
	if (key === 'c' || key === 'C') {
		if (flipQueue.length > 0) {
			let [i1, j1, i2, j2] = flipQueue.shift();
			if (i1 == -1 && j1 == -1 && i2 == -1 && j2 == -1) {
				console.log(`skip turn`);
				player = 3 - player;
				updateHighlight();
			}
			else {
				console.log(`Flipping (${i1}, ${j1}) to (${i2}, ${j2})`);
				flip(i1, i2, j1, j2);
			}
		}
		else console.log('No more flip instructions.');
	}
}

function updateHighlight() {
	let pfs = Array(rows + 1).fill().map(() => Array(cols + 1).fill(0));
	for (let i = 1; i <= rows; i++) {
		for (let j = 1; j <= cols; j++) {
			pfs[i][j] = board[i - 1][j - 1] + pfs[i][j - 1] + pfs[i - 1][j] - pfs[i - 1][j - 1];
		}
	}

    const getSum = (i1, i2, j1, j2) => pfs[i2 + 1][j2 + 1] - pfs[i1][j2 + 1] - pfs[i2 + 1][j1] + pfs[i1][j1];

	for (let i = 0; i < rows; i++) {
		for (let j = 0; j < cols; j++) {
			highlight[i][j] = false;
		}
	}
    
    pos = [];
    let mx = 0;
	for (let i1 = 0; i1 < rows; i1++) {
		for (let i2 = i1; i2 < rows; i2++) {
			for (let j1 = 0; j1 < cols; j1++) {
				for (let j2 = j1; j2 < cols; j2++) {
                    if (getSum(i1, i1, j1, j2) == 0) continue;
                    if (getSum(i2, i2, j1, j2) == 0) continue;
                    if (getSum(i1, i2, j1, j1) == 0) continue;
                    if (getSum(i1, i2, j2, j2) == 0) continue;

					let sum = getSum(i1, i2, j1, j2);
                    if (sum !== 10) continue;

                    let cnt = [0, 0, 0];
                    for (let i = i1; i <= i2; i++) {
                        for (let j = j1; j <= j2 ;j++) {
                            ++cnt[owner[i][j]];
                        }
                    }
                    let val = cnt[0] + cnt[3 - player] * 2;
                    if (mx < val) {
                        mx = val;
                        pos = [];
                    }
                    if (mx == val) pos.push({ i1, i2, j1, j2 });

                    for (let i = i1; i <= i2; i++) {
                        for (let j = j1; j <= j2; j++) {
                            if (owner[i][j] === 0 && board[i][j] !== 0) highlight[i][j] = true;
                        }
                    }

				}
			}
		}
	}
}

function saveState() {
	let state = {
		board: board.map(row => row.slice()),
		owner: owner.map(row => row.slice()),
		player: player
	};
	historyStack.push(state);
	redoStack = [];
}

function undo() {
	if (historyStack.length === 0) return;
	let prevState = historyStack.pop();
	let currentState = {
		board: board.map(row => row.slice()),
		owner: owner.map(row => row.slice()),
		player: player
	};
	redoStack.push(currentState);
	board = prevState.board.map(row => row.slice());
	owner = prevState.owner.map(row => row.slice());
	player = prevState.player;
	updateHighlight();
}

function redo() {
	if (redoStack.length === 0) return;
	let nextState = redoStack.pop();
	let currentState = {
		board: board.map(row => row.slice()),
		owner: owner.map(row => row.slice()),
		player: player
	};
	historyStack.push(currentState);
	board = nextState.board.map(row => row.slice());
	owner = nextState.owner.map(row => row.slice());
	player = nextState.player;
	updateHighlight();
}

let flipQueue = [];
function init(data) {
	const lines = data.trim().split('\n');
	for (let i = 1; i < lines.length; i++) {
		let line = lines[i].trim();
		if (line === "skip turn") line = "-1 -1 -1 -1";
		flipQueue.push(line.trim().split(/\s+/).map(Number));
	}
	console.log('Flip queue initialized with', flipQueue.length, 'entries.');
}