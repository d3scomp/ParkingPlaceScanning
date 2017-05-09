import matplotlib.pyplot as plt
import numpy as np


def plot_ete_latency():
	data = []
	times = []

	with open("log.ete-lattency.txt") as f:
		lines = f.readlines()

		for line in lines:
			parts = line.split(" ")
			data.append(float(parts[2]) * 1000)
			times.append(float(parts[0]))

	fig = plt.figure()

	ax1 = fig.add_subplot(121)
	ax1.boxplot(data, notch=True)
	ax1.set_ylabel("latency in ms")
	ax1.set_title("End to end delay\n(avg " + str(int(round(np.mean(data)))) + " ms)")

	ax2 = fig.add_subplot(122)
	ax2.plot(times, data, '.', color="black", markersize=1.5, fillstyle=None)
	ax2.set_xlabel("timestamp in seconds")
	ax2.set_ylabel("latency in milliseconds")
	ax2.set_title("End to end delay\n(avg " + str(int(round(np.mean(data)))) + " ms)")

	fig.savefig("EtE-latency.pdf")


def plot_ete_distance():
	data = []
	times = []

	with open("log.ete-distance.txt") as f:
		lines = f.readlines()

		for line in lines:
			parts = line.split(" ")
			data.append(float(parts[2]))
			times.append(float(parts[0]))

	fig = plt.figure()

	ax1 = fig.add_subplot(121)
	ax1.boxplot(data, notch=True)
	ax1.set_ylabel("distance in meters")
	ax1.set_title("Scan distance (+ ~ ahead)\n(avg " + str(round(np.mean(data), 1)) + ") m")

	ax2 = fig.add_subplot(122)
	ax2.plot(times, data, '.', color="black", markersize=1.5, fillstyle=None)
	ax2.set_xlabel("timestamp in seconds")
	ax2.set_ylabel("distance in meters")
	ax2.set_title("Scan distance (+ ~ ahead)\n(avg " + str(round(np.mean(data), 1)) + ") m")

	plt.savefig("EtE-distance.pdf")


def plot_cars_scanning():
	data = {}

	with open("log.carscanning.txt") as f:
		lines = f.readlines()

		for line in lines:
			parts = line.split(" ")
			value = float(parts[2])
			time = round(float(parts[0]), 0)
			if time not in data:
				data[time] = 0
			if value:
				data[time] += 1

		fig = plt.figure()

		ax1 = fig.add_subplot(121)
		ax1.boxplot(list(data.values()), notch=True)
		ax1.set_ylabel("#cars scanning")
		ax1.set_title("#cars scanning")
		ax1.set_title("#cars scanning\n(avg " + str(round(np.mean(list(data.values())), 1)) + ")")

		ax2 = fig.add_subplot(122)
		ax2.plot(list(data.keys()), list(data.values()))
		ax2.set_xlabel("timestamp in seconds")
		ax2.set_ylabel("#cars scanning")
		ax2.set_title("#cars scanning\n(avg " + str(round(np.mean(list(data.values())), 1)) + ")")

		plt.savefig("carscanning.pdf")


def plot_num_cars():
	data = []
	times = []

	with open("log.numcars.txt") as f:
		lines = f.readlines()

		for line in lines:
			parts = line.split(" ")
			value = float(parts[1])
			time = float(parts[0])

			data.append(value)
			times.append(time)

		fig = plt.figure()

		ax1 = fig.add_subplot(121)
		ax1.boxplot(data, notch=True)
		ax1.set_ylabel("# cars")
		ax1.set_title("# cars\n(avg " + str(round(np.mean(data), 1)) + ")")

		ax2 = fig.add_subplot(122)
		ax2.plot(times, data)
		ax2.set_xlabel("timestamp in seconds")
		ax2.set_ylabel("# cars")
		ax2.set_title("# cars\n(avg " + str(round(np.mean(data), 1)) + ")")

		plt.savefig("cars.pdf")


def plot_server_queue():
	data = {}
	times = {}

	with open("log.serverqueue.txt") as f:
		lines = f.readlines()

		for line in lines:
			parts = line.split(" ")
			value = float(parts[2])
			server = str(parts[1])
			time = float(parts[0])

			if server not in data:
				data[server] = []
				times[server] = []

			data[server].append(value)
			times[server].append(time)

		fig = plt.figure()

		ax1 = fig.add_subplot(121)
		boxdata = []
		for server in data.keys():
			boxdata.append(data[server])
		ax1.boxplot(boxdata, notch=True)
		ax1.set_xlabel("server")
		ax1.set_ylabel("# scans to process")
		ax1.set_title("server queue length")

		ax2 = fig.add_subplot(122)
		cnt = 0
		colors = ["red", "green", "blue"]
		for server in data.keys():
			ax2.plot(times[server], data[server], color=colors[cnt])
			cnt += 1
		ax2.set_xlabel("timestamp in seconds")
		ax2.set_ylabel("# scans to process")
		ax2.set_title("server queue length")

		plt.savefig("serverqueue.pdf")


plot_server_queue()
plot_num_cars()
plot_cars_scanning()
plot_ete_latency()
plot_ete_distance()