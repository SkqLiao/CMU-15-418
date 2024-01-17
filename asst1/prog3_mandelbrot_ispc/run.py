import subprocess
import re

NUM_THREADS = 64
NUM_TESTS = 8
NUM_VIEWS = 6

def generateCode(i):
    backup_file = 'mandelbrot.ispc.backup'
    new_file = 'mandelbrot.ispc'
    
    with open(backup_file, 'r') as file:
        lines = file.readlines()

    lines[70] = f'\tuniform int threadCount = {i};\n'

    with open(new_file, 'w') as file:
        file.writelines(lines)

def getSpeedUp(s):
    pattern = r"\((\d+\.\d+)x speedup"
    match = re.search(pattern, s)
    return round(float(match.group(1)), 2) if match else -1

def testSpeed(threadNum):
    print('running threadNum = {}'.format(threadNum))
    total1 = [[] for i in range(NUM_VIEWS)]
    total2 = [[] for i in range(NUM_VIEWS)]
    global NUM_TESTS
    generateCode(threadNum)
    result = subprocess.run(['make'], capture_output=True, text=True)
    result.check_returncode()
    for j in range(1, NUM_VIEWS + 1):
        for i in range(NUM_TESTS):
            result = subprocess.run(['./mandelbrot_ispc', '-t', '-v', str(j)], capture_output=True, text=True)
            result.check_returncode()
            output = result.stdout.split('\n')
            speedUp1 = getSpeedUp(output[-3])
            speedUp2 = getSpeedUp(output[-2])
            
            assert speedUp1 > 0 and speedUp2 > 0, 'Speed up should be greater than 0'
            
            total1[j - 1].append(speedUp1)
            total2[j - 1].append(speedUp2)
        
    return total1, total2

threads, speedUp1, speedUp2 = [], [], []

for i in range(1, NUM_THREADS + 1):
    total1, total2 = testSpeed(i)
    threads.append(i)
    speedUp1.append(total1)
    speedUp2.append(total2)
    
print(threads)
print(speedUp1)
print(speedUp2)