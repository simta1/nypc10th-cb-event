## [nypc 10주년 캠퍼스 리그](https://nypc10th-cb-event.xvive.link/) 브루트포스봇

![Image](https://github.com/user-attachments/assets/f8feb7d4-51d5-4d77-bcb1-ad52843ac8d4)

## Usage

### 1. data/board.txt 작성   
11*10로 숫자 적기(띄어쓰기 없이)   

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

### 2. 실행   
```bash
make run
```


## utils
### src/count.cpp
data/board.txt에서 숫자별 등장 횟수 세는 용
```bash
make count
```

### bin/image_to_board.py
게임 화면에서 숫자판 캡처한 이미지 넣으면 자동으로 board.txt 생성   
설치하기 힘들면 그냥 직접 board.txt작성해도 문제는 없음

#### 1. 설치 (arch linux)
```
sudo pacman -S tk tesseract tesseract-data-eng wl-clipboard

python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

#### 2. 실행
```
make board
```

---

## 핑크빈 전략
영역 드래그 한 뒤 {핑크빈점수-내점수}가 최대가 되는 영역 \[i1:i2\]\[j1:j2\] 중에서   
    i1이 가장 작고, 같은 게 여럿이면   
    그 중 j1이 가장 작고, 같은 게 여럿이면   
    그 중 i2가 가장 긴 걸 선택함.   
    -> 그냥 직사각형의 점들 (x,y)쌍 정렬한게 사전순최소인거 고르는 거임   
