#!/bin/env python
import string
import numpy as np
import sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

#need to get this data from the outFile:
myfilename = "log/graph_and_table_file_MG.txt"
arr2d = {}

global rmax
global tmax
rmax = 0
tmax = 0
f = open(myfilename, 'r')
mylinesarray = f.readlines()
global mylen
mylen = len(mylinesarray)
print mylen
for i in range(1,mylen): 
	if mylinesarray[i] != "\n" and mylinesarray[i] != []:
		myarray = mylinesarray[i].split()
		print myarray[0], i
		if myarray[0] == "AvgCPUtime4innerits":
			tmax = len(myarray) - 1
			print "tmax is "
			print tmax
			j = 1
			row = 0
			column = 0
			myarray2 = mylinesarray[i+1].split()
			while myarray2 != []:	
				for k in range(1,tmax+1):
					arr2d[row, column] = float(myarray2[k])
					print row, column, arr2d[row,column]
					column +=1
				row += 1
				column = 0		
				myarray2 = mylinesarray[i+j+1].split()
				print "myarray2 is"
				print myarray2
				j = j+1
				rmax = j
	
rmax = rmax - 1
print "tmax and rmax are "
print tmax, rmax
t_1=[]

#IMPORTANT ONLY PLOTS UP TO 5 GRAPHS AT THE MOMENT

r_1=[]
r_2=[]
if(rmax == 3):
	r_3=[]

if(rmax == 4):	
	r_3=[]
	r_4=[]
	
if(rmax == 5):	
	r_3=[]
	r_4=[]
	r_5=[] 

if(rmax > 5):
	print "can't do more than 5 graphs\n";
	sys.exit()		


t_1.append(1.0)
		

for k in range(1,tmax):
	t_1.append(t_1[k-1] + 1.0)
	
	
	
for k in range(0,tmax):
	r_1.append(arr2d[0,k])
	r_2.append(arr2d[1,k])
	if(rmax == 3):
		r_3.append(arr2d[2,k])
	if(rmax == 4):	
		r_3.append(arr2d[2,k])
		r_4.append(arr2d[3,k])
	if(rmax == 5):	
		r_3.append(arr2d[2,k])
		r_4.append(arr2d[3,k])
		r_5.append(arr2d[4,k])
	print k, r_1[k], r_2[k]

	

f1 = plt.plot(t_1,r_1, 'go-')
f2 = plt.plot(t_1,r_2, 'ro-')
if(rmax == 3):
	f3 = plt.plot(t_1,r_3, 'bo-')

if(rmax == 4):	
	f3 = plt.plot(t_1,r_3, 'bo-')
	f4 = plt.plot(t_1,r_4, 'po-')
if(rmax == 5):	
	f3 = plt.plot(t_1,r_3, 'bo-')
	f4 = plt.plot(t_1,r_4, 'po-')
	f5 = plt.plot(t_1,r_5, 'ko-')
	
plt.title('Avg CPU Time For Inner Iterations for Each Option File for different Resolutions/Runs')
plt.ylabel('CPU time')
plt.xlabel('OptionFile')
plt.xlim(0,(tmax+1))
plt.grid(True)
plt.draw()
#plt.show()

plt.savefig("log/AvgCPUTimeInnerIts.png", dpi=None, facecolor='w', edgecolor='w',
       orientation='portrait', papertype=None, format=None,
      transparent=False)

