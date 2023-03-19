import time
import os
import random
import datetime
import string
from enum import Enum

LOG_DIR = "./test_log"
MAX_LINE_COUNT = 100


class data_type(Enum):
    INT = 1
    FLOAT = 2
    STRING = 3
    PHONE = 4
    DATE = 5
    TIMESTAMP = 6


data_type_list = list(data_type)


def gen_random_data(typ=data_type.INT):
    if typ == data_type.INT:
        return random.randint(-1000000, 1000000)
    elif typ == data_type.FLOAT:
        return random.uniform(-2000, 2000)
    elif typ == data_type.STRING:
        random_str = ''
        base_str = 'ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyz0123456789'
        random_length = random.randint(3, 20)
        for i in range(random_length):
            random_str += base_str[random.randint(0, len(base_str) - 1)]
        return random_str
    elif typ == data_type.PHONE:
        mobiles = ['130', '131', '132', '134']
        number = ''.join(random.sample(string.digits, 8))
        return random.choice(mobiles)+number
    elif typ == data_type.DATE:
        return datetime.date.today() - datetime.timedelta(random.randint(0, 29200))
    elif typ == data_type.TIMESTAMP:
        return int(time.time()) - random.randint(0, 60 * 60 * 24 * 365)


def gen_log_file():
    col_cnt = len(data_type_list)

    col_name_list = []
    for i in range(col_cnt):
        col_name_list.append(str(data_type_list[i]).lstrip("data_type."))

    if not os.path.exists(LOG_DIR):
        os.makedirs(LOG_DIR)

    file_path = os.path.join(LOG_DIR, str(int(time.time() * 1000)) + ".log")
    with open(file_path, "a") as f:
        f.write('\t'.join(col_name_list) + '\n')
        for i in range(random.randint(MAX_LINE_COUNT/2, MAX_LINE_COUNT)):
            data_list = []
            for j in range(col_cnt):
                data_list.append(str(gen_random_data(data_type_list[j])))
            f.write('\t'.join(data_list) + '\n')


def main():
    gen_log_file()


if __name__ == "__main__":
    main()
