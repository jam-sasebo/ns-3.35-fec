#!/usr/bin/env python3

import sys
import copy
import matplotlib.pyplot as plt
plt.style.use('seaborn-whitegrid')
import numpy as np

delays = [5,10,25,50,100]
groups = [1,3,5,7,9]
fwins = [10,20,50,75,100]
fmaxs = [10,20,50,75,100]
slowdown = 0.8


delayitr = iter(delays)
groupitr = iter(groups) 

def get_stream(obj,index_lst):
	return {i:copy.deepcopy(obj) for i in index_lst}

g_streams = get_stream(0,groups)
#FEC-ROUTER用データ
w_streams = get_stream(g_streams,fwins)
router_d_steams = get_stream(w_streams,delays)
#FEC-END用データ
wx_streams = get_stream(g_streams,fmaxs)
end_d_streams = get_stream(wx_streams,delays)
#TCPデータ
tcp_thpstream = {5:89.5,10:88.1,25:83.8,50:79.3,100:78.0}
tcp_cwndstream = {5:50,10:60,25:86,50:130,100:235}

#Parse data into dict
router = open(sys.argv[1],'r')
for line in router:
	line = line.rstrip()
	items = line.split(' ')
	data = { k:v for (k,v) in zip(items[::2],items[1::2])}
	data['f']=int(data['f'])
	data['d']=int(data['d'])
	data['g']=int(data['g'])
	data['w']=int(data['w'])
	data['thp']=float(data['thp'])
	router_d_steams[data['d']][data['w']][data['g']]=data['thp']
router.close()

end = open(sys.argv[2],'r')
for line in end:
	line = line.rstrip()
	items = line.split(' ')
	data = { k:v for (k,v) in zip(items[::2],items[1::2])}
	data['f']=int(data['f'])
	data['d']=int(data['d'])
	data['g']=int(data['g'])
	data['wmax']=int(data['wmax'])
	data['thp']=float(data['thp'])
	end_d_streams[data['d']][data['wmax']][data['g']]=data['thp']
end.close()


fig = plt.figure()
fig.subplots_adjust(hspace=0.75, wspace=0.5)
fig.suptitle("Throughput of Fec-Router vs Fec-End vs Tcp ( f = 10 )", fontsize=12)
style = dict(size=3, color='black')
router_clrs = ['#330000','#990000','#ff0000','#ff6666','#ffcccc']
end_clrs = ['#09282a','#1a797f','#2bc9d4','#80dfe5','#aeeaee']

for (d,w_stream),(d_,wx_stream) in zip(router_d_steams.items(),end_d_streams.items()):
	i=delays.index(d)
	itr = iter(router_clrs)
	itr_ = iter(end_clrs)
	ax = plt.subplot(3,2,i+1)

	for (w,g_stream),(wmax,g_stream_) in zip(w_stream.items(),wx_stream.items()):
		#Plot Fec-Router's curves
		ax.plot(groups,list(_ for _ in g_stream.values()), \
		color=f'{next(itr)}', 
		linestyle='-',\
		marker='.',\
		markersize=2,\
		label=f'FECR-w={w}',\
		linewidth=0.75)
		#Plot Fec-End's curves
		ax.plot(groups,list(_ for _ in g_stream_.values()), \
		color=f'{next(itr_)}', 
		linestyle='--',\
		marker='s',\
		markersize=2,\
		markeredgecolor="k",\
		markeredgewidth=0.1,\
		label=f'FECE-wm={wmax}',\
		linewidth=0.75)
	
	#Add Tcp curve
	ax.plot(groups,\
		[tcp_thpstream[d]]*5,\
		color='k', 
		linestyle=':',\
		marker='^',\
		fillstyle='none',\
		markersize=2,\
		markeredgewidth=0.4,\
		label='TCP',\
		linewidth=0.75)

	ax.set(ylim=(70,95))
	ax.legend(bbox_to_anchor=(1,1),loc="best",fontsize=5,ncol=2)
	# ax.set_title(f"delay = {d} [ms]",fontsize=10)
	ax.set_xlabel(f"number of groups g \n delay = {d} [ms] TCP-avg-cwnd = {tcp_cwndstream[d]} [pkt]",fontsize=6)
	ax.set_ylabel("Throughput [Mbps]",fontsize=6)
	plt.xticks((1, 3, 5, 7, 9))
	plt.yticks((70, 75, 80, 85, 90,95))
	ax.set_aspect('0.3','box')
	plt.grid(linestyle='-')
	plt.tight_layout()
	extent = ax.get_window_extent().transformed(fig.dpi_scale_trans.inverted())
	fig.savefig(f'FECR-FECE-TCP-thp-d{d}.pdf', bbox_inches=extent.expanded(2, 2))

plt.savefig("thp5.pdf")


# router_clrs = ['darkred','firebrick','indianred','lightcoral','rosybrown']
# end_clrs = ['darkblue','royalblue','']
# fig,ax = plt.subplots(5)
# fig.subplots_adjust(hspace=0.75, wspace=0.4)
# fig.suptitle("Throughput of Fec vs Tcp ( f = 10 )", fontsize=12)
# for d,w_stream in router_d_steams.items():
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