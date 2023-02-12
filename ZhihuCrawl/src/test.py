# -*- coding: utf-8 -*-

import requests
import time
import datetime
import os
import json
import uuid
from pyquery import PyQuery as pq
from fake_useragent import UserAgent

# 地址 https://www.zhihu.com/question/34243513
def start(offset, sort):
    # url = 'https://www.zhihu.com/api/v4/questions/279938742/answers?include=data%5B%2A%5D.is_normal%2Cadmin_closed_comment%2Creward_info%2Cis_collapsed%2Cannotation_action%2Cannotation_detail%2Ccollapse_reason%2Cis_sticky%2Ccollapsed_by%2Csuggest_edit%2Ccomment_count%2Ccan_comment%2Ccontent%2Ceditable_content%2Cvoteup_count%2Creshipment_settings%2Ccomment_permission%2Ccreated_time%2Cupdated_time%2Creview_info%2Crelevant_info%2Cquestion%2Cexcerpt%2Crelationship.is_authorized%2Cis_author%2Cvoting%2Cis_thanked%2Cis_nothelp%2Cis_labeled%3Bdata%5B%2A%5D.mark_infos%5B%2A%5D.url%3Bdata%5B%2A%5D.author.follower_count%2Cbadge%5B%2A%5D.topics&limit=5&offset=' + str(
    #     offset) + '&platform=desktop&sort_by=' + sort
    url = "https://www.zhihu.com/api/v4/questions/{number}/feeds?include=data%5B*%5D.is_normal%2Cadmin_closed_comment%2Creward_info%2Cis_collapsed%2Cannotation_action%2Cannotation_detail%2Ccollapse_reason%2Cis_sticky%2Ccollapsed_by%2Csuggest_edit%2Ccomment_count%2Ccan_comment%2Ccontent%2Ceditable_content%2Cattachment%2Cvoteup_count%2Creshipment_settings%2Ccomment_permission%2Ccreated_time%2Cupdated_time%2Creview_info%2Crelevant_info%2Cquestion%2Cexcerpt%2Cis_labeled%2Cpaid_info%2Cpaid_info_content%2Creaction_instruction%2Crelationship.is_authorized%2Cis_author%2Cvoting%2Cis_thanked%2Cis_nothelp%2Cis_recognized%3Bdata%5B*%5D.mark_infos%5B*%5D.url%3Bdata%5B*%5D.author.follower_count%2Cvip_info%2Cbadge%5B*%5D.topics%3Bdata%5B*%5D.settings.table_of_content.enabled&offset={offset}&limit=3&order=default&platform=desktop".format(
        number = 279938742, offset = offset
    )
    headers = {
        'user-agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.119 Safari/537.36',
        'cookie': 'SESSIONID=CqKqfamPsUdbBGSAqIkEpA9cqoclgWLmktQD1aH45fz; JOID=UFsSCkrVenieTzsqJd8YL4GyZgg1oz9F_w1XSmmsAg3KIXZTWTOwlvZNPiAj1LH1us3zSwlGsCzYEyDa9KDJRG8=; osd=W1oWBkzee3ySSTArIdMeJIC2ag4-ojtJ-QZWTmWqCQzOLXBYWDe8kP1MOiwl37Dxtsv4Sg1KtifZFyzc_6HNSGk=; _zap=3207d511-600a-4c14-b94e-34426f95ad88; d_c0="ABBfz6Xh6BSPTsTy8-pqNTd_kuV9deLYQlw=|1652005792"; _9755xjdesxxd_=32; YD00517437729195:WM_TID=41PhhmbXMDlAQBFUVAfVEdgLXy6uPa6D; q_c1=20ac94cb7535475b9565a84df96feaef|1652022174000|1652022174000; __snaker__id=x1dJWavYgAVheDaq; q_c1=ae64652980564ad9bb3b92ee316c0d19|1667114270000|1662642430000; YD00517437729195:WM_NI=/uiXPEhiuecKYwpd1Baf27jdbkFYls2p42C7sENhB9qgDgf28WOC5XYULfQEhYCO18Igigwnm6lWoU7bBzS5dSiN3TySQ+ZSkQ7ONM3osG7AEZroyom/IRX7c/uZ51dkbGU=; YD00517437729195:WM_NIKE=9ca17ae2e6ffcda170e2e6eeaee7708f89f9b2ec698deb8ab2d84f828b9ab1d4539c8ffcd2d453af9fa58ece2af0fea7c3b92aedaba7b9d34facb7bb86e2419ba9c085b44aa3bafed8e970a2b58abaef79f2b0fcd3c23db19b8289c133b3b98c88e53afc8981a4b8549b8b8487d44fac878db0b4348bb9a788f666afeaa4b8ee5ff5be8e9bd23fa88da7afb260bb8d9fccd959a98ff9b0cd47f1b1bda8b64b8786be90fc65aff187b3db3da1f1bbbbb83ba8b5aea8d837e2a3; _xsrf=7e932b44-b6fa-49aa-8322-28483e136a22; z_c0=2|1:0|10:1676039331|4:z_c0|80:MS4xaUFja0FBQUFBQUFtQUFBQVlBSlZUYU9pMDJScHltMGxiT29ETmlESWZleHV2VV81QWxTUlBnPT0=|57b286994071c1d75dac77e854659be679f1f6dae270b6fbb3219ab53ee86759; Hm_lvt_98beee57fd2ef70ccdd5ca52b9740c49=1674384260,1674384771,1675783530,1676119600; arialoadData=false; Hm_lpvt_98beee57fd2ef70ccdd5ca52b9740c49=1676183873; SESSIONID=wA3iAmdBzyqZfNxxwrNQmrorvPzUEApKtbVlde7EYCe; JOID=UF4SAE7MTxkxrOvTVcwsTiFVvPBFvw8uXuqCthOxMmlvzKKvLPb381Ov7tZXcYEAbbyK5tciChKuHuwrpijfVZ0=; osd=WlkTB0vGSBg2qeHUVMspRCZUu_VPuA4pW-CFtxS0OG5uy6elK_fw9lmo79FSe4YBarmA4dYlDxipH-surC_eUpg=; tst=h; KLBRSID=b33d76655747159914ef8c32323d16fd|1676198115|1676197107'
    }
    res = requests.get(url, headers=headers).text
    print(res)
    data = json.loads(res)
    picRepo = 'picRepo'
    if not os.path.exists(picRepo):
        os.makedirs(picRepo)
    if data.get("data"):
        for i, item in enumerate(data['data']):
            content = pq(item['content'])
            imgUrls = content.find('noscript img').items()
            for imgTag in imgUrls:
                src = imgTag.attr("src")
                strIndex = src.rfind('.')
                suffix = src[strIndex:]
                with open(f"{picRepo}/{uuid.uuid4()}{suffix}", 'wb') as f:
                    f.write(requests.get(src).content)


if __name__ == '__main__':
    starttime = datetime.datetime.now()
    strTime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    stime = print(f'开始抓取,当前时间: {strTime}')
    for i in range(3):  # 偏移量单位是5，循环3次
        start(offset=((i * 5) if i != 0 else 0), sort='updated')  # updated:按时间降序，default：默认排序
        # 这里先睡一会，如果太快可能有些图片下载后会查看不了，
        # 越慢，下载的图片可以查看的越多。原因大概是知乎的反爬虫机制，
        # 看不了的图片其实返回的是一个400的badRequest的状态码
        time.sleep(3)
    endtime = datetime.datetime.now()
    print(f'抓取完毕,用时 {((endtime - starttime).seconds)} 秒:')