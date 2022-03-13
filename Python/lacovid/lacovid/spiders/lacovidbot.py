# -*- coding: utf-8 -*-
import scrapy
import json

class LacovidbotSpider(scrapy.Spider):
    name = 'lacovidbot'
    allowed_domains = ['www.ph.lacounty.gov/media/Coronavirus/locations.htm']
    start_urls = ['http://www.ph.lacounty.gov/media/Coronavirus/locations.htm']

    def parse(self, response):
        cities = response.xpath('//table/tr/td[1]').extract()
        cases = response.xpath('//table/tr/td[2]').extract()
        rates = response.xpath('//table/tr/td[3]').extract()
        cities = list(map(lambda x: x.replace('<td>','').replace('</td>',''), cities))
        cases = list(map(lambda x: x.replace('<td>','').replace('</td>',''), cases))
        rates = list(map(lambda x: x.replace('<td>','').replace('</td>',''), rates))
        alist=[]
        lacitiesdata = { "lacitiescovid": alist }
        i = 0
        for city in cities:
            if i<28:
                i=i+1
                continue
            arec = { 'city': city, 'cases': cases[i] , 'rate':rates[i]}
            i=i+1
            alist.append(arec)
        with open('lacovidinfo.json', 'w') as jsonfile:
            json.dump(lacitiesdata, jsonfile, indent=4)

