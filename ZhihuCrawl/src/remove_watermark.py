# -*- coding:utf-8 -*-
 
import cv2
import numpy as np
import os

def img_clean(img):
    height, width = img.shape[0:2]

    # show the original picture
    cv2.namedWindow("Image", 0)
    cv2.resizeWindow("Image", int(width / 2), int(height / 2))
    cv2.imshow('Image', img)
 
    rects = ((width - 170, height - 38, width, height),(1, 1, 164, 100)) #水印区域
    mask = np.zeros((height, width), np.uint8)
    for rect in rects:
        x1, y1, x2, y2 = rect
        cv2.rectangle(mask, (x1, y1), (x2, y2), (255, 255, 255), -1)
        img = cv2.inpaint(img, mask, 1.5, cv2.INPAINT_TELEA) #蒙版

    # show the new picture
    cv2.namedWindow("newImage", 0)
    cv2.resizeWindow("newImage", int(width / 2), int(height / 2))
    cv2.imshow('newImage', img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

def crop_watermark():
    img_path_list = os.listdir('./pictures')
    total = len(img_path_list)
    cnt = 1
    for img_path in img_path_list:
        print("Debug::the overall process::{}/{}".format(cnt, total))
        img_abs_path = './pictures/' + img_path
        # print(img_abs_path)
        img = cv2.imread(img_abs_path)
        height, width = img.shape[0:2]
        cropped = img[0:height-40, 0:width]
        adjusted_img_abs_path = './adjusted/' + img_path
        cv2.imwrite(adjusted_img_abs_path, cropped)
        cnt += 1

if __name__ == '__main__':
    # 法一：修复水印
    # f_img = 'shui1.jpg'
    # img = cv2.imread(f_img)
    # img_clean(img)

    # 法二：裁剪图片
    img_folder = 'adjusted'
    if not os.path.exists(img_folder):
        os.makedirs(img_folder)

    crop_watermark()