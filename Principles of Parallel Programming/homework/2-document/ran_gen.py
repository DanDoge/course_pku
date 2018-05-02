import os
import random

home_path = "C:\\Users\\AndrewHuang\\Documents\\GitHub\\course_pku\\Principles of Parallel Programming\\homework\\2-document";

with open(home_path + "\\homework2-input", "w") as fp:
    print("hello!\n");
    fp.write(str(100000));
    fp.write('\n');
    for i in range(0, 100000):
        fp.write(str(random.uniform(0, 100000000)));
        fp.write('\n');
