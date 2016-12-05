'''
    Socket program using Threads
    Data received from Particle Photon is of the form "Date:Time:Quantity:PatientInfo"
'''

import socket
import sys
import sqlite3
import thread  
import threading
from threading import Thread
import time
import boto3
import time

#************************ SOCKET connection ************************#
HOST = ''   													# Symbolic name, meaning all available interfaces
PORT = 9000 													# Arbitrary non-privileged port


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print 'Socket created'

# Bind socket to local host and port
try:
    s.bind((HOST, PORT))
except socket.error as msg:
    print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()
 
# Start listening on socket
s.listen(10)
print 'Listening of Port'
#********************* SOCKET connection END **********************#


#*********** Function for handling multiple connections ***********#
def clientthread(conn,patient):
    # infinite loop so that function do not terminate and thread do not end.
    print "Started Client Thread"
    while True:
        client_data = conn.recv(1024)
        client_dataArray = client_data.strip().split(":")
        size = len(client_dataArray)
        #reply = 'OK_DATA'										# Respond with reply once data received
        if size != 4: 											# Check point if data actually received
            continue
        #conn.sendall(reply)									# Acknowledge when data from client received
        infoHour = 	"Time_"+client_dataArray[1]+"00"			# Time of new update on quantity
        time.sleep(2)
        #******************** Write to File *******************#
        date = str(client_dataArray[0])							# Find date
        patient = client_dataArray[3]							# Change patient name as patient in ICU changes through HTML and Photon
        quantity = client_dataArray[2]							# Urine quantity at given time for given patient
        patientFile = patient+"_"+date							# Patient file name format - Patient Name/EMR_Date	
        print "FileName = ", patientFile
        time.sleep(1)
        file = open(patientFile,'a')	
        file.write(infoHour+"\t"+client_dataArray[2]+"\n")		# Write urine quantity data into file whenever received
        time.sleep(1)
        file.close()
        #****************** Write to File End *****************#

		#******************* Write to SQLite ******************#
    	datab = sqlite3.connect('DigitalUri_copy.sqlite')
    	cur = datab.cursor()
    	cur.execute('SELECT ? FROM Urimeter WHERE Patient_Info = ? ', (infoHour, patient))
    	row = cur.fetchone()
        if row is None:
        	cur.execute('''INSERT INTO Urimeter (Patient_Info,Time_000,Time_100,Time_200,Time_300,Time_400,Time_500,Time_600,
        	Time_700,Time_800,Time_900,Time_1000,Time_1100,Time_1200,Time_1300,Time_1400,Time_1500,Time_1600,
        	Time_1700,Time_1800,Time_1900,Time_2000,Time_2100,Time_2200,Time_2300) VALUES (?,NULL,NULL,NULL,NULL,
        	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)''',(patient,))
        	cur.execute('INSERT INTO Vitals (Patient_info,Heart_Rate,Systolic_BP,Diastolic_BP,Pulse_Rate,Breathings) VALUES (?,73,120,80,60,15)',(patient,))
        	cur.execute("UPDATE Urimeter SET " +infoHour+"=? WHERE Patient_Info=?",(quantity,patient,))
        	datab.commit()
    	else : 
        	cur.execute("UPDATE Urimeter SET " +infoHour+"=? WHERE Patient_Info=?",(quantity,patient,))
        	datab.commit()
        #***************** Write to SQLite End ****************#
        
        
        
#********* Function for handling multiple RFID connections *********#  
def rfid_client(conn):
	# infinite loop so that function do not terminate and thread do not end.
    while True:
    	tagID = ""
        rfid_data = conn.recv(1024)
        print rfid_data
        rfid_dataArray = rfid_data.strip().split("-")
        if len(rfid_dataArray)==5:
        	for i in range(0,5):
        		random = str(hex(int(rfid_dataArray[i])))
        		tagValue = random.split("x");
        		tagID = tagID + tagValue[1]
        	print tagID
        	if tagID=="1526a76b":
        		nurseID = "Nurse1"   
        con = sqlite3.connect('DigitalUri_copy.sqlite')
        cur = con.cursor() 
        cur.execute("UPDATE Login SET Login_Status=? WHERE Username=?",(1,"Nurse1"))
        con.commit()


# now keep talking with the client
while 1:
    conn, addr = s.accept()
    print 'Connected with ' + addr[0] + ':' + str(addr[1])
    data = conn.recv(1024)
    print type(data)
    dataArray = data.split(":")
    print dataArray
    if dataArray[0] == "2":
    	print "Here"
    	thread.start_new_thread(rfid_client ,(conn,))
    else:										# Basic Handshake
    	thread.start_new_thread(clientthread ,(conn,data))
    
s.close()