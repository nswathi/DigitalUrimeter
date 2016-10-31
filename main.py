# Copyright 2016 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import webapp2
from google.appengine.ext.webapp import template
import urllib
import webbrowser
from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

#fhand = urllib.urlopen('http://www.dr-chuck.com/page1.htm')
#fhand = open('index.html')
#readData = fhand.read();
#webbrowser.open_new("index.html")
#class MainPage(webapp2.RequestHandler):
#    def get(self):
#        self.response.headers['Content-Type'] = 'text/html'
 #       self.response.write(readData)


#app = webapp2.WSGIApplication([
#    ('/', MainPage),
#], debug=True)

#class IndexHandler(webapp.RequestHandler):
#    def get(self):
#        self.redirect('static/index.html')


#def main():
#	app = webapp.WSGIApplication([
#        ], debug=True)
#	run_wsgi_app(app)