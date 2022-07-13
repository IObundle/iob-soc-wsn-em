#!/usr/bin/python

import datetime
from sys import argv
import pytz
import csv
from csv import reader

def get_time():
    gt=datetime.datetime.now(pytz.utc)
    return (gt.strftime("%Y:%m:%d:%H:%M:%S:%f")[:-3]+"Z")

in_file1=argv[1]
in_file2=argv[2]
out_file="toserver.csv"
data=[]

f=open(in_file1, "r")
fcontent=f.readlines()
with open(in_file2, 'r') as fgps:
    csv_reader = reader(fgps)
    for row in csv_reader:
        if(row[0].strip().lower()==fcontent[2].strip().lower()):
            gps=row[1].strip()+", "+row[2].strip()
            data.insert(0, gps)   
            data.insert(1,fcontent[3].strip())
            break
data.insert(1, get_time())

with open(out_file, 'w') as fcsv:
    writer = csv.writer(fcsv, delimiter = "|")
    writer.writerow(data)
f.close()
fgps.close()
fcsv.close()
