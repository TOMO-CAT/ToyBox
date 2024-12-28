import cv2
import logging

logger = logging.getLogger("camera_capture")

class WslCameraCapture:
    def __init__(self, address: str):
        """
        初始化 WslCameraCapture 实例
        :param address: 相机的地址（例如：'udp://127.0.0.1:5000'）
        """
        self.__address = address
        self.__cap = None
    
    def open_camera(self):
        """打开相机"""
        self.__cap = cv2.VideoCapture(self.__address)
        if not self.__cap.isOpened():
            logger.error("cannot open the camera!")
            self.__cap.release()
            return False
        return True

    def capture_frame(self):
        ret, frame = self.__cap.read()
        if ret:
            logger.info(f"Received camera frame with [{frame.nbytes}] bytes")
            return frame
        else:
            logger.warn("waiting for a frame...")
            return None

    def release(self):
        """释放相机和窗口资源"""
        if self.__cap:
            self.__cap.release()

# python3 wsl_camera_capture.py
if __name__ == "__main__":
    address = "udp://127.0.0.1:5000"
    camera_capture = WslCameraCapture(address)
    if not camera_capture.open_camera():
        exit(1)

    while True:
        frame = camera_capture.capture_frame()
        cv2.imshow("frame", frame)
        # 按 'q' 键退出
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    camera_capture.release()
    cv2.destroyAllWindows()
