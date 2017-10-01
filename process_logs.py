import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import os
import glob
import re
import pickle
import os.path


def plot_ete_latency():
	try:
		if os.path.getctime('log.ete-lattency.txt') < os.path.getctime('log.ete-lattency.dat'):
			file = open('log.ete-lattency.dat', 'r')
			return float(file.readline())
	except:
		pass

	data = []
	times = []

	with open("log.ete-lattency.txt") as f:
		lines = f.readlines()

		for line in lines:
			parts = line.split(" ")
			data.append(float(parts[2]) * 1000)
			times.append(float(parts[0]))

	ete_lattency = np.mean(data)
	file = open('log.ete-lattency.dat', 'w')
	file.write(str(ete_lattency))
	file.close()

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
	try:
		if os.path.getctime('log.ete-distance.txt') < os.path.getctime('log.ete-distance.dat'):
			file = open('log.ete-distance.dat', 'r')
			return float(file.readline())
	except:
		pass

	data = []
	times = []

	with open("log.ete-distance.txt") as f:
		lines = f.readlines()

		for line in lines:
			parts = line.split(" ")
			data.append(float(parts[2]))
			times.append(float(parts[0]))

	ete_distance = np.mean(data)
	file = open('log.ete-distance.dat', 'w')
	file.write(str(ete_distance))
	file.close()

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
	try:
		if os.path.getctime('log.carscanning.txt') < os.path.getctime('log.carscanning.dat'):
			file = open('log.carscanning.dat', 'r')
			return float(file.readline())
	except:
		pass

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
		file = open('log.carscanning.dat', 'w')
		file.write(str(num_scanning))
		file.close()

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
	try:
		if os.path.getctime('log.numcars.txt') < os.path.getctime('log.numcars.dat'):
			file = open('log.numcars.dat', 'r')
			return float(file.readline())
	except:
		pass

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
		file = open('log.numcars.dat', 'w')
		file.write(str(avgnumcars))
		file.close()

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
	try:
		if os.path.getctime('log.serverqueue.txt') < os.path.getctime('serverqueue.pdf'):
			return
	except:
		pass

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

		data = {
			"probability": probability,
			"num_cars": num_cars,
			"num_scanning": num_scanning,
			"ete_latency": ete_latency,
			"ete_distance": ete_distance
		}

		global_data[probability] = data

	os.chdir(base)

	return global_data


def plot_global(global_data):
	data = list(global_data.values())

	fig = plt.figure()

	ax1 = fig.add_subplot(1,1,1)

	num_cars = list(map(lambda x: x["num_cars"], data))
	ete_latency = list(map(lambda x: x["ete_latency"], data))

	qn_num_cars = (10, 20, 30, 40, 50)
	qn_ete_latency = (140.86, 341.67, 783.92, 1381.9, 1949.83)

	print("Num cars: ")
	print(num_cars)

	print("EtE latency")
	print(ete_latency)

	ax1.plot(num_cars, ete_latency, "bx")
	ax1.plot(qn_num_cars, qn_ete_latency, "rx")

	ax1.set_xlabel("Average number of cars")
	ax1.set_ylabel("Average end to end latency in milliseconds")
	ax1.set_title("Average number of cars vs average EtE latency")

#	ax1.set_ylim(0, 10000)
	ax1.set_yscale("log", nonposy='clip')

	blue_patch = mpatches.Patch(color='blue', label='Simulation data')
	red_patch = mpatches.Patch(color='red', label='Prediction data')
	plt.legend(handles=[blue_patch, red_patch])

	plt.savefig("global.pdf")
	plt.close()


def plot_global_num_vs_probability(global_data):
	data = sorted(list(global_data.values()), key=lambda x: x["probability"])

	print("Data: " + str(data))
	file = open('global.data', 'w')
	file.write(repr(data))
	file.close()

	fig = plt.figure()

	ax1 = fig.add_subplot(1,1,1)

	num_cars = list(map(lambda x: x["num_cars"], data))
	probability = list(map(lambda x: x["probability"], data))

	print(num_cars)
	print(probability)

	ax1.plot(probability, num_cars, "bx")

	ax1.set_xlabel("Car emergence probability")
	ax1.set_ylabel("Average number of cars")
	ax1.set_title("Probability vs num cars")

	plt.savefig("global_num_vs_probability.pdf")
	plt.close()



global_data = plot_all()
plot_global(global_data)
plot_global_num_vs_probability(global_data)

#print data in simple format
print("num_cars ete_latency [ms]")
for record in sorted(list(global_data.values()), key=lambda x: x["num_cars"]):
	print(str(record["num_cars"]) + " " + str(record["ete_latency"]))