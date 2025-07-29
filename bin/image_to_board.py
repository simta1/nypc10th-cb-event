import tkinter as tk
import cv2
import pytesseract
import os
import numpy as np
import sys

import subprocess
import platform

def get_image_from_clipboard():
    img_holder = {"img": None}

    def on_paste(event=None):
        print("Ctrl+V 눌림, 클립보드 이미지 가져오는 중...")
        img = None

        if platform.system() == "Linux":
            # Wayland 환경에서 wl-paste 사용
            temp_path = "/tmp/clipboard.png"
            try:
                with open(temp_path, 'wb') as f:
                    subprocess.run(["wl-paste", "--type", "image/png"], stdout=f, check=True)
                if os.path.exists(temp_path):
                    img = cv2.imread(temp_path)
            except Exception as e:
                print(f"Wayland에서 이미지 클립보드 실패: {e}")
        else:
            # Windows/macOS
            try:
                from PIL import ImageGrab, Image
                grabbed = ImageGrab.grabclipboard()
                if isinstance(grabbed, Image.Image):
                    img = cv2.cvtColor(np.array(grabbed), cv2.COLOR_RGB2BGR)
            except Exception as e:
                print(f"ImageGrab 실패: {e}")

        if img is not None:
            img_holder["img"] = img
            print("이미지 붙여넣기 완료")
            root.destroy()
        else:
            print("클립보드에 유효한 이미지가 없습니다.")

    root = tk.Tk()
    root.title("Ctrl+V로 이미지 붙여넣기")
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

def ask_user_for_number(cell_img, predicted, confidence):
    from PIL import Image, ImageTk
    import tkinter as tk

    result_holder = {"value": None}
    root = tk.Tk()
    root.title("숫자 확인")
    root.geometry("320x430")
    root.resizable(False, False)

    # label_text = f"OCR : {predicted} (신뢰도: {confidence}%)\n직접 입력 (1~9)"
    label_text = "OCR 실패\n직접 입력 (1~9)"

    label = tk.Label(root, text=label_text, font=("Arial", 12), justify='center')
    label.pack(pady=10)

    # 이미지 표시
    img_rgb = cv2.cvtColor(cell_img, cv2.COLOR_BGR2RGB)
    img_pil = Image.fromarray(img_rgb)
    img_pil = img_pil.resize((200, 200))
    img_tk = ImageTk.PhotoImage(img_pil)
    canvas = tk.Label(root, image=img_tk)
    canvas.image = img_tk
    canvas.pack()

    # 입력창
    entry = tk.Entry(root, font=("Arial", 20), justify='center')
    entry.pack(pady=20)
    entry.focus_set() # 자동 포커스

    def submit(event=None):
        val = entry.get().strip()
        if len(val) == 1 and val in "123456789":
            result_holder["value"] = val
            root.destroy()
        else:
            label.config(text="숫자 1~9 중 하나만 입력해주세요!")
            
    def clear_input(event=None):
        entry.delete(0, tk.END) # 입력창 비우기
        
    def force_quit(event=None):
        sys.exit(0)

    btn = tk.Button(root, text="입력", command=submit, font=("Arial", 14))
    btn.pack()

    root.bind('<Return>', submit)  # Enter 키 입력 처리
    root.bind('<Escape>', clear_input)
    root.bind('<Control-c>', force_quit)

    root.mainloop()
    return result_holder["value"]

# 숫자 OCR
def recognize_number(cell_img, threshold=70):
    gray = cv2.cvtColor(cell_img, cv2.COLOR_BGR2GRAY)
    _, threshed = cv2.threshold(gray, 100, 255, cv2.THRESH_BINARY_INV)
    config = '--oem 3 --psm 10 -c tessedit_char_whitelist=123456789'

    data = pytesseract.image_to_data(threshed, config=config, output_type=pytesseract.Output.DICT)

    text = ''
    confidence = -1
    for i in range(len(data['text'])):
        candidate = data['text'][i].strip()
        conf = int(data['conf'][i])
        if candidate and candidate.isdigit():
            text = candidate
            confidence = conf
            break

    if confidence < threshold or not text:
        predicted = text or "(없음)"
        user_input = ask_user_for_number(cell_img, predicted, confidence)
        return user_input or "0"
    return text

def main():
    try:
        img = get_image_from_clipboard()

        mask_path = 'data/mask.png'
        output_txt = 'data/board.txt'
        os.makedirs(os.path.dirname(output_txt), exist_ok=True)

        processed = preprocess(img, mask_path)
        cv2.namedWindow("OCR 진행중", cv2.WINDOW_NORMAL)
        cv2.resizeWindow("OCR 진행중", 800, 800)
        cv2.imshow("OCR 진행중", processed.copy())
        cv2.waitKey(1)

        grid = extract_grid(processed)
        with open(output_txt, 'w') as f:
            for i, row in enumerate(grid):
                numbers = []
                for j, cell in enumerate(row):
                    # OCR 중 현재 셀 하이라이트해서 보기
                    preview = processed.copy()
                    h, w = preview.shape[:2]
                    cell_h, cell_w = h // 11, w // 10
                    y1, y2 = i * cell_h, (i + 1) * cell_h
                    x1, x2 = j * cell_w, (j + 1) * cell_w

                    # 빨간 사각형으로 현재 위치 표시
                    cv2.rectangle(preview, (x1, y1), (x2, y2), (0, 0, 255), 2)
                    cv2.imshow("OCR 진행중", preview)
                    cv2.waitKey(1)

                    number = recognize_number(cell)
                    numbers.append(number)
                    print(f"{number} ", end="", flush=True)
                print()

                f.write(''.join(numbers) + '\n')

            cv2.destroyAllWindows()

    except Exception as e:
        print(f"에러 발생: {e}")


if __name__ == "__main__":
    main()