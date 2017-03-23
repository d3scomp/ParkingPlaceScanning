import matplotlib.pyplot as plt
import numpy as np

data = []

with open("end-to-end lattency.txt") as f:
	lines = f.readlines()

	for line in lines:
		data.append(float(line) * 1000)

#plt.boxplot(data, notch=True, bootstrap=10000)
plt.plot(data, 'bo')
plt.ylabel("ms")

plt.show()
