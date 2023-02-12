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
        'cookie': '_zap=d62a474a-43dc-450a-bdcc-cb807357f4ab; _xsrf=3999fea2-0356-42e7-8d2a-ac968d594c86; d_c0="AEDkV8GlCw-PTjUrBeB6hDjqwntD_bakBL8=|1551259687";q_c1=7d117a8c0f564a3ea2a20d02f83e3bb1|1551259688000|1551259688000; tgw_l7_route=66cb16bc7f45da64562a077714739c11'
    }
    res = requests.get(url, headers=headers).text
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