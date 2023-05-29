# -*- coding: utf-8 -*-
"""
Created on Fri Mar  8 14:04:33 2019

@author: TOMOCAT
"""

import pandas as pd
from pandas import DataFrame
import re
from pyecharts.charts import Line, Geo, Bar, Pie, Page, ThemeRiver
from snownlp import SnowNLP
import jieba
from jieba import analyse
import matplotlib.pyplot as plt
from wordcloud import WordCloud, STOPWORDS, ImageColorGenerator
# pip install jupyter-echarts-pypkg==0.1.1  # 解决pyecharts纵轴无法显示文本的问题


class MovieAnalysis():

    # 获取csv中影片信息
    def movie_info(self, movie):

        # 路径不能出现中文字符
        csv_file = movie + '.csv'
        # 注意 csv 默认不保存 utf-8, 需要自己用记事本打开更改编码为 utf-8, 否则会报错 UnicodeDecodeError
        dta = pd.read_csv(csv_file, encoding='utf-8')
        # 修改列名
        dta = dta.rename(columns={'电影名': 'movie_name', '用户昵称': 'user_name',
                         '评论内容': 'content', '评论日期': 'date', '点赞数': 'votes', '星级数': 'score'})
        # 去掉缺失值
        dta = dta[['score', 'date']].dropna()

        return dta

    # 电影评分时间走势图
    def draw_content_pic(self, movie):
        dta = self.movie_info(movie)

        # 初始化score_list数组存放由数据框转换后的列表
        score_list = []
        result = {}

        # 将df转化为列表
        # 列表格式形如[(50.0, '2018/3/4'),...,(40.0, '2018/2/19')]
        # dta.loc['index']:选取index对应的数据，返回Series;values获取Series的value
        for idx in dta.index:
            score_list.append(tuple(dta.loc[idx].values[:]))

        # print("有效评分总数量为：",len(score_list), " 条")
        for i in set(list(score_list)):
            result[i] = score_list.count(i)  # dict类型 ('很差', '2018-04-28'): 55

        info = []
        for key in result:
            score = key[0]
            date = key[1]
            val = result[key]
            info.append([score, date, val])
        # 将字典转换成为数据框
        info_new = DataFrame(info)
        info_new.columns = ['score', 'date', 'votes']
        info_new['date'] = pd.to_datetime(info_new['date'], format='%Y/%m/%d')
        # 按日期升序排列df，便于找最早date和最晚data，方便后面插值
        info_new.sort_values('date', inplace=True)

        # 排序完再把它转化为object类型
        info_new["date"] = info_new["date"].astype("str")
        # print("first df", info_new)

        # 以下代码用于插入空缺的数据，每个日期的评分类型应该有5种，依次遍历判断是否存在，若不存在则往新的 df 中插入新数值
        mark = 0
        create_df = pd.DataFrame(
            columns=['score', 'date', 'votes'])  # 创建空的dataframe
        for i in list(info_new['date']):
            location = info_new[(info_new.date == i) & (
                info_new.score == 50)].index.tolist()
            if location == []:
                create_df.loc[mark] = [50, i, 0]
                mark += 1
            location = info_new[(info_new.date == i) & (
                info_new.score == 40)].index.tolist()
            if location == []:
                create_df.loc[mark] = [40, i, 0]
                mark += 1
            location = info_new[(info_new.date == i) & (
                info_new.score == 30)].index.tolist()
            if location == []:
                create_df.loc[mark] = [30, i, 0]
                mark += 1
            location = info_new[(info_new.date == i) & (
                info_new.score == 20)].index.tolist()
            if location == []:
                create_df.loc[mark] = [20, i, 0]
                mark += 1
            location = info_new[(info_new.date == i) & (
                info_new.score == 10)].index.tolist()
            if location == []:
                create_df.loc[mark] = [10, i, 0]
                mark += 1
        # 创建了一个空的 create_df
        # info_new = info_new.append(
        #     create_df.drop_duplicates(), ignore_index=True)
        info_new = pd.concat(
            [create_df.drop_duplicates(), info_new], ignore_index=True)
        score_list = []
        # 按日期升序排列df，便于找最早date和最晚data，方便后面插值
        info_new.sort_values('date', inplace=True)
        print(info_new)
        # for index, row in info_new.iterrows():   # 第二种遍历df的方法
        # score_list.append([row['date'], row['votes'], row['score']])

        # 提取日期列表attr作为横坐标，v1、v2、v3、v4、v5作为各评分人数，绘制柱状图
        attr, v1, v2, v3, v4, v5 = [], [], [], [], [], []
        attr = list(sorted(set(info_new['date'])))
        for i in attr:
            v1.append(
                int(info_new[(info_new['date'] == i) & (info_new['score'] == 50)]['votes']))
            v2.append(
                int(info_new[(info_new['date'] == i) & (info_new['score'] == 40)]['votes']))
            v3.append(
                int(info_new[(info_new['date'] == i) & (info_new['score'] == 30)]['votes']))
            v4.append(
                int(info_new[(info_new['date'] == i) & (info_new['score'] == 20)]['votes']))
            v5.append(
                int(info_new[(info_new['date'] == i) & (info_new['score'] == 10)]['votes']))
        # pyecharts绘图
        # line = Line("影评走势图")
        line = Line()
        line.add("五星", attr, v1, is_stack=True)
        line.add("四星", attr, v2, is_stack=True)
        line.add("三星", attr, v3, is_stack=True)
        line.add("二星", attr, v4, is_stack=True)
        line.add("一星", attr, v5, is_stack=True, is_convert=False,
                 mark_line=["average"], is_more_utils=True)
        line.render("影评走势图.html")

    # 绘制情感分析曲线图
    def count_sentiment(csv_file):
        csv_file = csv_file + ".csv"
        # csv_file = csv_file.replace('\\', '\\\\')
        # 注意csv默认不保存utf-8，需要自己用记事本打开更改编码为utf-8
        d = pd.read_csv(csv_file, encoding='utf-8')
        motion_list = []  # 构造空列表，相当于向量
        for i in d['content']:
            try:
                # 文本过长时提取关键词后得分比较准确，而且SnowNLP这主要由于此SnowNLP主要用贝叶斯机器学习方法进行训练文本，机器学习在语料覆盖上不够，特征上工程处理不当会减分，也没考虑语义等
                s = round(
                    SnowNLP("".join(jieba.analyse.textrank(i))).sentiments, 2)
                motion_list.append(s)  # 将每一步的结果追加到motion向量中
            except:
                continue  # python异常处理
        result = {}
        for i in set(motion_list):
            # 将每一步追加到motion_list列表中，每个sentiment得分的计数
            result[i] = motion_list.count(i)
        return result

    def draw_sentiment_pic(self, csv_file1, csv_file2):
        attr1, val1 = [], []
        attr2, val2 = [], []
        info1 = count_sentiment(csv_file1)
        info2 = count_sentiment(csv_file2)
        # 字典的items()返回可遍历的(键, 值) 元组数组。
        # dict按照键值排序方法
        info1 = sorted(info1.items(), key=lambda x: x[0], reverse=False)
        # dict按照键值排序方法
        info2 = sorted(info2.items(), key=lambda x: x[0], reverse=False)
        for each in info1[:-1]:
            attr1.append(each[0])
            val1.append(each[1])
        for each in info2[:-1]:
            attr2.append(each[0])
            val2.append(each[1])
        line = Line("豆瓣影评情感分析图")
        # line.add("", attr, val, is_smooth=True, is_more_utils=True)
        line.add("红海行动", attr1, val1, is_fill=True,
                 line_opacity=0.2, area_opacity=0.4, symbol=None)
        line.add("我不是药神", attr2, val2, is_fill=True,
                 area_color='#000', area_opacity=0.3, is_smooth=True)
        line.show_config()
        line.render("豆瓣影评情感分析图.html")
    # 调用函数：draw_sentiment_pic('honghai','yaoshen')  #情感分析图

    # 过滤字符，只保留中文
    def translate(self, str):
        line = str.strip()  # 移除字符串头尾指定的字符（默认为空格或换行符）
        p2 = re.compile('[^\u4e00-\u9fa5]')  # 中文的编码范围是：\u4e00到\u9fa5
        # compile()函数单独使用就没有任何意义，需要和findall(), search(), match(）搭配使用。
        zh = " ".join(p2.split(line)).strip()  # 把标题符号或特殊字符转换为空格
        zh = ",".join(zh.split)
        str = re.sub("[A-Za-z0-9!！，%\[\],。]", "", zh)
        return str

    # 绘制词云图
    def word_cloud(self, csv_file, stopwords_path, pic_path):
        pic_name = csv_file+"_词云图.png"
        csv_file = csv_file + ".csv"
        d = pd.read_csv(csv_file, encoding='utf-8')
        # 注意csv默认不保存utf-8，需要自己用记事本打开更改编码为utf-8
        # d['content'].head()

        content = []
        for i in d['content']:
            try:
                i = self.translate(i)
            except AttributeError as e:
                continue
            else:
                content.append(i)
        comment_after_split = jieba.cut(str(content), cut_all=False)
        wl_space_split = " ".join(comment_after_split)
        backgroud_Image = plt.imread(pic_path)
        stopwords = STOPWORDS.copy()
        with open(stopwords_path, 'r', encoding='utf-8') as f:
            for i in f.readlines():
                stopwords.add(i.strip('\n'))
            f.close()

        wc = WordCloud(width=1024, height=768, background_color='white',
                       mask=backgroud_Image, font_path="C:\simhei.ttf",
                       stopwords=stopwords, max_font_size=400,
                       random_state=50)
        wc.generate_from_text(wl_space_split)
        img_colors = ImageColorGenerator(backgroud_Image)
        wc.recolor(color_func=img_colors)
        plt.imshow(wc)
        plt.axis('off')
        plt.show()
        wc.to_file(pic_name)


if __name__ == '__main__':
    movie_analyer = MovieAnalysis()
    movie_list = ['yaoshen', 'honghai']
    for movie in movie_list:
        movie_analyer.draw_content_pic(movie)
