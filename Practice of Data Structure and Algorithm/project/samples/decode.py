with open("frb30-15-1.clq", "r") as fin:
    with open("frb30.txt", "w") as fout:
        while True:
            line = fin.readline()
            if not line:
                break;
            arr = line.split(" ")
            for i in arr:
                try:
                    d = int(i)
                    fout.write("%d " % d) 
                except ValueError:
                    a = 1 
            fout.write("\n")
