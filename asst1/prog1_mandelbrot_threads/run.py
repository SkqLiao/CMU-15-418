import subprocess
import matplotlib.pyplot as plt
import re
import numpy as np

NUM_TESTS = 20
NUM_THREADS = 900

def plot(threads, speedUp):
    plt.figure(figsize=(20, 12))
    plt.scatter(threads, speedUp, color='red', marker='x', s=20)
    plt.plot(threads, speedUp, linestyle='dashed', color='green')
    plt.title('Speedup vs Number of Threads', fontsize=20)
    plt.xlabel('Number of Threads', fontsize=20)
    plt.ylabel('Speedup', fontsize=20)
    plt.xticks(fontsize=20)
    plt.yticks(fontsize=20)
    plt.show()

def getSpeedUp(s):
    match = re.search(r'\((\d+\.\d+)', s)
    return round(float(match.group(1)), 2) if match else -1

def testSpeed(threadNum):
    total = []
    global NUM_TESTS
    for i in range(NUM_TESTS):
        result = subprocess.run(['./mandelbrot', '-t', str(threadNum)], capture_output=True, text=True)
        result.check_returncode()
        output = result.stdout.split('\n')[-2]
        
        speedUp = getSpeedUp(output)
        
        assert speedUp > 0, 'Speed up should be greater than 0'
        
        total.append(speedUp)
        
    return total

def getNext(i):
    delta, now = 1, 64
    while now <= i:
        delta += 1
        now *= 2
    return i + delta

threads, speedUp = [], []

i = 1
while i <= NUM_THREADS:
    speedup = testSpeed(i)
    threads.append(i)
    speedUp.append(speedup)
    print('Thread number: {:d}, Speed up: {:.2f}'.format(i, np.average(speedup)))
    i = getNext(i)

print(threads)
print(speedUp)
plot(threads, speedUp)