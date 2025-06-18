import re
import json
from bs4 import BeautifulSoup
from urllib.request import urlopen
from urllib.parse import quote, unquote
import concurrent.futures  # 导入线程池模块
import threading # 导入 threading 模块，用于线程锁

# 全局锁，用于保护对 done_titles 和 done.json 的并发访问
done_titles_lock = threading.Lock()
done_titles = [] # 全局变量，存储已完成的标题，需要线程安全地访问

def scrape_wikipedia_textarea(url, output_filename="output.txt"):
    try:
        response = urlopen(url)
        html = response.read().decode('utf-8')

        soup = BeautifulSoup(html, 'html.parser')
        textarea = soup.find('textarea', {'id': 'wpTextbox1'})

        if textarea:
            content = textarea.text
            # 检查是否是重定向
            redirect_match = re.match(r'#REDIRECT\s*\[\[([^\]]+)\]\]', content, re.IGNORECASE)
            if redirect_match:
                redirect_title = redirect_match.group(1)
                print(f"线程 {threading.current_thread().name}: 发现重定向到: {redirect_title}")
                return redirect_title # 返回重定向的目标标题

            with open(output_filename, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"线程 {threading.current_thread().name}: 内容保存到 {output_filename}")
            return None # 没有重定向
        else:
            print(f"线程 {threading.current_thread().name}: 未在 URL: {url} 的页面上找到 id 为 'wpTextbox1' 的 textarea")
            return None

    except Exception as e:
        print(f"线程 {threading.current_thread().name}: 抓取 URL: {url} 时出错 - {e}")
        return None

def process_title(title): # 将处理单个标题的逻辑提取到函数中
    global done_titles # 声明使用全局变量 done_titles
    current_title = title # 记录当前处理的标题，用于文件名
    while True: # 循环处理重定向
        # 使用 quote 对 title 进行 url 编码
        encoded_title = quote(current_title)
        url = "https://zh.wikipedia.org/w/index.php?title="+encoded_title+"&action=edit"

        # 创建基于标题的输出文件名，使用原始标题 (第一次的标题)
        output_filename = title + ".txt" # 使用最初的 title 作为文件名
        redirect_target = scrape_wikipedia_textarea(url, output_filename)

        if redirect_target:
            current_title = redirect_target # 更新为重定向的目标标题，继续抓取
            print(f"线程 {threading.current_thread().name}: 继续抓取重定向目标: {current_title}")
        else:
            with done_titles_lock: # 获取锁
                if title not in done_titles: # 再次检查，防止重复添加 (虽然理论上不应该发生，但作为一种保护)
                    done_titles.append(title) # 添加原始标题到已下载列表
                    try:
                        with open("done.json", 'w', encoding='utf-8') as f:
                            json.dump(done_titles, f, ensure_ascii=False, indent=4)
                        print(f"线程 {threading.current_thread().name}: 标题 '{title}' 已添加到 done.json")
                    except Exception as e:
                        print(f"线程 {threading.current_thread().name}: 保存 done.json 文件时发生错误: {e}")
            break # 没有重定向或者抓取成功，退出循环

def main():
    global done_titles # 声明使用全局变量 done_titles
    try:
        with open("done.json", 'r', encoding='utf-8') as f:
            done_titles = json.load(f)
        print(f"已加载 {len(done_titles)} 个已下载标题。")
    except FileNotFoundError:
        print("done.json 文件未找到，将创建一个新的 done.json 文件。")
    except json.JSONDecodeError:
        print("错误: done.json 文件内容格式不正确，无法解析为 JSON。将创建一个新的 done.json 文件。")
        done_titles = []
    except Exception as e:
        print(f"读取 done.json 文件时发生错误: {e}")
        return

    try:
        with open("output.json", 'r', encoding='utf-8') as f:
            titles = json.load(f)
    except FileNotFoundError:
        print("错误: 找不到 output.json 文件。请确保文件存在于同一目录下，并且已经运行过之前的脚本生成该文件。")
        return
    except json.JSONDecodeError:
        print("错误: output.json 文件内容格式不正确，无法解析为 JSON。请检查文件内容是否为 JSON 数组。")
        return
    except Exception as e:
        print(f"读取 output.json 文件时发生错误: {e}")
        return

    filtered_titles = [title for title in titles if title not in done_titles] # 过滤掉已完成的标题
    print(f"待处理的标题数量: {len(filtered_titles)}")

    with concurrent.futures.ThreadPoolExecutor(max_workers=8) as executor: # 设置线程池大小，例如 8
        future_to_title = {executor.submit(process_title, title): title for title in filtered_titles} # 提交任务到线程池

        for future in concurrent.futures.as_completed(future_to_title): # 遍历已完成的 Future
            title = future_to_title[future]
            try:
                future.result() # 获取结果，如果发生异常，会在这里抛出
                # print(f"标题 '{title}' 处理完成。") # 可以选择性输出，process_title 内部已经有输出了
            except Exception as exc:
                print(f"处理标题 '{title}' 时发生异常: {exc}")

    print("所有标题处理完成。")

if __name__ == "__main__":
    main()