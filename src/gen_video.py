import os
import cv2

# 此方法需保证每张图片size和视频size一致，对于不规则的图片集需要resize，由于会损失图片信息故暂时不使用该方法

if __name__ == '__main__':
    fps = 10
    size = (1280, 720)
    videowriter = cv2.VideoWriter("test.mp4", cv2.VideoWriter_fourcc(*"mp4v"), fps, size)
    img_path_list = os.listdir('./adjusted/')
    for img_path in img_path_list:
        img_abs_path = './adjusted/' + img_path
        img = cv2.imread(img_abs_path)
        videowriter.write(img)