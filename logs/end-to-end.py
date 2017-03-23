import matplotlib.pyplot as plt
import numpy as np

data = []

with open("end-to-end-latency.txt") as f:
	lines = f.readlines()

	for line in lines:
		data.append(float(line) * 1000)

ax1 = plt.subplot(121)
ax1.boxplot(data, notch=True)
plt.ylabel("ms")
plt.title("End to end delay - boxplot")

ax2 = plt.subplot(122)
ax2.plot(data, 'wo')
plt.title("End to end delay - all samples")

plt.savefig("EtE.pdf")
