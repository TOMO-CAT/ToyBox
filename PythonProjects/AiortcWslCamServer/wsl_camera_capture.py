import cv2

address = "udp://127.0.0.1:5000"

cap = cv2.VideoCapture(address)

if not cap.isOpened():
    print("cannot open camera!")
    cap.release()
    exit()

while True:
    ret, frame = cap.read()
    if ret:
        print(f'receive camera frame with [{frame.nbytes}] bytes')
        cv2.imshow("frame", frame)
    else:
        print('wait for frame...')
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
