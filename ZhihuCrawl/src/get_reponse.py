# -*- coding: utf-8 -*-
from fake_useragent import UserAgent
import requests
import json

if __name__ == '__main__':
    target_url = "https://www.zhihu.com/api/v4/questions/37143847/answers?include=data%5B*%5D.is_normal%2Cadmin_closed_comment%2Creward_info%2Cis_collapsed%2Cannotation_action%2Cannotation_detail%2Ccollapse_reason%2Cis_sticky%2Ccollapsed_by%2Csuggest_edit%2Ccomment_count%2Ccan_comment%2Ccontent%2Ceditable_content%2Cvoteup_count%2Creshipment_settings%2Ccomment_permission%2Ccreated_time%2Cupdated_time%2Creview_info%2Crelevant_info%2Cquestion%2Cexcerpt%2Crelationship.is_authorized%2Cis_author%2Cvoting%2Cis_thanked%2Cis_nothelp%2Cis_labeled%2Cis_recognized%2Cpaid_info%2Cpaid_info_content%3Bdata%5B*%5D.mark_infos%5B*%5D.url%3Bdata%5B*%5D.author.follower_count%2Cbadge%5B*%5D.topics&offset=3&limit=5&sort_by=default&platform=desktop"
    headers = {
        'User-Agent': UserAgent().random,
    }
    response = requests.get(url = target_url, headers = headers)
    html = response.text 
    # print(html)
    # print(json.loads(html))
    with open('html_example.txt', 'w', encoding="utf-8") as f: # set the encoding option to avoid unintelligible text
        f.write(html)