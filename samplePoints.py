import random

gravConstant = 6.6743e-11
points = 400
timeSteps = 1000

with open("input.txt", "w") as file:
    file.write(str(gravConstant) + ' ' + str(points) + ' ' + str(timeSteps) + '\n')
    for i in range(points):
        mass = round(random.randint(0, 1000000), 3)
        x = i
        y = i
        Vx = round(random.random() * 100, 3)
        Vy = round(random.random() * 100, 3)
        file.write(str(mass) + '\n')
        file.write(str(x) + ' ' + str(y) + '\n')
        file.write(str(Vx) + ' ' + str(Vy) + '\n')

