import base64
import requests

s = 'app="Ollama" && country="UK" && is_domain=false'
encoded_content = base64.b64encode(s.encode('utf-8')).decode('utf-8')
url = f"https://fofa.info/result?qbase64={encoded_content}"
user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
try:
    headers = {'User-Agent': user_agent}
    response = requests.get(url, headers=headers)
    response.raise_for_status()
except requests.RequestException as e:
    print(f"请求失败: {str(e)}")
    exit(1)

start_tag = 'hsxa-host"><a href="'
end_tag = '"'

matches = []
start_index = 0
while True:
    start = response.text.find(start_tag, start_index)
    if start == -1: 
        break
    end = response.text.find(end_tag, start + len(start_tag))
    if end == -1:
        break
    matches.append(response.text[start + len(start_tag):end])
    start_index = end

with open('result.txt', 'a', encoding='utf-8') as file:
    for match in matches:
        file.write(f"{match}\n")
        print(match)

print("已保存：result.txt")