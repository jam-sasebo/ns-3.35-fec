#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import numpy as np

f = open(sys.argv[1],'r')

def get_evenhalf(lst):
	return [lst[i] for i in range(len(lst)) if i%2==0 ]
def get_oddhalf(lst):
	return [lst[i] for i in range(len(lst)) if i%2==1 ]


for line in f:
	line = line.rstrip()
	items = line.split(' ')
	data = dict(map(lambda key,val: (key,val),get_evenhalf(items),get_oddhalf(items)))
	print(data)