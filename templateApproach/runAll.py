__author__ = 'tbergmueller'

import os
import subprocess
import shutil
import re

from subprocess import Popen, PIPE

def makeLine(file, output):

    output = output.replace("<results>", "")
    output = output.replace("</results>", "")

    parts = output.split(";")

    line = os.path.basename(file) + ";"

    if output == "":
        return line + "-666;-666"


    count = 0
    for p in parts:
        kv = p.split(":")
        if count == 0:

            if "lower" in kv[0]:
                # normal, lower found
                line = line + str(kv[1]) + ";"
            else:
                line = line + str(-666) + ";"
            count += 1
        else:
            if "upper" in kv[0]:
                 line = line + str(kv[1]) + ";"
            else:
                line = line + str(-666) + ";"

            break

    return line


def runTest(executable, file):
    process = Popen([executable, file], stdout=PIPE)
    (output, err) = process.communicate()
    exit_code = process.wait()

    regex = "<results>(.*?)<\/results>"

    for line in output.split(os.linesep):
        m = re.search(regex, line)

        if(m is not None):
            #print m.group(0)
            return makeLine(file, m.group(0))

    return makeLine(file, "")




imageDirectory="/home/tbergmueller/DEV/muscle/01_MuscularArchitecture/Muscle_VascusLateralis/"
executable="Debug/templateApproach"

files = os.listdir(imageDirectory)
files.sort()

resultFile = "file;angleToLowerApo;angleToUpperApo\n"

for f in files:
    result = runTest(executable, imageDirectory + f)
   # print result
    resultFile += result + "\n"

with open("TestResults.csv", "w") as text_file:
    text_file.write(resultFile)




