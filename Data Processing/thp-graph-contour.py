#!/usr/bin/env python3

import sys
import copy
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
plt.style.use('seaborn-whitegrid')
import numpy as np
from matplotlib import patheffects


delays = [5,10,25,50,100]
groups = [1,3,5,7,9]
fwins = [10,20,50,75,100]

delayitr = iter(delays)
groupitr = iter(groups) 

def get_stream(obj,index_lst):
	return {i:copy.deepcopy(obj) for i in index_lst}

g_streams = get_stream(0,groups)
w_streams = get_stream(g_streams,fwins)
d_streams = get_stream(w_streams,delays)
tcp_thpstream = {5:89.5,10:88.1,25:83.8,50:79.3,100:78.0}
tcp_cwndstream = {5:50,10:60,25:86,50:130,100:235}
f = open(sys.argv[1],'r')
for line in f:
	line = line.rstrip()
	items = line.split(' ')
	data = { k:v for (k,v) in zip(items[::2],items[1::2])}
	data['f']=int(data['f'])
	data['d']=int(data['d'])
	data['g']=int(data['g'])
	data['w']=int(data['w'])
	data['thp']=float(data['thp'])
	d_streams[data['d']][data['w']][data['g']]=data['thp']
f.close()
# print(d_streams)

# clrs = 'rgbcmyk'
fig = plt.figure()
fig.subplots_adjust(hspace=0.75, wspace=0.5)
fig.suptitle("Throughput of Fec-Router vs Tcp ( f = 10 )", fontsize=12)
style = dict(size=3, color='black')

X, Y = np.meshgrid(groups,fwins)


for d,w_stream in d_streams.items():
	i=delays.index(d)
	Z = [list(g_stream.values()) for g_stream in w_stream.values() ]
	print(Z)
	ax = plt.subplot(3,2,i+1)
	plt.contourf(X, Y, Z, 20, cmap='viridis')
	cb = plt.colorbar()
	cb.set_label(label='Throughput [Mbps]',size=8)
	cb.ax.tick_params(labelsize=6)
	ax.set_xlabel(f"number of groups g \n delay = {d} [ms] Tcp-Thp = {tcp_thpstream[d]} [Mbps]",fontsize=8)
	ax.set_ylabel("fwin",fontsize=8)
	plt.xticks((1,3,5,7,9))
	plt.yticks((10,25,50,75,100))
	plt.grid(linestyle='-')
	ax.set_aspect('0.09','box')
	plt.tight_layout()


plt.savefig("thp6.pdf")

# clrs = 'rgbcmyk'
# fig,ax = plt.subplots(5)
# fig.subplots_adjust(hspace=0.75, wspace=0.4)
# fig.suptitle("Throughput of Fec vs Tcp ( f = 10 )", fontsize=12)
# for d,w_stream in d_streams.items():
# 	i=delays.index(d)
# 	itr = iter(clrs)
# 	for w,g_stream in w_stream.items():
# 		ax[i].plot(groups,g_stream.values(), \
# 			f'{next(itr)}.-',label=f'w = {w}',linewidth=0.75)

# 	ax[i].plot(groups,[tcp_thpstream[d]]*5,'.-k',label='tcp',linewidth=0.75)
# 	ax[i].set(xlim=(0.5,9.5),ylim=(-20,100))
# 	ax[i].legend(bbox_to_anchor=(0.5,0.05),loc="center",ncol=6)
# 	ax[i].set_title(f"delay = {d}[ms])",fontsize=10)
# 	ax[i].set_xlabel("number of groups g")
# 	ax[i].set_ylabel("Throughput [Mbps]")

# plt.show()