import os
from subprocess import call

tdr = "/Users/tanghaomo/works/searches/test"
exe = "/Users/tanghaomo/works/searches/main"


def check_diff(file_1, file_2):
    f_1 = open(file_1)
    f_2 = open(file_2)
    for l1 in f_1:
        l1 = str.strip(l1)
        l2 = str.strip(next(f_2))
        if l1 != l2:
            return False
    if f_2.read():
        return False
    return True


def test_fail(input_file, method):
    print("Failed at input {0} with method {1}".format(input_file, method))


def test(input_file, method):
    test_in = open(input_file)
    temp_in = open(os.path.join(os.path.dirname(exe), "input.txt"), mode="w")
    method_header = method
    if method_header == "A":
        method_header = "A*"
    temp_in.write(method_header + "\n")
    test_in.readline()
    t = test_in.read()
    temp_in.write(t)
    temp_in.close()
    os.chdir(os.path.dirname(exe))
    call([exe])
    if not os.path.exists(os.path.join(os.path.dirname(exe), "output.txt")):
        test_fail(input_file, method)
        return
    else:
        if not check_diff(os.path.join(os.path.dirname(exe), "output.txt"), input_file + "_" + method):
            test_fail(input_file, method)
            return


def recursive_test(cur_path):
    ls = os.listdir(cur_path)
    for item in ls:
        if os.path.isdir(os.path.join(cur_path, item)):
            recursive_test(os.path.join(cur_path, item))
        else:
            if "_" not in item:
                input_file = open(os.path.join(cur_path, item))
                method = str.strip(next(input_file))
                if method == 'A*' or method == 'ALL':
                    if os.path.exists(os.path.join(cur_path, item + '_A')):
                        test(os.path.join(cur_path, item), "A")
                if method == 'UCS' or method == 'ALL':
                    if os.path.exists(os.path.join(cur_path, item + '_UCS')):
                        test(os.path.join(cur_path, item), "UCS")
                if method == 'DFS' or method == 'ALL':
                    if os.path.exists(os.path.join(cur_path, item + '_DFS')):
                        test(os.path.join(cur_path, item), "DFS")
                if method == 'BFS' or method == 'ALL':
                    if os.path.exists(os.path.join(cur_path, item + '_BFS')):
                        test(os.path.join(cur_path, item), "BFS")


recursive_test(tdr)
