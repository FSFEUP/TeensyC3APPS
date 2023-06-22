with open("APPS_calib.txt", "r") as f:
    uniqueLines = sorted(set(f.readlines()))
    with open("APPS_calib_unique.txt", "w") as f2:
        f2.writelines(uniqueLines)
