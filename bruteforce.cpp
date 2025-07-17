#include <bits/stdc++.h>
using namespace std;

constexpr int numberOfChoices = 4;

constexpr int rows = 11, cols = 10;

int board[rows][cols], owner[rows][cols];

int player = 1;

vector<vector<tuple<int, int, int, int> > > moveHistory;

constexpr int INF = 1e5;
int bestScore = -INF;

random_device rd;
mt19937 g(rd());

void init() {
    ifstream fin("board.txt");
    if (!fin) {
        cerr << "input.txt 파일을 열 수 없습니다.\n";
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        string st;
        fin >> st;
        for (int j = 0; j < cols; j++) board[i][j] = st[j] & 15;
    }
    
    fin.close();
}

void print() {
    cout << "---\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) cout << board[i][j] << " ";
        cout << "\n";
    }
}

int currentRound = 0;
void printHistoryWithScore(int score) {
    cout << "---------------\n";
    cout << "history(" << moveHistory.size() << "), score : " << score << ", round : " << currentRound << "\n";
    for (auto tmp : moveHistory) {
        // cout << "---\n";
        // for (auto [i, j, boardVal, ownerVal] : tmp) cout << i << " " << j << " " << boardVal << " " << ownerVal << "\n";
        auto [i1, j1, _, __] = tmp.front();
        auto [i2, j2, ___, ____] = tmp.back();
        cout << i1 << " " << j1 << " " << i2 << " " << j2 << "\n";
    }
}

constexpr int PRINT_INTERVAL = 500000;
void calScore() {
    int score = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (owner[i][j] == 1) ++score;
            else if (owner[i][j] == 2) --score;
        }
    }

    if (bestScore < score) {
        bestScore = score;
        printHistoryWithScore(score);
    }
    
    ++currentRound;
    if (currentRound % PRINT_INTERVAL == 0) cout << "current round: " << currentRound << "\n";
    return;
}

void move(int i1, int i2, int j1, int j2) {
    // cout << "move : " << i1 << " " << j1 << " " << i2 << " " << j2 << "\n";
    vector<tuple<int, int, int, int> > tmp;
    for (int i = i1; i <= i2; i++) {
        for (int j = j1; j <= j2; j++) {
            tmp.emplace_back(i, j, board[i][j], owner[i][j]);
            board[i][j] = 0;
            owner[i][j] = player;
        }
    }
    player = 3 - player;
    moveHistory.push_back(tmp);
}

void backtracking() {
    auto tmp = moveHistory.back();
    moveHistory.pop_back();
    
    for (auto [i, j, boardVal, ownerVal] : tmp) {
        board[i][j] = boardVal;
        owner[i][j] = ownerVal;
    }
    player = 3 - player;
}

int pfs[rows + 1][cols + 1];
void calPfs() {
    for (int j = 0; j <= cols; j++) pfs[0][j] = 0;
    for (int i = 0; i <= rows; i++) pfs[i][0] = 0;

    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= cols; j++) {
            pfs[i][j] = board[i - 1][j - 1] + pfs[i][j - 1] + pfs[i - 1][j] - pfs[i - 1][j - 1];
        }
    }
}

inline int getSum(int i1, int i2, int j1, int j2) {
    return pfs[i2 + 1][j2 + 1] - pfs[i1][j2 + 1] - pfs[i2 + 1][j1] + pfs[i1][j1];
}

bool botMove() {
    calPfs();
    
    int mx = 0;
    int i1, i2, j1, j2;
    
    for (int startRow = 0; startRow < rows; startRow++) {
        for (int startCol = 0; startCol < cols; startCol++) {
            int colPrefCnt[cols][3] = {0};

            int prevRowLastCol = cols;
            for (int row = startRow; row < rows; row++) {
                int cnt[3] = {0};

                for (int col = startCol; col < cols; col++) {
                    ++colPrefCnt[col][owner[row][col]];
                    cnt[0] += colPrefCnt[col][0];
                    cnt[1] += colPrefCnt[col][1];
                    cnt[2] += colPrefCnt[col][2];

                    int sum = getSum(startRow, row, startCol, col);
                    if (sum == 10) {
                        if (getSum(startRow, row, startCol, startCol) && getSum(startRow, startRow, startCol, col) && getSum(row, row, startCol, col)) {
                            int dscore = cnt[0] + 2 * cnt[3 - player];
                            // cout << startRow << " " << startCol << " " << row << " " << col << " : " << dscore << "\n";

                            if (mx < dscore) {
                                mx = dscore;
                                i1 = startRow;
                                j1 = startCol;
                                i2 = row;
                                j2 = col;
                            }
                        }
                        
                        break;
                    }
                    else if (sum > 10) break;
                }
            }
        }
    }

    if (!mx) return false;

    move(i1, i2, j1, j2);
    return true;
}

void personMove() {
    calPfs();

    vector<tuple<int, int, int, int> > candi;
    
    for (int startRow = 0; startRow < rows; startRow++) {
        for (int startCol = 0; startCol < cols; startCol++) {
            int colPrefCnt[cols][3] = {0};

            for (int row = startRow; row < rows; row++) {
                int cnt[3] = {0};

                for (int col = startCol; col < cols; col++) {
                    ++colPrefCnt[col][owner[row][col]];
                    cnt[0] += colPrefCnt[col][0];
                    cnt[1] += colPrefCnt[col][1];
                    cnt[2] += colPrefCnt[col][2];

                    int sum = getSum(startRow, row, startCol, col);
                    if (sum == 10) {
                        if (getSum(startRow, row, startCol, startCol) && getSum(startRow, startRow, startCol, col) && getSum(row, row, startCol, col)) {
                            int dscore = cnt[0] + 2 * cnt[3 - player];
                            // cout << startRow << " " << startCol << " " << row << " " << col << " : " << dscore << "\n";

                            int i1 = startRow;
                            int j1 = startCol;
                            int i2 = row;
                            int j2 = col;
                            candi.emplace_back(i1, j1, i2, j2);
                        }
                        
                        break;
                    }
                    else if (sum > 10) break;
                }
            }
        }
    }

    if (candi.empty()) {
        calScore();
        return;
    }
    
    // TODO: 턴 넘기는 것도 고려해야 됨
	shuffle(candi.begin(), candi.end(), g);
    if (candi.size() > numberOfChoices) candi.resize(numberOfChoices); // TODO: candi 섞기
    for (auto [i1, j1, i2, j2] : candi) {
        move(i1, i2, j1, j2);
        if (botMove()) {
            personMove();
            backtracking();
        }
        else {
            calScore();
        }
        backtracking();
    }
}

int main() {
    init();
    
    // while (true) {
    //     botMove();
    //     // print();
    // }
    
    personMove();
    cout << "best score: " << bestScore << "\n";
    
    return 0;
}

// // cout << moveHistory.size() << "\n";
// for (auto tmp : moveHistory) {
//     // cout << "---\n";
//     // for (auto [i, j, boardVal, ownerVal] : tmp) cout << i << " " << j << " " << boardVal << " " << ownerVal << "\n";
//     auto [i1, j1, _, __] = tmp.front();
//     auto [i2, j2, ___, ____] = tmp.back();
//     cout << i1 << " " << j1 << " " << i2 << " " << j2 << "\n";
// }
// cout << "xxx";