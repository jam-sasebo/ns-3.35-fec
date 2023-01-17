#!/usr/bin/env python3

import sys
import copy
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.transforms import Bbox 
import numpy as np

routers = [5,7,9,11,13,15]
fwins = [10,20,0]
groups = [1,3,0]
flows = [10,20]
sim_ids = ["tcprouter","fecrouter","fecend"]
values = ["thp",'retr','to','redun','mc','lossr','effrec','fr']
slowdown = 0.8
delay = 100

def aton(s):
	try:
		return int(s)
	except ValueError:
		try:
			return float(s)
		except ValueError:
			return s

def get_stream(index_lst,obj):
	return {i:copy.deepcopy(obj) for i in index_lst}
v_streams = get_stream(values,0)
s_streams = get_stream(sim_ids,v_streams)
r_streams = get_stream(routers,s_streams)
w_streams = get_stream(fwins,r_streams)
g_streams = get_stream(groups,w_streams)
f_streams = get_stream(flows,g_streams)


#Parse data into dict
f = open(sys.argv[1],'r')
for line in f:
	line = line.rstrip()
	items = line.split(' ')
	data = { k:v for (k,v) in zip(items[::2],items[1::2])}
	for k,v in data.items():
		data[k] = aton(v)
	for v in values:
		f_streams[data['f']][data['g']][data['w']][data['r']][data['id']][v]=data[v]
f.close()

for f,g_stream in f_streams.items():
	for g,w_stream in g_stream.items():
		for w,r_stream in w_stream.items():
			for r,s_stream in r_stream.items():
				for sim_id,v_stream in s_stream.items():

input("Data parsed")

def plot_thp(f,g_streams):
	for g,w_stream in f_streams.items():
		max_text = dict()
		m = {'value':-99999999,'Text':'','id':0}
		for w,r_stream in w_stream.items():
			for r,s_stream in r_stream.items():
				input()
			# m_ = m['value']
			# m_id = groups[thp_stream.index(m_)]  #あるいはそれに対するグループ数の値
			# if m_ > m['value']:
			# 	m['value']=m_
			# 	m['id']=m_id
			# 	m['Text']=f'Maximum Throughput: \nFec-Router g:{m_id} w:{w} thp-gain:{m_}[%]'
			ax.plot(groups,thp_stream, \
			linestyle='-',\
			marker='.',\
			markersize=4,\
			markeredgecolor="k",\
			markeredgewidth=0.05,\
			label=f'FECR-w={w}',\
			linewidth=0.75)

			ax.plot(groups,thp_stream_, \
			linestyle=':',\
			marker='s',\
			markersize=2,\
			markeredgecolor="k",\
			markeredgewidth=0.05,\
			label=f'FECE-wmax={wmax}',\
			linewidth=0.75)
			max_text=m

			ax.plot(groups,\
				[0]*5,\
				color='k', 
				linestyle=':',\
				label='TCP',\
				linewidth=0.75)

		ax.annotate(max_text['Text'], xy=(max_text['id'], max_text['value']),  xycoords='data',\
	        xytext=(0.1, 1), textcoords='axes fraction',\
	        arrowprops=dict(facecolor='black', arrowstyle='->'),\
	        horizontalalignment='left', verticalalignment='bottom', fontsize=5\
	        )

		ax.legend(title=f"TCP-thp= {tcp_thpstream[d]} [Mbps]",bbox_to_anchor=(1,1),loc="best",fontsize=5,ncol=2)

		ax.set_xlabel(f"number of groups g \n delay = {d} [ms] TCP-avg-cwnd = {tcp_cwndstream[d]} [pkt]",fontsize=6)
		ax.set_ylabel("Throughput/Tcp  [%]",fontsize=6)
		ax.set(ylim=(-5,max_text['value']+2))
		ax.tick_params(axis='y', labelsize=5)
		plt.xticks((1, 3, 5, 7, 9),fontsize=5)
		plt.grid(linestyle='-')
		plt.tight_layout()

		#Instead of getting extents, use the following
		# for i, ax in enumerate(fig.axes):
    	# 	ax.set_ylabel(str(i))
		extent = ax.get_window_extent().transformed(fig.dpi_scale_trans.inverted())
		pts = extent.get_points()
		def expanded_(bbx, sw, sh):
			width = bbx.width
			height = bbx.height
			deltaw = (sw * width - width) / 2.0
			deltah = (sh * height - height) / 2.0
			a = np.array([[-width/3, -height/2.5], [deltaw, deltah]])
			return Bbox(bbx.get_points() + a)
		fig.savefig(f'FECR-FECE-TCP-thp-d{d}.pdf', bbox_inches=expanded_(extent,4.5,1.5))

	plt.savefig("thp-diff.pdf")

def main():
	for f,g_stream in f_streams.items():
		plot_thp(f,g_stream)
		input("Here")

main()

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