import os
import random

home_path = "C:\\Users\\AndrewHuang\\Documents\\GitHub\\course_pku\\Algorithm Design and Analysis\\Project";

with open(home_path + "\\skimmed-sketch-test1", "w") as fp:
    print("hello!\n");
    fp.write(str(100000));
    fp.write('\n');
    for i in range(0, 100000):
        fp.write(str(random.randint(0, 999)));
        fp.write('\n');

with open(home_path + "\\skimmed-sketch-test2", "w") as fp:
    print("hello!\n");
    fp.write(str(100000));
    fp.write('\n');
    for i in range(0, 100000):
        fp.write(str(random.randint(0, 999)));
        fp.write('\n');
