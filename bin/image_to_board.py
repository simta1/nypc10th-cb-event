import tkinter as tk
from PIL import ImageGrab, Image
import cv2
import pytesseract
import numpy as np
import os

# 클립보드에서 이미지 받기
def get_image_from_clipboard():
    img_holder = {"img": None}

    def on_paste(event=None):
        img = ImageGrab.grabclipboard()
        if isinstance(img, Image.Image):
            img_holder["img"] = cv2.cvtColor(np.array(img), cv2.COLOR_RGB2BGR)
            print("✔ 이미지 붙여넣기 완료")
            root.destroy()
        else:
            print("❌ 클립보드에 이미지가 없습니다.")

    root = tk.Tk()
    root.title("📋 Ctrl+V로 이미지 붙여넣기")
    root.geometry("300x150")
    label = tk.Label(root, text="Ctrl+V로 이미지 붙여넣기", font=("Arial", 14))
    label.pack(expand=True)

    root.bind_all("<Control-v>", on_paste)
    root.mainloop()

    if img_holder["img"] is None:
        raise ValueError("클립보드에서 이미지를 가져오지 못했습니다.")
    return img_holder["img"]

# mask.png로 전처리
def preprocess(img, mask_path):
    h, w = img.shape[:2]
    mask = cv2.imread(mask_path, cv2.IMREAD_UNCHANGED)
    if mask is None or mask.shape[2] < 4:
        raise ValueError("마스크 파일이 없거나 알파 채널이 없습니다.")

    alpha = cv2.resize(mask[:, :, 3], (w, h), interpolation=cv2.INTER_NEAREST)
    _, binary = cv2.threshold(alpha, 1, 255, cv2.THRESH_BINARY_INV)
    binary = binary.astype(np.uint8)

    masked = cv2.bitwise_and(img, img, mask=binary)
    gray = cv2.cvtColor(masked, cv2.COLOR_BGR2GRAY)
    _, bright_mask = cv2.threshold(gray, 120, 255, cv2.THRESH_BINARY)

    bg_color = (0, 0, 0)
    result = np.full_like(img, bg_color, dtype=np.uint8)
    result[bright_mask == 255] = masked[bright_mask == 255]

    return result

# 격자 추출
def extract_grid(img, rows=11, cols=10):
    h, w = img.shape[:2]
    cell_h, cell_w = h // rows, w // cols
    return [
        [img[r * cell_h:(r + 1) * cell_h, c * cell_w:(c + 1) * cell_w] for c in range(cols)]
        for r in range(rows)
    ]

# 숫자 OCR
def recognize_number(cell_img):
    gray = cv2.cvtColor(cell_img, cv2.COLOR_BGR2GRAY)
    _, threshed = cv2.threshold(gray, 100, 255, cv2.THRESH_BINARY_INV)
    config = '--oem 3 --psm 10 outputbase digits'
    text = pytesseract.image_to_string(threshed, config=config)
    return text.strip() or "0"

def main():
    try:
        img = get_image_from_clipboard()

        mask_path = 'data/mask.png'
        output_txt = 'data/board.txt'
        os.makedirs(os.path.dirname(output_txt), exist_ok=True)

        processed = preprocess(img, mask_path)

        grid = extract_grid(processed)
        with open(output_txt, 'w') as f:
            for i, row in enumerate(grid):
                numbers = []
                for j, cell in enumerate(row):
                    number = recognize_number(cell)
                    numbers.append(number)
                    print(f"OCR: ({i}, {j}) = {number}")

                    # OCR 중 현재 셀 하이라이트해서 보기
                    preview = processed.copy()
                    h, w = preview.shape[:2]
                    cell_h, cell_w = h // 11, w // 10
                    y1, y2 = i * cell_h, (i + 1) * cell_h
                    x1, x2 = j * cell_w, (j + 1) * cell_w

                    # 빨간 사각형으로 현재 위치 표시
                    cv2.rectangle(preview, (x1, y1), (x2, y2), (0, 0, 255), 2)
                    cv2.imshow("OCR 진행중", preview)
                    cv2.waitKey(1)  # 100ms 대기 (또는 0으로 하면 키 입력 기다림)

                f.write(''.join(numbers) + '\n')

            cv2.destroyAllWindows()

    except Exception as e:
        print(f"에러 발생: {e}")


if __name__ == "__main__":
    main()