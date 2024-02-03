#!/usr/bin/python3

import http.client
import json

#conn = http.client.HTTPSConnection('192.168.231.129')
#conn = http.client.HTTPConnection('192.168.231.129')
conn = http.client.HTTPConnection('192.168.1.116')
headers = {'Content-type': 'application/json'}

data = {"id": "test1234", "key": "123456", "data": {"temp1": 12.3, "tepm2": 23.4, "tup": 5678}}
json_data = json.dumps(data)

conn.request('POST', '/thing.php', json_data, headers)
response = conn.getresponse()

print(response.read().decode())
