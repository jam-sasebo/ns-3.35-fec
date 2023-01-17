#!/usr/bin/env python3

import sys
import copy
import re
import numpy as np
import pandas as pd
import seaborn as sns
import japanize_matplotlib
import matplotlib.pyplot as plt
from matplotlib.transforms import Bbox 
from matplotlib.font_manager import FontProperties
import matplotlib.backends.backend_pdf
# matplotlib.font_manager.FontProperties(fname ="IPAPGothic" )
sns.set(font="IPAexGothic")

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

labels = {
	'thp':['総スループット','[Mbps]'],
	'retr':['再送率','[%]'],
	'to':['タイムアウト回数',''],
	'redun':['冗長率','[%]'],
	'mc':['Fmaxカウント',''],
	'lossr':['ロス率','[%]'],
	'effrec':['有効回復成功率','[%]']
}



def main():
	#set output file
	
	#Parse data into dict
	file_path = sys.argv[1]
	df = pd.read_csv(file_path)
	for f in [10,20]:
		f_stream = df.loc[df['nFlows'] == f].copy()
		for val in sys.argv[2::]:
			f_stream_ = f_stream.rename(columns={'fmax':'fwin,fmax'})
			if val == 'lossr':
				ax = sns.lmplot(x='nRouters',y=val,hue='sim_id',data=f_stream_.loc[f_stream['sim_id'].isin(['fecrouter','fecend'])],col='fwin,fmax',row='nGroups',order=3, ci=None, legend=False)	
			else:
				ax = sns.lmplot(x='nRouters',y=val,hue='sim_id',data=f_stream_,col='fwin,fmax',row='nGroups',order=3, ci=None, legend=False)	
			ax.set(xlabel='ルータ台数',ylabel=f'{labels[val][0]}{labels[val][1]}')
			ax.fig.suptitle(f'フロー数f={f}', fontsize=12)
			ax.add_legend(title=" ")
			plt.xticks(routers)
			plt.show()
			# plt.savefig(f'f{f}-{labels[va][0]}.pdf')

		# ax = sns.lmplot(x='nRouters',y='retr',hue='sim_id',data=f_stream,col='fmax',row='nGroups',order=3, ci=None, legend=True)	
		# ax.set(xlabel='ルータ台数',ylabel='再送率[%]')
		# plt.xticks(routers)
		# plt.savefig(f'f{f}-再送率.pdf')

		# ax = sns.lmplot(x='nRouters',y='lossr',hue='sim_id',data=f_stream.loc[f_stream['sim_id'].isin(['fecrouter','fecend'])],col='fmax',row='nGroups',order=3, ci=None, legend=True)	
		# ax.set(xlabel='ルータ台数',ylabel='パケット消失率[%]')
		# plt.xticks(routers)
		# plt.savefig(f'f{f}ロス率.pdf')

		# ax = sns.lmplot(x='nRouters',y='to',hue='sim_id',data=f_stream,col='fmax',row='nGroups',order=3, ci=None, legend=True)	
		# ax.set(xlabel='ルータ台数',ylabel='タイムアウト回数')
		# plt.xticks(routers)
		# plt.savefig(f'f{f}タイムアウト回数.pdf')











		# sns.lmplot(x='nRouters',y='to',hue='sim_id',data=f_stream,col='fmax',row='nGroups',order=3, ci=None, legend=True)	
		# sns.lmplot(x='nRouters',y='redun',hue='sim_id',data=f_stream,col='fmax',row='nGroups',order=3, ci=None, legend=True)	
		# sns.lmplot(x='nRouters',y='effrec',hue='sim_id',data=f_stream,col='fmax',row='nGroups',order=3, ci=None, legend=True)	

		# thp_ax = sns.regplot(x='nRouters',y='thp',data=f_stream.loc[f_stream['sim_id'] == 'fecend'],order=3, ci=None)	
		# sns.regplot(ax = thp_ax,x='nRouters',y='thp',data=f_stream.loc[f_stream['sim_id'] == 'fecrouter'],order=3, ci=None)	
		# sns.regplot(ax = thp_ax,x='nRouters',y='thp',data=f_stream.loc[f_stream['sim_id'] == 'tcprouter'],order=3, ci=None)	
		# retr_ax = thp_ax.twinx()
		# sns.regplot(ax = retr_ax,x='nRouters',y='retr',data=f_stream.loc[f_stream['sim_id'] == 'fecend'],order=3, ci=None)	
		# sns.regplot(ax = retr_ax,x='nRouters',y='retr',data=f_stream.loc[f_stream['sim_id'] == 'fecrouter'],order=3, ci=None)	
		# sns.regplot(ax = retr_ax,x='nRouters',y='retr',data=f_stream.loc[f_stream['sim_id'] == 'tcprouter'],order=3, ci=None)	
		
		# # plt.legend(title=f'f{f}',loc='best')
		# thp_ax.figure.legend()
		# plt.savefig(f'f{f}thp+retr.pdf')




# def main():
# 	#set output file
	
# 	#Parse data into dict
# 	file_path = sys.argv[1]
# 	df = pd.read_csv(file_path)
# 	for f in [10,20]:
# 		f_stream = df.loc[df['nFlows'] == f].copy()
# 		fig, axes = plt.subplots(2, 2)
# 		i=0
# 		for g in [1,3]:
# 			g_stream = f_stream.loc[f_stream['nGroups'].isin([g,0])].copy()
# 			for w in [10,20]:
# 				w_stream = g_stream.loc[g_stream['fmax'].isin([w,0])]
# 				sns.lmplot(ax=axes[i],x='nRouters',y='thp',hue='sim_id',data=w_stream,order=3, ci=None, legend=False)	
# 				plt.legend(title=f'f{f} g{g} w{w}',loc='best')
# 				plt.xticks(routers)
# 				i+=1
# 				print(i)
# 		plt.savefig(f'f{f}thp.pdf')

if __name__ == '__main__':
	main()