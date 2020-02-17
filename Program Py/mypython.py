import string
import random
import sys

def generateLetters():
	myStr = ""
	for i in range(0,10):
		myStr += random.choice(string.ascii_lowercase)
	return myStr

def makeFiles():
	for i in range(0,3):
		orig_stdout = sys.stdout;
		fileName = "file" + str(i) + ".txt"
		f = open(fileName,"w+")
		randLetters = generateLetters()
		print(randLetters)
		sys.stdout = f;
		print(randLetters)
		sys.stdout = orig_stdout;
		f.close()

makeFiles()
num1 = (random.randint(1,42))
num2 = (random.randint(1,42))
print(num1)
print(num2)
print(num1*num2)