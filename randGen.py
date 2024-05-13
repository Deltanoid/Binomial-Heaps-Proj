import random
import os
import sys

abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)


def genNums(size):
    nums = [1]
    for i in range(size):
        new = nums[-1] + random.randint(1,20)
        nums.append(new)    
    return nums

def outfileNums(nums):
    f= open('random_inputs.txt','w')
    line = ''
    for i in nums:
        line += str(i) + ' '
    line = line[:-1]
    f.write(line)
    f.close()

if __name__ == '__main__':
    nums = genNums(2000)
    outfileNums(nums)