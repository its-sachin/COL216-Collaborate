import subprocess
import sys

n = int(sys.argv[1])
m = int(sys.argv[2])
testfiles = []
testStr = ""
for i in range(n):
    testfiles.append(sys.argv[3+i])
    testStr += sys.argv[3+i] + " "
rowdel = int(sys.argv[3+n])
coldel = int(sys.argv[4+n])


print("\nExecuting assignment 5")
subprocess.run("./a.out " + str(n) + " " + str(m) + " " + testStr + str(rowdel) + " " + str(coldel) + " 0",shell=True)

print ("\n------------------------------------------------------------------")

print("\nExecuting assignment 4")

for i in range(n):
    subprocess.run("./b.out " + testfiles[i] + " " + str(rowdel) + " " + str(coldel) + " " + str(i) + " " + str(n),shell=True)

old = [[]]
new = [[]]
for i in range(n):
    oldname = "regbefore" + str(i+1) + ".txt"
    newname = "regafter" + str(i+1) + ".txt"

    oldFile = open(oldname).readlines() 
    newFile = open(newname).readlines() 

    oldFile_line = []
    newFile_line = []

    for lines in oldFile: 
        oldFile_line.append(lines) 

    for lines in newFile: 
        newFile_line.append(lines) 

    old.append(oldFile_line)
    new.append(newFile_line)

done = True

for i in range(n):
    oldFile_line = old[i]
    newFile_line = new[i]

    for i in range(len(oldFile_line)):
        if (oldFile_line[i] != newFile_line[i]):
            done = False
            print("Output dont match at core ",i+1 )
            print("Without reordering: ", oldFile_line)
            print("Witg reordering: ", newFile_line)

if (done):
    print("\n\nOutput Matches!!")
