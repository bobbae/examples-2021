import scrapy
import json

class Lacovidbot(scrapy.Spider):
    name = 'lacovidbot'
    allowed_domains = ['www.ph.lacounty.gov/media/Coronavirus/locations.htm']
    start_urls = ['http://www.ph.lacounty.gov/media/Coronavirus/locations.htm']

    #def start_requests(self):
    #    self.alogger.info("lacovidbot: start_requests")
    #    yield scrapy.Request(start_urls[0], self.parse)

    def parse(self, response):
        self.alogger.info("lacovidbot: parse")
        cities = response.xpath('//table/tr/td[1]').extract()
        cases = response.xpath('//table/tr/td[2]').extract()
        rates = response.xpath('//table/tr/td[3]').extract()
        cities = list(map(lambda x: x.replace('<td>','').replace('</td>',''), cities))
        cases = list(map(lambda x: x.replace('<td>','').replace('</td>',''), cases))
        rates = list(map(lambda x: x.replace('<td>','').replace('</td>',''), rates))
        i = 0
        for city in cities:
            arec = { 'city': city, 'cases': cases[i] , 'rate':rates[i]}
            if i>300:
                break
            i=i+1
            self.alist.append(arec)
        print(self.alist)

    def store(self):
        self.alogger.info('writing the json file')
        with open('/tmp/lacovidinfo.json', 'w') as jsonfile:
            json.dumps(self.alist, jsonfile, indent=4)


