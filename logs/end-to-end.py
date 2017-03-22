import matplotlib.pyplot as plt

data = []

with open("end-to-end lattency.txt") as f:
	lines = f.readlines()

	for line in lines:
		data.append(float(line) * 1000)

plt.boxplot(data)
plt.ylabel("ms")
plt.show()
