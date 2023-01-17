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
pdf = matplotlib.backends.backend_pdf.PdfPages("exhaustive-output.pdf")
sns.set(font="IPAexGothic")
delays = [5,10,25,50,100]
fwins = [10,20,50,75,100]
groups = [1,3,5,7,9]
flows = [20]
sim_ids = ["tcprouter","fecrouter","fecend"]
values = ["thp",'retr','to','redun','mc','lossr','effrec','fr']
slowdown = 0.8

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
	for f in flows:
		f_stream = df.loc[df['nFlows'] == f].copy()
		for val in sys.argv[2::]:
			cols = {'fwin':'FECウインドウfwin','nGroups':'グループ数g','coredelay':'遅延時間d'}
			rows = {'fecrouter':'FEC-ROUTER','fecend':'FEC-END','tcprouter':'TCP'}
			f_stream_ = f_stream.rename(columns=cols)
			f_stream_ = f_stream_.replace(rows)
			title = ""
			if val in ['lossr']:
				ax = sns.lmplot(x='グループ数g',y=val,hue='sim_id',aspect=1,\
					data=f_stream_.loc[f_stream_['sim_id'].isin(['FEC-ROUTER','FEC-END'])],\
					row=cols['coredelay'],order=3, ci=None, legend=False)
			elif val in ['effrec']:
				f_stream_=f_stream_.loc[f_stream_['グループ数g'] == 3]
				title = "グループ数g = 3"
				ax = sns.lmplot(x='グループ数g',y=val,hue='sim_id',aspect=1,\
					data=f_stream_.loc[f_stream_['sim_id'].isin(['FEC-ROUTER','FEC-END'])],\
					row=cols['coredelay'],order=2, ci=None, legend=False)
			elif val in ['redun']:
				ax = sns.lmplot(x='グループ数g',y=val,hue='sim_id',aspect=1,\
					data=f_stream_.loc[f_stream_['sim_id'].isin(['FEC-ROUTER','FEC-END'])],\
					row=cols['coredelay'],order=3, ci=None, legend=False)
			elif val in ['fmaxcount']:
				ax = sns.lmplot(x='グループ数g',y=val,hue='sim_id',aspect=1,\
					data=f_stream_.loc[f_stream_['sim_id'].isin(['FEC-END'])],\
					row=cols['coredelay'],order=3, ci=None, legend=False)
			else:
				ax = sns.lmplot(x='グループ数g',y=val,hue='sim_id',aspect=1,\
					data=f_stream_,col=cols['coredelay'],\
					order=3, ci=None, legend=False)	

			ax.set(xlabel='ルータ台数',ylabel=f'{labels[val][0]}{labels[val][1]}')
			ax.fig.suptitle(f'ルータ台数の影響による「{labels[val][0]}」の変移\nフロー数f = {f} ', fontsize=12, y=0.98)
			ax.add_legend(title=" ")
			plt.xticks(groups)
			make_space_above(ax, topmargin=3)    
			plt.show()
			# pdf.savefig(ax.fig) # save each figure in the pdf
			# plt.savefig(f'f{f}-{labels[val][0]}.pdf')
	pdf.close()


if __name__ == '__main__':
	main()