import json
import requests

base = 'http://122.116.109.94:11434/'

url = base + 'api/ps'
sp = requests.get(url)
sp = json.loads(sp.text)
result = [
    {"id": model["model"], "name": model["name"]}
    for model in sp['models']
]