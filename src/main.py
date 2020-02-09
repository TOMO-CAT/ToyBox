# -*- coding: utf-8 -*-
import os
from fake_useragent import UserAgent
import requests
import argparse
import json
from pyquery import PyQuery as pq
import uuid
import time
import cv2

def parse_input():
    number = ""
    parser = argparse.ArgumentParser()
    parser.description = "Input the answer identifier"
    parser.add_argument("-n",
                        "--number",
                        action="store",
                        help="answer identifier",
                        dest="number",
                        nargs='+', 
                        type=str)
    args = parser.parse_args()
    if args.number:
        number = args.number
        print("Info::receive argument::" + number[0])
    else:
        print("Error::do not receive any argument, please try again")
        exit(1)
    return number[0]

def get_http_content(number, offset):
    target_url = "https://www.zhihu.com/api/v4/questions/{number}/answers?include=data%5B*%5D.is_normal%2Cadmin_closed_comment%2Creward_info%2Cis_collapsed%2Cannotation_action%2Cannotation_detail%2Ccollapse_reason%2Cis_sticky%2Ccollapsed_by%2Csuggest_edit%2Ccomment_count%2Ccan_comment%2Ccontent%2Ceditable_content%2Cvoteup_count%2Creshipment_settings%2Ccomment_permission%2Ccreated_time%2Cupdated_time%2Creview_info%2Crelevant_info%2Cquestion%2Cexcerpt%2Crelationship.is_authorized%2Cis_author%2Cvoting%2Cis_thanked%2Cis_nothelp%2Cis_labeled%2Cis_recognized%2Cpaid_info%2Cpaid_info_content%3Bdata%5B*%5D.mark_infos%5B*%5D.url%3Bdata%5B*%5D.author.follower_count%2Cbadge%5B*%5D.topics&offset={offset}&limit=5&sort_by=default&platform=deskto".format(
        number = number, offset = offset)
    # print("Debug::target url::" + target_url)
    headers = {
        'User-Agent': UserAgent().random,
    }
    response = requests.get(target_url, headers = headers)
    if (response == None) | (response.status_code != 200):
        print("Warn::http response is None, number=[{}], offset=[{}], status_code=[{}]".format(
            number, offset, response.status_code))
        return None
    html = response.text
    return json.loads(html)

def crop_watermark(img_path):
    img = cv2.imread(img_path)
    height, width = img.shape[0:2]
    print("Debug::height:[{}], width:[{}]".format(height, width))
    cropped = img[0:height-38, 0:width]
    cv2.imwrite("./adjusted/test.jpg", cropped)

if __name__ == '__main__':
    # python ./src/main.py -n 279938742
    # 0) set constants
    picture_path = 'pictures'
    if not os.path.exists(picture_path):
        os.makedirs(picture_path)
    max_offset = 10

    # 1) get the zhihu answer identifier
    number = parse_input()
    print("Info::zhihu answer identifier::" + number)

    # 2) fake http request and download pictures
    offset = 0
    while True:
        print("Info::now download zhihu pictures with offset {}". format(offset))
        content_dict = get_http_content(number, offset)
        if content_dict == None:
            print("Warn::content_dict is None, number={}, offset={}".format(
                number, offset))
            continue
        answer_texts = content_dict['data']
        for answer_text in answer_texts:
            answer_content = pq(answer_text['content'])
            # print(type(answer_content))
            img_urls = answer_content.find('noscript img').items()
            # print(img_urls)
            for img_url in img_urls:
                src = img_url.attr("src")
                strIndex = src.rfind('.')
                suffix = src[strIndex:]
                with open(f"{picture_path}/{uuid.uuid4()}{suffix}", 'wb') as f:
                    f.write(requests.get(src).content)
                time.sleep(2)
        offset += 1
        if offset > max_offset:
            break

    # 3) remove watermark of the download pictures
    # do in remove_watermark.py