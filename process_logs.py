import matplotlib.pyplot as plt
import numpy as np
import os
import glob
import re
import pickle
import os.path
from functools import reduce


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
		if os.path.getctime('log.serverqueue.txt') < os.path.getctime('log.serverqueue.dat'):
			file = open('log.serverqueue.dat', 'r')
			return float(file.readline())
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

		avg_server_queue_len = np.mean(reduce(lambda a,b: a + b, data.values(), []))
		file = open('log.serverqueue.dat', 'w')
		file.write(str(avg_server_queue_len))
		file.close()

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

		return avg_server_queue_len


def plot_all():
	base = os.getcwd()
	global_data = {}

	for log in glob.glob("*.logs"):
		os.chdir(base + os.sep + log)
		print("Processing logs in " + os.getcwd())

		probability = float(re.sub(".*-|\.ini.*", "", log))
		server_queue = plot_server_queue()
		num_cars = plot_num_cars()
		num_scanning = plot_cars_scanning()
		ete_latency = plot_ete_latency()
		ete_distance = plot_ete_distance()

		data = {
			"probability": probability,
			"num_cars": num_cars,
			"num_scanning": num_scanning,
			"ete_latency": ete_latency,
			"ete_distance": ete_distance,
			"server_queue": server_queue
		}

		global_data[probability] = data

	os.chdir(base)

	return list(global_data.values())


def plot_global(global_data):
	data = sorted(global_data, key=lambda x: x["num_cars"])

	fig = plt.figure()

	ax1 = fig.add_subplot(1,1,1)
	ax2 = ax1.twinx()
	ax1.grid(True)

	num_cars = list(map(lambda x: x["num_cars"], data))
	ete_latency = list(map(lambda x: x["ete_latency"], data))
	server_queue = list(map(lambda x: x["server_queue"], data))

	qn_num_cars = (3, 7, 10, 13, 16, 20, 30, 40, 50)
	qn_ete_latency = (93.89, 137.08, 143.67, 187.09, 249.67, 343.48, 783.92, 1381.9, 1949.83)

	# Strip extra data
	real_data_take_first = 57
	num_cars = num_cars[:real_data_take_first]
	ete_latency = ete_latency[:real_data_take_first]
	server_queue = server_queue[:real_data_take_first]

	prediction_data_take_first = 6
	qn_num_cars = qn_num_cars[:prediction_data_take_first]
	qn_ete_latency = qn_ete_latency[:prediction_data_take_first]

	print("Num cars: ")
	print(num_cars)

	print("EtE latency")
	print(ete_latency)

	print("Server queue")
	print(server_queue)

	# Trend lines
	poly_degree = 5
	ete_latency_z = np.polyfit(num_cars, ete_latency, poly_degree)
	ete_latency_p = np.poly1d(ete_latency_z)

	qn_ete_latency_z = np.polyfit(qn_num_cars, qn_ete_latency, poly_degree)
	qn_ete_latency_p = np.poly1d(qn_ete_latency_z)

	ax1.plot(num_cars, ete_latency_p(num_cars), "-", color="lightskyblue")
	ax1.plot(qn_num_cars, qn_ete_latency_p(qn_num_cars), "-", color="salmon")

	ete_plot, = ax1.plot(num_cars, ete_latency, "b^")
	qn_cars_plot, = ax1.plot(qn_num_cars, qn_ete_latency, "rs")
	server_queue_plot, = ax2.plot(num_cars, server_queue, "g*")

	ax1.set_xlabel("Average number of cars")
	ax1.set_ylabel("Average end to end latency in milliseconds")
	ax1.set_title("Average number of cars vs average EtE latency")

	ax2.set_ylabel("Average server queue length")

#	ax1.set_ylim(0, 1000)
#	ax1.set_xlim(0, 25)
#	ax1.set_yscale("log", nonposy='clip')
#	ax2.set_yscale("log", nonposy='clip')

	plt.legend(
		(ete_plot, qn_cars_plot, server_queue_plot),
		("Simulated latency", "Predicted latency", "Server queue length")
	)

	plt.savefig("global.pdf")
	plt.close()

	for num in qn_num_cars:
		print("num_cars: " + str(num) + " : " + str(ete_latency_p(num)))


def plot_global_num_vs_probability(global_data):
	data = sorted(global_data, key=lambda x: x["probability"])

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
print("prob.;\tnum_cars;\t\tete_latency [ms];\tserver_queue")
for record in sorted(global_data, key=lambda x: x["num_cars"]):
	print(str(record["probability"]) + ";\t" + str(record["num_cars"]) + ";\t" + str(record["ete_latency"]) + ";\t\t" + str(record["server_queue"]))


# Process simsec/sec data
with open('simsec-sec.txt') as simsecsec_file:
	simsecsec = list(map(lambda x: float(x), simsecsec_file.read().splitlines()))

simsecsec_avg = sum(simsecsec) / len(simsecsec)
print("Avertage simsec/sec : " + str(simsecsec_avg))
print("Considering 3600 sec, average sim time is : " + str(3600 / simsecsec_avg))

print(simsecsec)