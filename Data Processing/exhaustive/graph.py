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
sns.set(font="IPAexGothic",style="whitegrid",font_scale=1.5)
delays = [10,100]
fwins = [10,75,100]
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
	'effrec':['有効回復成功率','[%]'],
	'fr':['高速回復カウント','[回]']
}

cols = {	'fwin':'FECウインドウfwin',\
			'nGroups':'グループ数g',\
			'coredelay':'遅延時間d',\
			}
rows = {'fecrouter':'FEC-ROUTER','fecend':'FEC-END','tcprouter':'TCP'}

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
	df = df.loc[df['coredelay'].isin(delays)]
	title = ""
	for flow in flows:
		f_stream = df.loc[df['nFlows'] == flow].copy()
		f_stream = f_stream.rename(columns=cols)
		f_stream = f_stream.replace(rows)
		for val in sys.argv[2::]:

			if val in ['lossr']:
				g = sns.FacetGrid(f_stream, col=cols['coredelay'],margin_titles=True,sharey=False,legend_out=True)
				for (i,j,k),data in g.facet_data():
					ax = g.axes[i,j]
					for w in fwins:
						data_ = data.loc[ (data['sim_id'] == 'FEC-ROUTER') & (data[cols['fwin']] == w) ]						
						fecr = ax.plot(groups,data_[val],'o-',label=f'FEC-R fwin = {w}')
						data_ = data.loc[ (data['sim_id'] == 'FEC-END') & (data[cols['fwin']] == w) ]
						fece = ax.plot(groups,data_[val],'^--',label=f'FEC-E fmax = {w}')
					data_ = data.loc[ (data['sim_id'] == 'TCP') ]
					print(data_[val])
					thp_stream = [data_[val]]*5
					tcp = ax.plot(groups,thp_stream,'kv:',label=f'TCP')
				g.set(ylim=(0,2))
				ax.legend()
				sns.move_legend(ax,'upper left',bbox_to_anchor=(1,1))

			elif val in ['effrec']:
				g = sns.FacetGrid(f_stream, col=cols['coredelay'],margin_titles=True,sharey=False,legend_out=True)
				for (i,j,k),data in g.facet_data():
					ax = g.axes[i,j]
					for w in fwins:
						data_ = data.loc[ (data['sim_id'] == 'FEC-ROUTER') & (data[cols['fwin']] == w) ]						
						fecr = ax.plot(groups,data_[val],'o-',label=f'FEC-R fwin = {w}')
						data_ = data.loc[ (data['sim_id'] == 'FEC-END') & (data[cols['fwin']] == w) ]
						fece = ax.plot(groups,data_[val],'^--',label=f'FEC-E fmax = {w}')
					data_ = data.loc[ (data['sim_id'] == 'TCP') ]
					print(data_[val])
					thp_stream = [data_[val]]*5
				# 	tcp = ax.plot(groups,thp_stream,'kv:',label=f'TCP')
				ax.legend()
				sns.move_legend(ax,'upper left',bbox_to_anchor=(1,1))
			
			elif val in ['thp']:
				# sns.set_theme(style="white")
				g = sns.FacetGrid(f_stream, col=cols['coredelay'],margin_titles=True,sharey=True,legend_out=True)
				for (i,j,k),data in g.facet_data():
					ax = g.axes[i,j]
					for w in fwins:
						data_ = data.loc[ (data['sim_id'] == 'FEC-ROUTER') & (data[cols['fwin']] == w) ]						
						fecr = ax.plot(groups,data_[val],'o-',label=f'FEC-R fwin = {w}')
						data_ = data.loc[ (data['sim_id'] == 'FEC-END') & (data[cols['fwin']] == w) ]
						fece = ax.plot(groups,data_[val],'^--',label=f'FEC-E fmax = {w}')
					data_ = data.loc[ (data['sim_id'] == 'TCP') ]
					print(data_[val])
					thp_stream = [data_[val]]*5
					tcp = ax.plot(groups,thp_stream,'kv:',label=f'TCP')
					g.set(ylim=(75,95))
				ax.legend()
				sns.move_legend(ax,'upper left',bbox_to_anchor=(1,1))

			elif val in ['redun']:
				g = sns.FacetGrid(f_stream, col=cols['coredelay'],margin_titles=True,sharey=False,legend_out=True)
				for (i,j,k),data in g.facet_data():
					ax = g.axes[i,j]
					for w in fwins:
						data_ = data.loc[ (data['sim_id'] == 'FEC-ROUTER') & (data[cols['fwin']] == w) ]						
						fecr = ax.plot(groups,data_[val],'o-',label=f'FEC-R fwin = {w}')
						data_ = data.loc[ (data['sim_id'] == 'FEC-END') & (data[cols['fwin']] == w) ]
						fece = ax.plot(groups,data_[val],'^--',label=f'FEC-E fmax = {w}')
					data_ = data.loc[ (data['sim_id'] == 'TCP') ]
					print(data_[val])
					thp_stream = [data_[val]]*5
					# tcp = ax.plot(groups,thp_stream,'kv:',label=f'TCP')
				# 	# g.set(ylim=(75,95))
				ax.legend()
				sns.move_legend(ax,'upper left',bbox_to_anchor=(1,1))

			elif val in ['fmaxcount']:
				g = sns.FacetGrid(f_stream, col=cols['coredelay'],margin_titles=True,sharey=False,legend_out=True)
				for (i,j,k),data in g.facet_data():
					ax = g.axes[i,j]
					for w in fwins:
						# data_ = data.loc[ (data['sim_id'] == 'FEC-ROUTER') & (data[cols['fwin']] == w) ]						
						# fecr = ax.plot(groups,data_[val],'o-',label=f'FEC-R fwin = {w}')
						data_ = data.loc[ (data['sim_id'] == 'FEC-END') & (data[cols['fwin']] == w) ]
						fece = ax.plot(groups,data_[val],'^--',label=f'FEC-E fmax = {w}')
					data_ = data.loc[ (data['sim_id'] == 'TCP') ]
					print(data_[val])
					thp_stream = [data_[val]]*5
					# tcp = ax.plot(groups,thp_stream,'kv:',label=f'TCP')
					# g.set(ylim=(75,95))
				ax.legend()
				sns.move_legend(ax,'upper left',bbox_to_anchor=(1,1))

			elif val in ['retr']:
				g = sns.FacetGrid(f_stream, col=cols['coredelay'],margin_titles=True,sharey=False,legend_out=True)
				for (i,j,k),data in g.facet_data():
					ax = g.axes[i,j]
					for w in fwins:
						data_ = data.loc[ (data['sim_id'] == 'FEC-ROUTER') & (data[cols['fwin']] == w) ]						
						fecr = ax.plot(groups,data_[val],'o-',label=f'FEC-R fwin = {w}')
						data_ = data.loc[ (data['sim_id'] == 'FEC-END') & (data[cols['fwin']] == w) ]
						fece = ax.plot(groups,data_[val],'^--',label=f'FEC-E fmax = {w}')
					data_ = data.loc[ (data['sim_id'] == 'TCP') ]
					print(data_[val])
					thp_stream = [data_[val]]*5
					tcp = ax.plot(groups,thp_stream,'kv:',label=f'TCP')
					g.set(ylim=(0,1.5))
				ax.legend()
				sns.move_legend(ax,'upper left',bbox_to_anchor=(1,1))

			elif val in ['timeout']:
				g = sns.FacetGrid(f_stream, col=cols['coredelay'],margin_titles=True,sharey=False,legend_out=True)
				for (i,j,k),data in g.facet_data():
					ax = g.axes[i,j]
					for w in fwins:
						data_ = data.loc[ (data['sim_id'] == 'FEC-ROUTER') & (data[cols['fwin']] == w) ]						
						fecr = ax.plot(groups,data_[val],'o-',label=f'FEC-R fwin = {w}')
						data_ = data.loc[ (data['sim_id'] == 'FEC-END') & (data[cols['fwin']] == w) ]
						fece = ax.plot(groups,data_[val],'^--',label=f'FEC-E fmax = {w}')
					data_ = data.loc[ (data['sim_id'] == 'TCP') ]
					print(data_[val])
					thp_stream = [data_[val]]*5
					tcp = ax.plot(groups,thp_stream,'kv:',label=f'TCP')
					g.set(ylim=(0,100))
				ax.legend()
				sns.move_legend(ax,'upper left',bbox_to_anchor=(1,1))

			elif val in ['fr']:
				g = sns.FacetGrid(f_stream, col=cols['coredelay'],margin_titles=True,sharey=False,legend_out=True)
				for (i,j,k),data in g.facet_data():
					ax = g.axes[i,j]
					for w in fwins:
						data_ = data.loc[ (data['sim_id'] == 'FEC-ROUTER') & (data[cols['fwin']] == w) ]						
						fecr = ax.plot(groups,data_[val],'o-',label=f'FEC-R fwin = {w}')
						data_ = data.loc[ (data['sim_id'] == 'FEC-END') & (data[cols['fwin']] == w) ]
						fece = ax.plot(groups,data_[val],'^--',label=f'FEC-E fmax = {w}')
					data_ = data.loc[ (data['sim_id'] == 'TCP') ]
					print(data_[val])
					thp_stream = [data_[val]]*5
					tcp = ax.plot(groups,thp_stream,'kv:',label=f'TCP')
					# g.set(ylim=(0,100))
				ax.legend()
				sns.move_legend(ax,'upper left',bbox_to_anchor=(1,1))

			else:
				g = sns.FacetGrid(f_stream, col=cols['coredelay'],margin_titles=True,sharey=False,legend_out=True)
				for (i,j,k),data in g.facet_data():
					ax = g.axes[i,j]
					for w in fwins:
						data_ = data.loc[ (data['sim_id'] == 'FEC-ROUTER') & (data[cols['fwin']] == w) ]						
						fecr = ax.plot(groups,data_[val],'o-',label=f'FEC-R fwin = {w}')
						data_ = data.loc[ (data['sim_id'] == 'FEC-END') & (data[cols['fwin']] == w) ]
						fece = ax.plot(groups,data_[val],'^--',label=f'FEC-E fmax = {w}')
					data_ = data.loc[ (data['sim_id'] == 'TCP') ]
					print(data_[val])
					thp_stream = [data_[val]]*5
					tcp = ax.plot(groups,thp_stream,'kv:',label=f'TCP')
					# g.set(ylim=(0,2))
				ax.legend()
				sns.move_legend(ax,'upper left',bbox_to_anchor=(1,1))
			g.set(xlabel=cols['nGroups'],ylabel=f'{labels[val][0]}{labels[val][1]}')
			# g.fig.suptitle(f'「{labels[val][0]}」の変移 | フロー数f = {flow} ', y=1.5)
			plt.xticks(groups)
			# make_space_above(g, topmargin=1)    
			plt.show()
			# pdf.savefig(g.fig) # save each figure in the pdf
			g.savefig(f'exhaustive-f{flow}-{val}.pdf')
	pdf.close()


if __name__ == '__main__':
	main()