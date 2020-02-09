# -*- coding: utf-8 -*-
from fake_useragent import UserAgent
import requests
import argparse

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


if __name__ == '__main__':
    # 1) get the zhihu answer identifier
    number = parse_input()
    print("Info::zhihu answer identifier::" + number)

    # 2) fake http request
    target_url = 
    