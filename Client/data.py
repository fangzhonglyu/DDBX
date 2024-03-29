host = open("log_host.txt", "r")
client = open("log_client.txt", "r")

import matplotlib.pyplot as plt
from scipy import stats

#read data from log_host.txt and log_client.txt

"""
format of the file

timestep 1
x,y
x,y
x,y
...
timestep 2
x,y
x,y
x,y
...
"""

#read data from log_host.txt into a 2d array and convert the string to float up to the first 500 timesteps
host_data = []
for line in host:
    if line == "\n":
        continue
    if line[0] == "t":
        host_data.append([])
        continue
    if len(host_data) == 500:
        break
    host_data[-1].append([float(x) for x in line.split(",")])

#read data from log_client.txt into a 2d array and convert the string to float up to the first 500 timesteps
client_data = []
for line in client:
    if line == "\n":
        continue
    if line[0] == "t":
        client_data.append([])
        continue
    if len(client_data) == 500:
        break
    client_data[-1].append([float(x) for x in line.split(",")])

#create a 2d array to store the euclidean difference between host and client, host and client might not have the same number of timesteps
diff = []
for i in range(500):
    diff.append([])
    for j in range(min(len(host_data[i]), len(client_data[i]))):
        diff[i].append(((host_data[i][j][0] - client_data[i][j][0])**2 + (host_data[i][j][1] - client_data[i][j][1])**2)**0.5)

#calculate the average euclidean difference for each timestep
avg_diff = []
for i in range(len(diff)):
    avg_diff.append(sum(diff[i])/len(diff[i]))

high = []
for i in range(len(diff)):
    high.append(stats.scoreatpercentile(diff[i], 99))

#calculate the average euclidean difference for all timesteps
avg_diff_all = sum(avg_diff)/len(avg_diff)

#do a full stat of the difference with high 1%, and mean
#im getting inhomgenous. fix it for me
diff_stat = stats.describe(avg_diff)
print(diff_stat)

#plot the average euclidean difference and high 1% for each timestep
plt.plot(avg_diff)
plt.plot(high)
plt.show()

