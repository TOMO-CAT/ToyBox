import random
from enum import Enum
import string
import datetime
import time
import logging
import os
import getopt
import sys

logging.basicConfig(level=10)

DEFAULT_LOG_DIR = "./fake_log"


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


def gen_log_file(col_cnt, line_cnt=-1, dir=DEFAULT_LOG_DIR):
    if line_cnt <= 0:
        line_cnt = random.randint(500, 5000)

    col_idx2col_type = {}
    col_name_list = []
    for i in range(col_cnt):
        col_idx2col_type[i] = random.choice(data_type_list)
        col_name_list.append("col_"+str(i))

    file_path = os.path.join(dir, str(int(time.time() * 1000)) + ".log")
    logging.debug("start mock data into " + file_path)
    with open(file_path, "a") as f:
        f.write('\t'.join(col_name_list) + '\n')

        for i in range(line_cnt):
            data_list = []
            for col_idx in range(col_cnt):
                data_list.append(
                    str(gen_random_data(col_idx2col_type[col_idx])))
            f.write('\t'.join(data_list) + '\n')


def main(argv):
    fake_log_cnt = 1
    try:
        opts, args = getopt.getopt(argv, "hn:", ["number="])
    except getopt.GetoptError:
        print("gen_test_log.py -n <log file number>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == 'h':
            print("gen_test_log.py -n <log file number [1, 100]>")
            sys.exit()
        elif opt in ("-n", "--number"):
            fake_log_cnt = int(arg)

    if fake_log_cnt <= 0 or fake_log_cnt > 100:
        print("fake_log_cnt", fake_log_cnt)
        print("invalid value for log number, please set in range [1, 100]")
        sys.exit(2)

    if not os.path.exists(DEFAULT_LOG_DIR):
        os.makedirs(DEFAULT_LOG_DIR)

    fake_col_cnt = random.randint(10, 20)

    for i in range(fake_log_cnt):
        gen_log_file(col_cnt=fake_col_cnt)


if __name__ == "__main__":
    main(sys.argv[1:])
