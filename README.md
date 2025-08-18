## [nypc 10주년 캠퍼스 리그](https://nypc10th-cb-event.xvive.link/) 브루트포스봇
[NYPC 아카이브](https://nypc.github.io/)

## 최종등수
| ![Ranking](/static/ranking.png) | ![Ranking2](/static/ranking2.png) |
|-|-|

## Preview
![Preview](https://github.com/user-attachments/assets/df69f392-87dd-4d7a-8064-0e273ea43fbe)

## Usage
### 1. 초기화
```bash
git clone https://github.com/simta1/nypc10th-cb-event.git
cd nypc10th-cb-event
./bin/init
```

### 2. data/board.txt 작성   
11*10 격자 숫자 적기(띄어쓰기 없이)   

ex)   
```
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
```

### 3. 실행   
```bash
make run
# 이후 마음에 드는 점수 도달 시 Ctrl+c로 종료

python3 -m http.server 5500 # 또는 vsc 사용 시 liveserver extension 실행
```

### 4. 결과확인
http://localhost:5500/ 안될경우 http://127.0.0.1:5500/ 확인   
`c`, `z`, `y` 사용   

직접 마우스로 드래그해서 게임하는 것도 가능 하지만 `c`랑 같이 사용할 경우 꼬임   
턴 넘기기는 `space`로 가능


## utils
### src/count.cpp
data/board.txt에서 숫자별 등장 횟수 출력   
```bash
make count
```

### bin/image_to_board.py
게임 화면에서 숫자판 캡처한 이미지 넣으면 자동으로 board.txt 생성   
설치하기 힘들면 그냥 직접 board.txt작성해도 상관X

```bash
# 설치 (arch linux)
sudo pacman -S --needed tk tesseract tesseract-data-eng
sudo pacman -S --needed wl-clipboard # wayland
sudo pacman -S --needed xclip # X11

python3 -m venv .venv
source .venv/bin/activate
pip install --upgrade pip
pip install -r requirements.txt

# 실행
make board
```

### bin/backup, bin/restore
`data/board.txt`, `data/solution.txt` 저장/복원

ex)
```bash
bin/backup # 현재 board.txt와 solution.txt 저장
bin/restore backup/backup_score101.md # score=101이었던 board.txt와 solution.txt 복원
```