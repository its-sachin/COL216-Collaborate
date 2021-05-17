import subprocess

test = input("Enter test file: ")
delays = input("Enter delays: ")

print("\nExecuting by reordering")
subprocess.run("./a.exe " + test + " " + delays + " " + "0")
print ("\n------------------------------------------------------------------")
subprocess.run("./a.exe " + test + " " + delays + " " + "1")

oldName = "./old.txt"
newName = "./new.txt"

oldFile = open(oldName).readlines() 
 
oldFile_line = [] 
 
clockNew = 0
clockOld = 0

i = 0
for lines in oldFile: 
    if (i == 0):
        clockOld = int(lines[6:])
        i = 1
    else:
	    oldFile_line.append(lines) 
 
newFile = open(newName).readlines() 
 
newFile_line = [] 

i = 0 
for lines in newFile: 
    if (i == 0):
        clockNew = int(lines[6:])
        i = 1
    else:
	    newFile_line.append(lines) 

print ("\n")

if (len(oldFile_line) != len(newFile_line)):
    print("Output not same!!")

else:
    done = True
    for i in range(len(oldFile_line)):
        if (oldFile_line[i] != newFile_line[i]):
            done = False
            print("Without reordering: ", oldFile_line)
            print("Witg reordering: ", newFile_line)

    if (done):
        print("Output Matches!!")

    print("Gain in clocks: ", clockOld-clockNew)
