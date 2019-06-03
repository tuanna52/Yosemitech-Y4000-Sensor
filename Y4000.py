#!/usr/bin/env Python3.7

import serial
from time import strftime, sleep
from ast import literal_eval
import sqlite3

#Location
location = 'Tank1'

#Serial port
serial_port = '/dev/ttyACM0'

#Connect to database
conn = sqlite3.connect('y4000.db')
#Create cursor for sqlite3 connection
db = conn.cursor()

#Connect to Serial Port for communication
ser = serial.Serial(serial_port, 9600, timeout=0.1)

#Setup a loop to send Temperature values at fixed intervals in seconds
fixed_interval = 6

def main():
    while 1:
        try:
            #Value obtained from Arduino + Atlas sensors
            predata = ser.readline()
            predata_utf = predata.decode("utf-8")
            data = literal_eval(predata_utf)

            #Extract data from serial port
            DO = data['DO']
            Tur = data['Tur']
            CT = data['CT']
            pH = data['pH']
            Temp = data['Temp']
            Orp = data['Orp']
            Chl = data['Chl']
            BGA = data['BGA']

            #current time and date
            time_hhmmss = strftime('%H:%M:%S')
            date_mmddyyyy = strftime('%d/%m/%Y')

            #current location name
            print(f"{time_hhmmss}, {date_mmddyyyy}, {location}")

            print(f"DO: {DO} --- Tur: {Tur} --- CT: {CT} --- pH: {pH} 
--- Temp: {Temp} --- Orp: {Orp} --- Chl: {Chl} --- BGA: {BGA}")

            #Insert a row of data to data table
            db.execute("INSERT INTO data(location, do, tur, ct, ph, 
temp, orp, chl, bga, date, time) VALUES 
(?,?,?,?,?,?,?,?,?,?,?)",(location, DO, Tur, CT, pH, Temp, Orp, Chl, 
BGA, date_mmddyyyy, time_hhmmss))
            conn.commit()

            sleep(fixed_interval)

        except:
            print('Something has been wrong!')
            sleep(fixed_interval)

if __name__ == '__main__':
    main()

