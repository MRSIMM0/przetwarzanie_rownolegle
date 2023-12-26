from os import listdir, path, getcwd, makedirs
from os.path import isdir, join, isfile, exists
import subprocess
from concurrent.futures import ThreadPoolExecutor

def compile_file(dir, file):
    if file[-4:] == ".cpp":
        command = "g++-13 -fopenmp -O3 -o " +  "./bin/" + dir+file[:-4] + " " + dir + "/" + file
        process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()
        if process.returncode != 0:
            print(f"Wystąpił błąd: {stderr.decode()}")
            return
        print(dir + "/" + file[:-4])

my_path = path.abspath(getcwd())
dirs = [f for f in listdir(my_path) if isdir(f) and f[0] != "."]

# Sprawdź, czy folder bin istnieje, jeśli nie - utwórz go
if not exists("./bin"):
    makedirs("./bin")

with ThreadPoolExecutor() as executor:
    for dir in dirs:
        files = [f for f in listdir(dir) if isfile(join(dir, f))]
        for file in files:
            executor.submit(compile_file, dir, file)