#include <bits/stdc++.h>
using namespace std;

const string INPUT_DIR = "data/board.txt";
const string OUTPUT_DIR = "data/count.txt";

constexpr int rows = 11, cols = 10;
int board[rows][cols];

void init() {
    ifstream fin(INPUT_DIR);
    if (!fin) {
        cerr << INPUT_DIR << "를 열 수 없습니다.\n";
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            char ch;
            if (!(fin >> ch) || ch < '1' || ch > '9') {
                cerr << "data/board.txt 형식 잘못됨\n";
                exit(1);
            }
            board[i][j] = ch & 15;
        }
    }
    
    fin.close();
}

int main() {
    init();
    
    int cnt[10] = {0};
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            ++cnt[board[i][j]];
        }
    }

    for (int i = 1; i <= 9; i++) cout << i << ": " << cnt[i] << "\n";

    ofstream fout(OUTPUT_DIR, ios::trunc);
    if (!fout) {
        cerr << OUTPUT_DIR << "를 열 수 없습니다.\n";
    }
    else {
        for (int i = 1; i <= 9; i++) fout << i << ": " << cnt[i] << "\n";
    }

    return 0;
}