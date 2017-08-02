import matplotlib.pyplot as plt
import numpy as np
import os
import glob
import re
from operator import attrgetter


def plot_ete_latency():
	data = []
	times = []

	with open("log.ete-lattency.txt") as f:
		lines = f.readlines()

		for line in lines:
			parts = line.split(" ")
			data.append(float(parts[2]) * 1000)
			times.append(float(parts[0]))

	ete_lattency = np.mean(data)

	fig = plt.figure()

	ax1 = fig.add_subplot(121)
	ax1.boxplot(data, notch=True)
	ax1.set_ylabel("latency in ms")
	ax1.set_title("End to end delay\n(avg " + str(int(round(ete_lattency))) + " ms)")

	ax2 = fig.add_subplot(122)
	ax2.plot(times, data, '.', color="black", markersize=1.5, fillstyle=None)
	ax2.set_xlabel("timestamp in seconds")
	ax2.set_ylabel("latency in milliseconds")
	ax2.set_title("End to end delay\n(avg " + str(int(round(ete_lattency))) + " ms)")

	fig.savefig("EtE-latency.pdf")
	plt.close()

	return ete_lattency

def plot_ete_distance():
	data = []
	times = []

	with open("log.ete-distance.txt") as f:
		lines = f.readlines()

		for line in lines:
			parts = line.split(" ")
			data.append(float(parts[2]))
			times.append(float(parts[0]))

	ete_distance = np.mean(data)

	fig = plt.figure()

	ax1 = fig.add_subplot(121)
	ax1.boxplot(data, notch=True)
	ax1.set_ylabel("distance in meters")
	ax1.set_title("Scan distance (+ ~ ahead)\n(avg " + str(round(ete_distance, 1)) + ") m")

	ax2 = fig.add_subplot(122)
	ax2.plot(times, data, '.', color="black", markersize=1.5, fillstyle=None)
	ax2.set_xlabel("timestamp in seconds")
	ax2.set_ylabel("distance in meters")
	ax2.set_title("Scan distance (+ ~ ahead)\n(avg " + str(round(ete_distance, 1)) + ") m")

	plt.savefig("EtE-distance.pdf")
	plt.close()

	return ete_distance

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

		num_scanning = np.mean(list(data.values()))

		ax1 = fig.add_subplot(121)
		ax1.boxplot(list(data.values()), notch=True)
		ax1.set_ylabel("#cars scanning")
		ax1.set_title("#cars scanning")
		ax1.set_title("#cars scanning\n(avg " + str(round(num_scanning, 1)) + ")")

		ax2 = fig.add_subplot(122)
		ax2.plot(list(data.keys()), list(data.values()))
		ax2.set_xlabel("timestamp in seconds")
		ax2.set_ylabel("#cars scanning")
		ax2.set_title("#cars scanning\n(avg " + str(round(num_scanning, 1)) + ")")

		plt.savefig("carscanning.pdf")
		plt.close()

		return num_scanning

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

		avgnumcars = np.mean(data)

		fig = plt.figure()

		ax1 = fig.add_subplot(121)
		ax1.boxplot(data, notch=True)
		ax1.set_ylabel("# cars")
		ax1.set_title("# cars\n(avg " + str(round(avgnumcars, 1)) + ")")

		ax2 = fig.add_subplot(122)
		ax2.plot(times, data)
		ax2.set_xlabel("timestamp in seconds")
		ax2.set_ylabel("# cars")
		ax2.set_title("# cars\n(avg " + str(round(avgnumcars, 1)) + ")")

		plt.savefig("cars.pdf")
		plt.close()

	return avgnumcars

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
		plt.close()


def plot_all():
	base = os.getcwd()
	global_data = {}

	for log in glob.glob("*.logs"):
		os.chdir(base + os.sep + log)
		print("Processing logs in " + os.getcwd())

		probability = float(re.sub(".*-|\.ini.*", "", log))
		#plot_server_queue()
		num_cars = plot_num_cars()
		num_scanning = plot_cars_scanning()
		ete_latency = plot_ete_latency()
		ete_distance = plot_ete_distance()

		global_data[probability] = {
			"probability": probability,
			"num_cars": num_cars,
			"num_scanning": num_scanning,
			"ete_latency": ete_latency,
			"ete_distance": ete_distance
		}

	os.chdir(base)

	return global_data


def plot_global(global_data):
	print("Unsorted Data: " + str(global_data.values()))
	data = sorted(list(global_data.values()), key=lambda x: x["probability"])
	print("Data: " + str(data))

	fig = plt.figure()

	ax1 = fig.add_subplot(1,1,1)

	num_cars = list(map(lambda x: x["num_cars"], data))
	ete_latency = list(map(lambda x: x["ete_latency"], data))

	print(num_cars)

	print(ete_latency)

	ax1.plot(num_cars, ete_latency, "bs")


	#boxdata = []
	#for server in data.keys():
	#	boxdata.append(data[server])
	#ax1.boxplot(boxdata, notch=True)
	#ax1.set_xlabel("server")
	#ax1.set_ylabel("# scans to process")
	#ax1.set_title("server queue length")

	plt.savefig("global.pdf")
	plt.close()



global_data = plot_all()
plot_global(global_data)