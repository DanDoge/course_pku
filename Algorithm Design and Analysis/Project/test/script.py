import os
import random
import time

home_path = "C:\\Users\\AndrewHuang\\Documents\\GitHub\\course_pku\\Algorithm Design and Analysis\\Project";

home_2 = "C:\\spider\\tmp";

def gen_data():
    with open(home_path + "\\skimmed-sketch-test1", "w") as fp:
        print("hello!\n");
        fp.write(str(100000));
        fp.write('\n');
        for i in range(0, 100000):
            fp.write(str(random.randint(0, 999999)));
            fp.write('\n');

    with open(home_path + "\\skimmed-sketch-test2", "w") as fp:
        print("hello!\n");
        fp.write(str(100000));
        fp.write('\n');
        for i in range(0, 100000):
            fp.write(str(random.randint(0, 999999)));
            fp.write('\n');

def get_sketch_ans():
    print(home_2 + "\\skim_scketch.exe");
    os.system(home_2 + "\\skim_scketch.exe");
    with open(home_path + "\\skimmed_ans", 'r')as fp:
        res = fp.read();
    return res;

def get_exact_ans():
    os.system(home_2 + "\\verification.exe");
    with open(home_path + "\\exact_ans", 'r')as fp:
        res = fp.read();
    return res;

gen_data();

sketch_ans = get_sketch_ans();
exact_ans = get_exact_ans();

print(sketch_ans);
print(exact_ans);
