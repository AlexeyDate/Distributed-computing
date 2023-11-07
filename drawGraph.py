import matplotlib.pyplot as plt

fileNames = ['1.txt', '2.txt', '4.txt', '8.txt']
colors = ['navy', 'blue', 'turquoise', 'aqua']

threadPoints = [[] for _ in range(len(fileNames))]
threadTime = [[] for _ in range(len(fileNames))]

fig = plt.figure(figsize=(11, 7))
plt.ylabel('time (sec)')
plt.xlabel('number of points')

for i, currentFileName in enumerate(fileNames):
    with open(currentFileName, 'r') as file:
        for line in file.readlines():
            points, time = line.split(' ')
            threadPoints[i].append(int(points))
            threadTime[i].append(round(float(time.replace('\n', '')), 2))

    p = 2 ** i
    label = str(p) + ' thread' if i == 0 else str(p) + ' threads'
    plt.plot(threadPoints[i], threadTime[i], marker='o', color=colors[i], label=label)

visiblePoints = []
for point in threadPoints[0]:
    if point % 200 == 0:
        visiblePoints.append(point)
    else:
        visiblePoints.append('')

plt.xticks(ticks=threadPoints[0], labels=visiblePoints)
plt.grid(visible=1, linestyle="--", linewidth=0.5, color="0.5")
plt.legend(loc='upper left')
plt.show()
fig.savefig('TimeUp.png', bbox_inches='tight')


# Calculating Speed Up
fig = plt.figure(figsize=(11, 7))
plt.ylabel('speed up (Tserial / Tparallel)')
plt.xlabel('number of points')

for i in range(len(threadTime)):
    if i > 0:  # skip single thread
        p = 2 ** i
        speedUp = [threadTime[0][j] / threadTime[i][j] for j in range(len(threadTime[0]))]
        plt.plot(threadPoints[i], speedUp, marker='o', color=colors[i], label=str(p) + ' threads')

plt.xticks(ticks=threadPoints[0], labels=visiblePoints)
plt.grid(visible=1, linestyle="--", linewidth=0.5, color="0.5")
plt.legend(loc='upper left')
plt.show()
fig.savefig('SpeedUp.png', bbox_inches='tight')


# Calculating Efficiency
fig = plt.figure(figsize=(11, 7))
plt.ylabel('efficiency (Tserial / (Tparallel * p))')
plt.xlabel('number of points')

for i in range(len(threadTime)):
    if i > 0:  # skip single thread
        p = 2 ** i
        speedUp = [threadTime[0][j] / (threadTime[i][j] * p) for j in range(len(threadTime[0]))]
        plt.plot(threadPoints[i], speedUp, marker='o', color=colors[i], label=str(p) + ' threads')

plt.xticks(ticks=threadPoints[0], labels=visiblePoints)
plt.grid(visible=1, linestyle="--", linewidth=0.5, color="0.5")
plt.legend(loc='upper left')
plt.show()
fig.savefig('Efficiency.png', bbox_inches='tight')
