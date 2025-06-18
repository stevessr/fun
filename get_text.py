import os
import re
import json

def extract_bracketed_text(filepath):
    """
    从 TXT 文件中读取 [[ ]] 包裹的文字。

    Args:
        filepath: TXT 文件路径。

    Returns:
        包含提取文字的列表。
    """
    extracted_texts = []
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
            # 使用正则表达式查找 [[...]] 模式的文字
            matches = re.findall(r'\[\[(.*?)\]\]', content)
            extracted_texts.extend(matches)
    except FileNotFoundError:
        print(f"文件未找到: {filepath}")
    except Exception as e:
        print(f"读取文件 {filepath} 时发生错误: {e}")
    return extracted_texts

def load_processed_files(done_filepath="done.json"):
    """
    从 done.json 文件加载已处理的文件列表。

    Returns:
        已处理文件名的集合，如果文件不存在或加载失败则返回空集合。
    """
    processed_files = set()
    if os.path.exists(done_filepath):
        try:
            with open(done_filepath, 'r', encoding='utf-8') as f:
                processed_files = set(json.load(f))
        except Exception as e:
            print(f"加载已处理文件列表 {done_filepath} 失败: {e}")
    return processed_files

def save_processed_files(processed_files, done_filepath="output.json"):
    """
    将已处理的文件列表保存到 done.json 文件。

    Args:
        processed_files: 已处理文件名的集合。
    """
    try:
        with open(done_filepath, 'w', encoding='utf-8') as f:
            json.dump(list(processed_files), f, ensure_ascii=False, indent=4)
        print(f"已处理文件列表已更新到 {done_filepath}")
    except Exception as e:
        print(f"保存已处理文件列表 {done_filepath} 失败: {e}")

def main():
    """
    主函数，遍历当前目录下的所有 TXT 文件，提取文字并去重输出为 JSON 数组到文件，排除包含特定词汇的文本，并记录已处理文件。
    """
    all_extracted_texts = []
    txt_files = [f for f in os.listdir('.') if f.endswith('.txt')] # 获取当前目录下所有 txt 文件

    done_filepath = "done.json"
    processed_files = load_processed_files(done_filepath) # 加载已处理文件列表

    for txt_file in txt_files:
        if txt_file in processed_files:
            print(f"文件 {txt_file} 已处理，跳过。")
            continue # 跳过已处理文件

        filepath = os.path.join('.', txt_file) # 构建完整文件路径
        extracted_texts = extract_bracketed_text(filepath)
        all_extracted_texts.extend(extracted_texts)

        processed_files.add(txt_file) # 将当前文件添加到已处理列表
        save_processed_files(processed_files, done_filepath) # 保存已处理文件列表

    # 定义需要排除的词汇列表 (可以根据需要添加更多词汇)
    excluded_words = ["Category", "File","Image","|",":",".","年","运动","帝國","国家"]
    filtered_texts = []

    # 过滤包含排除词汇的文本
    for text in all_extracted_texts:
        is_excluded = False
        for word in excluded_words:
            if word in text:
                is_excluded = True
                break # 如果找到一个排除词汇，就跳出内循环
        if not is_excluded:
            filtered_texts.append(text)

    # 去重 (对过滤后的文本进行去重)
    unique_texts = sorted(list(set(filtered_texts)))

    # 输出为 JSON 数组到文件
    json_output = json.dumps(unique_texts, ensure_ascii=False, indent=4) # ensure_ascii=False 支持中文, indent=4 格式化输出
    output_filepath = "output.json"  # 设置输出 JSON 文件的路径
    try:
        with open(output_filepath, 'w', encoding='utf-8') as outfile:
            outfile.write(json_output)
        print(f"JSON 数组已输出到文件: {output_filepath}")
    except Exception as e:
        print(f"写入文件 {output_filepath} 时发生错误: {e}")


if __name__ == "__main__":
    main()