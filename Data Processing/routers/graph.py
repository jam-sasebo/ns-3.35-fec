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
pdf = matplotlib.backends.backend_pdf.PdfPages("output.pdf")
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

labels = {
	'thp':['総スループット','[Mbps]'],
	'retr':['再送率','[%]'],
	'timeout':['タイムアウト回数',''],
	'redun':['冗長率','[%]'],
	'fmaxcount':['Fmaxカウント',''],
	'lossr':['ロス率','[%]'],
	'effrec':['有効回復成功率','[%]']
}

def make_space_above(axis, topmargin=1):
    """ increase figure size to make topmargin (in inches) space for 
        titles, without changing the axes sizes"""
    fig = axis.figure
    s = fig.subplotpars
    w, h = fig.get_size_inches()

    figh = h - (1-s.top)*h  + topmargin
    fig.subplots_adjust(bottom=s.bottom*h/figh, top=1-topmargin/figh)
    fig.set_figheight(figh)

def main():
	file_path = sys.argv[1]
	df = pd.read_csv(file_path)
	df = df.rename(columns={'fwin':'w','fmax':'fwin'})
	for f in [10,20]:
		f_stream = df.loc[df['nFlows'] == f].copy()
		for val in sys.argv[2::]:
			cols = {'fwin':'FECウインドウfwin','nGroups':'グループ数g'}
			rows = {'fecrouter':'FEC-ROUTER','fecend':'FEC-END','tcprouter':'TCP'}
			f_stream_ = f_stream.rename(columns=cols)
			f_stream_ = f_stream_.replace(rows)
			title = ""
			if val in ['lossr']:
				ax = sns.lmplot(x='nRouters',y=val,hue='sim_id',aspect=1,\
					data=f_stream_.loc[f_stream_['sim_id'].isin(['FEC-ROUTER','FEC-END'])],\
					col=cols['fwin'],row=cols['nGroups'],order=3, ci=None, legend=False)
			elif val in ['effrec']:
				f_stream_=f_stream_.loc[f_stream_['グループ数g'] == 3]
				title = "グループ数g = 3"
				ax = sns.lmplot(x='nRouters',y=val,hue='sim_id',aspect=1,\
					data=f_stream_.loc[f_stream_['sim_id'].isin(['FEC-ROUTER','FEC-END'])],\
					col=cols['fwin'],order=2, ci=None, legend=False)
			elif val in ['redun']:
				ax = sns.lmplot(x='nRouters',y=val,hue='sim_id',aspect=1,\
					data=f_stream_.loc[f_stream_['sim_id'].isin(['FEC-ROUTER','FEC-END'])],\
					order=3, ci=None, legend=False)
			elif val in ['fmaxcount']:
				ax = sns.lmplot(x='nRouters',y=val,hue='sim_id',aspect=1,\
					data=f_stream_.loc[f_stream_['sim_id'].isin(['FEC-END'])],\
					col=cols['fwin'],row=cols['nGroups'],order=3, ci=None, legend=False)
			else:
				ax = sns.lmplot(x='nRouters',y=val,hue='sim_id',aspect=1,\
					data=f_stream_,col=cols['fwin'],row=cols['nGroups'],\
					order=3, ci=None, legend=False)	

			ax.set(xlabel='ルータ台数',ylabel=f'{labels[val][0]}{labels[val][1]}')
			ax.fig.suptitle(f'ルータ台数の影響による「{labels[val][0]}」の変移\nフロー数f = {f}  |  遅延時間d = {delay}[ms] {title}', fontsize=12, y=0.98)
			ax.add_legend(title=" ")
			plt.xticks(routers)
			make_space_above(ax, topmargin=1)    
			# plt.show()
			pdf.savefig(ax.fig) # save each figure in the pdf
			plt.savefig(f'f{f}-{labels[val][0]}.pdf')
	pdf.close()
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

if __name__ == '__main__':
	main()