import subprocess
import re

NUM_TASKS = 128
NUM_TESTS = 12

def generateCode(i):
    backup_file = 'saxpy.ispc.backup'
    new_file = 'saxpy.ispc'
    
    with open(backup_file, 'r') as file:
        lines = file.readlines()
    
    lines[35] = f'\tuniform int span = N / {i};\n'

    with open(new_file, 'w') as file:
        file.writelines(lines)

def getSpeedUp(s):
    pattern = r"\((\d+\.\d+)x speedup"
    match = re.search(pattern, s)
    return round(float(match.group(1)), 2) if match else -1

def getBW(s):
    pattern = r"\[(\d+\.\d+)\] GB/s"
    match = re.search(pattern, s)
    return round(float(match.group(1)), 2) if match else -1

def test(taskNum):
    generateCode(taskNum)
    result = subprocess.run(['make'], capture_output=True, text=True)
    result.check_returncode()
    bw_sum, speedup_sum = 0, 0
    for i in range(NUM_TESTS):
        result = subprocess.run(['./saxpy'], capture_output=True, text=True)
        result.check_returncode()
        output = result.stdout.split('\n')
        bw = getBW(output[-5])
        speedup = getSpeedUp(output[-2])
            
        assert bw > 0 and speedup > 0, 'should be greater than 0'
            
        bw_sum += bw
        speedup_sum += speedup
    return round(bw_sum / NUM_TESTS, 3), round(speedup_sum / NUM_TESTS, 3)
        
tasks = []
BW = []
speedUp = []

for i in range(1, NUM_TASKS + 1):
    bw, speedup = test(i)
    tasks.append(i)
    BW.append(bw)
    speedUp.append(speedup)
    print(i, bw, speedup)
    
print(tasks)
print(BW)
print(speedUp)