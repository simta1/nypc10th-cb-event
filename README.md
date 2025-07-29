## [nypc 10주년 캠퍼스 리그](https://nypc10th-cb-event.xvive.link/) 브루트포스봇

1. data/board.txt 작성   

아래처럼 11*10으로 숫자 적으면 됨(띄어쓰기 없어야 함)   

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

2. 실행   
```
make run
```


## utils
### src/count.cpp
```
make count
```
data/board.txt에서 숫자별 등장 횟수 출력해줌

### bin/image_to_board.py
게임 사이트에서 숫자판 부분 캡쳐한 사진으로부터 숫자를 추출해 자동으로 board.txt를 만들어줌

1. Tesseract OCR 설치

windows는 https://github.com/UB-Mannheim/tesseract/wiki 에서 설치
환경변수 등록 필요

2. python 라이브러리 설치
```
pip install -r requirements.txt
```

3. 실행
```
python bin/image_to_board.py
```
<!-- TODO makefile -->




--
핑크빈 전략: 영역 드래그 한 뒤 {핑크빈점수-내점수}가 최대가 되는 영역 \[i1:i2\]\[j1:j2\] 중에서   
    i1이 가장 작고, 같은 게 여럿이면   
    그 중 j1이 가장 작고, 같은 게 여럿이면   
    그 중 i2가 가장 긴 걸 선택함.   
    -> 그냥 직사각형의 점들 (x,y)쌍 정렬한게 사전순최소인거 고르는 거임   
