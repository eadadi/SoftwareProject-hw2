def file_comp(path1, path2):
    file1 = open(path1, 'r')
    file2 = open(path2, 'r')

    lst1 = []
    for line1 in file1:
        lst1.append(line1)

    lst2 = []
    for line2 in file2:
        lst2.append(line2)

    if len(lst1) != len(lst2):
        print("LEN ERROR")
        print(lst1)
        print(lst2)

    for i in range(len(line1)):
        if line1[i] != line2[i]:
            print("VALUES ERROR IN LINE: " + str(i))
            print(line1)
            print(line2)

    return True
    file1.close()
    file2.close()


x1 = "tests\output_1.txt"
y1 = "outputs\out1.txt"
if not(file_comp(x1, y1)):
    print("Error 1" + "\n")

x2 = "tests\output_2.txt"
y2 = "outputs\out2.txt"
if not(file_comp(x2, y2)):
    print("Error 2" + "\n")

x3 = "tests\output_3.txt"
y3 = "outputs\out3.txt"
if not(file_comp(x3, y3)):
    print("Error 3" + "\n")

print("DONE!")
