from flask import Flask, render_template, request, send_from_directory
import sqlite3 as sql
import requests
app = Flask(__name__)
from datetime import datetime
import itertools
data=[]

@app.route('/',methods = ['POST', 'GET'])
def home():
	del data[:]
	con = sql.connect('DigitalUri_copy.sqlite')
	cur = con.cursor()       
	cur.execute('SELECT * FROM Login WHERE Username = ? ', ("Nurse1",))
	row = cur.fetchall()
	newList =list(itertools.chain.from_iterable(row))
	con.close()	
	print newList
	if newList[2]==1:
		return result("Nurse1")
	else:
		return render_template('index.html')
		

			
@app.route('/login',methods = ['POST', 'GET'])
def login():
	del data[:]
	if request.method == 'POST':
		username = request.form['uname']
		password = request.form['psw']
		con = sql.connect('DigitalUri_copy.sqlite')
		cur = con.cursor()       
		cur.execute('SELECT ? FROM Login WHERE Username = ? ', (password, username))
		row = cur.fetchone()
		if row is None:   
		    return home()
		else:
			cur.execute('UPDATE Login SET Login_Status=? WHERE Username=?',(1,username,))
			con.commit()
			msg ='Access Granted'
			con.close()
			return result(username)
	elif request.method == 'GET':
		print 'Here'
		return "index.html"

@app.route('/result',methods = ['POST','GET'])
def result(username):
	con = sql.connect('DigitalUri_copy.sqlite')
	cur = con.cursor()
	cur.execute("UPDATE Login SET Login_Status=? WHERE Username=?",(0,"Nurse1",))
	cur.execute('SELECT * FROM Urimeter WHERE Nurse_ID = ? ', (username,))
	rows = cur.fetchall()
	for row in rows:
		info = row[0].strip()
		data.append(tuple(info.split('_')))
	con.commit()
	con.close();
	return render_template('accessGranted.html',rows=data)
	
@app.route("/patient/<row>",methods = ['POST','GET'])
def patient(row):
	con = sql.connect('DigitalUri_copy.sqlite')
	cur = con.cursor()
	cur.execute('SELECT * FROM Vitals WHERE Patient_Info = ? ', (row,))
	result = cur.fetchall()
	newList =list(itertools.chain.from_iterable(result))
	date_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S').split(" ")
	time = date_time[1].split(":")
	hour = "Time_"+str(time[0])+"00"
	cur.execute('SELECT * FROM Urimeter WHERE Patient_Info = ? ', (row,))
	index = 2+int(time[0])
	urine = cur.fetchall();
	row = urine[0]
	newList.append(row[index])
	con.close()
	return render_template('vitals.html',info = newList)
		

@app.route('/<path:path>', methods=['POST'])
def static_proxy(path):
    return send_from_directory('static', path)
		
			
if __name__ == '__main__':
   app.run(debug = True)