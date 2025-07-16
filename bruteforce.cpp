#include <bits/stdc++.h>
using namespace std;

constexpr int numberOfChoices = 3;
constexpr int SCORE_THRESHOLD = 40;

constexpr int rows = 11, cols = 10;
constexpr int INF = 1e5;

int board[rows][cols], owner[rows][cols];

int player = 1;

vector<vector<tuple<int, int, int, int> > > moveHistory;

// string input = R"(
// 3439574859
// 9856647774
// 7977889363
// 1727317271
// 9619428888
// 6615551959
// 9678533872
// 4347914671
// 4131525697
// 4444749359
// 2145429719
// )";

string input = R"(
2876285638
9154931916
1233346925
3216422421
5515543293
2749166953
2522921129
3273759573
9565865555
8461542573
3857217479
)";


void init() {
    istringstream iss(input);
    for (int i = 0; i < rows; i++) {
        string st;
        iss >> st;
        for (int j = 0; j < cols; j++) board[i][j] = st[j] & 15;
    }
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

int getScore() {
    int score = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (owner[i][j] == 1) ++score;
            else if (owner[i][j] == 2) --score;
        }
    }
    
    ++currentRound;
    if (score >= SCORE_THRESHOLD) printHistoryWithScore(score);
    return score;
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


bool botMove() {
    int mx = 0;
    int i1, i2, j1, j2;
    
    for (int startRow = 0; startRow < rows; startRow++) {
        int rowPrefSum[cols] = {0}; // rowPerfSum[col] := sum(board[startRow][startCol:col]) // 직사각형에 합이 0인 변 존재하는지 확인용
        rowPrefSum[0] = board[startRow][0];
        for (int col = 1; col < cols; col++) rowPrefSum[col] = rowPrefSum[col - 1] + board[startRow][col];

        for (int startCol = 0; startCol < cols; startCol++) {
            int colPrefCnt[cols][3] = {0};
            int colPrefSum[cols] = {0}; // colPrefSum[col] := sum(board[startRow:row][col])

            int prevRowLastCol = cols;
            for (int row = startRow; row < rows; row++) {
                int cnt[3] = {0};
                int sum = 0;

                int curRowLastCol = -1;
                for (int col = startCol; col < cols && col < prevRowLastCol; col++) {
                    curRowLastCol = col;
                    ++colPrefCnt[col][owner[row][col]];
                    cnt[0] += colPrefCnt[col][0];
                    cnt[1] += colPrefCnt[col][1];
                    cnt[2] += colPrefCnt[col][2];

                    colPrefSum[col] += board[row][col];
                    sum += colPrefSum[col];
                    
                    if (sum >= 10) {
                        int firstRowSum = rowPrefSum[col] - (startCol ? rowPrefSum[startCol - 1] : 0);
                        if (sum == 10 && colPrefSum[startCol] && firstRowSum) {
                            int dscore = cnt[0] + 2 * cnt[3 - player];
                            // cout << startRow << " " << startCol << " " << row << " " << col << " : " << dscore << "\n";

                            if (mx < dscore) {
                                mx = dscore;
                                i1 = startRow;
                                j1 = startCol;
                                i2 = row;
                                j2 = col;
                            }
                            // else if (mx == dscore) {
                            //     if (startRow == i1 && startCol == j1) {
                            //         // cout << moveHistory.size() << "\n";
                            //         for (auto tmp : moveHistory) {
                            //             // cout << "---\n";
                            //             // for (auto [i, j, boardVal, ownerVal] : tmp) cout << i << " " << j << " " << boardVal << " " << ownerVal << "\n";
                            //             auto [i1, j1, _, __] = tmp.front();
                            //             auto [i2, j2, ___, ____] = tmp.back();
                            //             cout << i1 << " " << j1 << " " << i2 << " " << j2 << "\n";
                            //         }
                            //         cout << "xxx";
                            //         exit(0);
                            //     }
                            // }
                        }
                        
                        break;
                    }
                }
                prevRowLastCol = curRowLastCol;
                if (curRowLastCol == startCol) break;
            }
        }
    }

    if (!mx) return false;

    move(i1, i2, j1, j2);
    return true;
}

int personMove() {
    vector<tuple<int, int, int, int> > candi;
    
    for (int startRow = 0; startRow < rows; startRow++) {
        int rowPrefSum[cols] = {0}; // rowPerfSum[col] := sum(board[startRow][startCol:col]) // 직사각형에 합이 0인 변 존재하는지 확인용
        rowPrefSum[0] = board[startRow][0];
        for (int col = 1; col < cols; col++) rowPrefSum[col] = rowPrefSum[col - 1] + board[startRow][col];

        for (int startCol = 0; startCol < cols; startCol++) {
            int colPrefCnt[cols][3] = {0};
            int colPrefSum[cols] = {0}; // colPrefSum[col] := sum(board[startRow:row][col])

            int prevRowLastCol = cols;
            for (int row = startRow; row < rows; row++) {
                int cnt[3] = {0};
                int sum = 0;

                int curRowLastCol = -1;
                for (int col = startCol; col < cols && col < prevRowLastCol; col++) {
                    curRowLastCol = col;
                    ++colPrefCnt[col][owner[row][col]];
                    cnt[0] += colPrefCnt[col][0];
                    cnt[1] += colPrefCnt[col][1];
                    cnt[2] += colPrefCnt[col][2];

                    colPrefSum[col] += board[row][col];
                    sum += colPrefSum[col];
                    
                    if (sum >= 10) {
                        int firstRowSum = rowPrefSum[col] - (startCol ? rowPrefSum[startCol - 1] : 0);
                        if (sum == 10 && colPrefSum[startCol] && firstRowSum) {
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
                }
                prevRowLastCol = curRowLastCol;
                if (curRowLastCol == startCol) break;
            }
        }
    }

    if (candi.empty()) return getScore();
    
    int res = -INF;
    // TODO: 턴 넘기는 것도 고려해야 됨
    
    if (candi.size() > numberOfChoices) candi.resize(numberOfChoices); // TODO: candi 섞기
    for (auto [i1, j1, i2, j2] : candi) {
        move(i1, i2, j1, j2);
        if (botMove()) {
            res = max(res, personMove());
            backtracking();
        }
        else {
            res = max(res, getScore());
        }
        backtracking();
    }
    return res;
}

int main() {
    init();
    
    // while (true) {
    //     botMove();
    //     // print();
    // }
    

    int res = personMove();
    cout << res << "\n";
    
    return 0;
}