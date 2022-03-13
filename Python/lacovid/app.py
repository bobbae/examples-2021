from bot.lacovidbot import Lacovidbot
from flask import (Flask,render_template,request,Response,jsonify)
from scrapy.crawler import CrawlerRunner
import logging
import crochet
import json
from logging.handlers import RotatingFileHandler

crochet.setup()

app = Flask(__name__)

crawl_runner = CrawlerRunner()

lacovidlist= []
scrape_in_progress=False
scrape_complete=False
logging.basicConfig(level=logging.DEBUG)

empty_data={"data": [{"city": "not ready", "cases":0,"rate":0}]}

handler = RotatingFileHandler('lacovid.log',maxBytes=10000,backupCount=1)
handler.setLevel(logging.DEBUG)
app.logger.addHandler(handler)
app.logger.info('starting')

@app.route('/')
def myindex():
    app.logger.info("myindex")
    return render_template("index.html",data=json.dumps(empty_data), flask_token="LA covid-19 info")
    
@app.route('/table')
def rendertable():
    app.logger.info("table")
    global lacovidlist
    data={ "data": filterres(lacovidlist)}
    return render_template("index.html", data=json.dumps(data),flask_token="la covid info")

@app.route('/crawl')
def crawl_for_covidinfo():
    global scrape_in_progress
    global scrape_complete

    app.logger.info("crawl")
    if not scrape_in_progress:
        scrape_in_progress=True
        global lacovidlist
        scrape_with_crochet(lacovidlist)
        app.logger.info("scraping")
        return 'SCRAPING'
    elif scrape_complete:
        app.logger.info("scrape complete")
        return 'SCRAPE_COMPLETE'
    app.logger.info("scrape in progress")
    return 'SCRAPE_IN_PROGRESS'

@app.route('/results')
def results():
    global scrape_complete
    global lacovidlist
    app.logger.info("results")
    if scrape_complete:
        return json.dumps({ "data": filterres(lacovidlist)})
    if scrape_in_progress:
        app.logger.info("work in progress")
        return 'Work in progress'
    return 'Run crawler first'

def filterres(olist):
    nlist =[]
    for item in olist:
        if item['city'].startswith('Los Angeles -') or item['city'].startswith('City of '):
            nlist.append(item)
    return nlist

def filtersummary(olist):
    nlist =[
      { 'info': 'new daily cases', 'details': olist[0]['cases'] },
      { 'info': 'new daily deaths', 'details': olist[1]['cases'] },
      { 'info': 'total cases', 'details': olist[4]['cases'] },
      { 'info': 'total cases in LA county', 'details': olist[5]['cases'] },
      { 'info': 'total cases in long beach', 'details': olist[6]['cases'] },
      { 'info': 'total cases in pasadena', 'details': olist[7]['cases'] },
      { 'info': 'total deaths', 'details': olist[9]['cases'] },
      { 'info': 'total deaths in LA county', 'details': olist[10]['cases'] },
      { 'info': 'total deaths in long beach', 'details': olist[11]['cases'] },
      { 'info': 'total deaths in pasadena', 'details': olist[12]['cases'] },
      { 'info': 'age 0 to 17', 'details': olist[14]['cases'] },
      { 'info': 'age 18 to 40', 'details': olist[15]['cases'] },
      { 'info': 'age 41 to 65', 'details': olist[16]['cases'] },
      { 'info': 'age over 65', 'details': olist[17]['cases'] },
      { 'info': 'gender - female', 'details': olist[20]['cases']},
      { 'info': 'gender - male', 'details': olist[21]['cases']},
      { 'info': 'gender - other', 'details': olist[22]['cases']},
      { 'info': 'race - american indian/alaska native', 'details': olist[26]['cases'] },
      { 'info': 'race - asian', 'details': olist[27]['cases'] },
      { 'info': 'race - black', 'details': olist[28]['cases'] },
      { 'info': 'race - hispanic/latino', 'details': olist[29]['cases'] },
      { 'info': 'race - native hawaiian/pacific islander', 'details': olist[30]['cases'] },
      { 'info': 'race - white', 'details': olist[31]['cases'] },
      { 'info': 'race - other', 'details': olist[32]['cases'] }
    ]

    return nlist
    
@app.route('/get_results')
def get_results():
    app.logger.info("get results")
    global scrape_complete
    global empty_data
    if scrape_complete:
        data={ "data": filterres(lacovidlist)}
        return jsonify(data)
    return empty_data

@app.route('/get_summary')
def get_summary():
    app.logger.info("get summary")
    global scrape_complete
    global empty_data
    if scrape_complete:
        data={ "data": filtersummary(lacovidlist)}
        return jsonify(data)
    return empty_data

@crochet.run_in_reactor
def scrape_with_crochet(_list):
    app.logger.info("start crawler")
    eventual = crawl_runner.crawl(Lacovidbot, alist = _list, alogger = app.logger)
    eventual.addCallback(finished_scrape)

def finished_scrape(null):
    global scrape_complete
    scrape_complete=True
    app.logger.info("scrape complete")

@app.route('/message', methods = ['POST'])
def message_post():
    app.logger.info("post message")
    if "Content-Type" in request.headers:
        pass
    else:
        return "No content type"
    if request.headers['Content-Type'] == 'text/plain':
        msg = request.data
        return "Text message: " + msg
    if request.headers['Content-Type'] == 'application/json':
        msg = request.json
        return "JSON message: " + json.dumps(msg)
    if request.headers['Content-Type'] == 'application/octet-stream':
        f = open('./binarydata.bin', 'wb')
        f.write(request.data)
        f.close()
        return 'Binary message written'
    return '415 unsupported media type'

@app.route('/start_crawling', methods=['POST'])
def start_crawling():
    app.logger.info("start crawling")
    crawl_for_covidinfo()
    resp = Response('OK',status=200, mimetype='application/json')
    return resp

if __name__=='__main__':
    app.run(threaded=True, port=5000)
