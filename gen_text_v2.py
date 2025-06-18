import os
import re
import json
import sys

# Functions (extract_bracketed_text, load_processed_files, save_processed_files, filter_concepts)
# remain the same as in the previous good version.
# Assume they are defined above this main function.

# --- [Include the definitions for extract_bracketed_text, load_processed_files, save_processed_files, filter_concepts here] ---

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
            # 当前正则 r'\[\[(.*?)\]\]' 会捕获括号内的所有内容，包括管道符及其后的文本
            # 例如 [[太阳系|Solar System]] 会捕获 "太阳系|Solar System"
            # 如果只想捕获链接目标（管道符前的内容），可以使用: r'\[\[([^|\]]+)(?:\|[^\]]+)?\]\]'
            matches = re.findall(r'\[\[(.*?)\]\]', content)
            # 清理提取的文本，去除首尾空格
            extracted_texts.extend([match.strip() for match in matches if match.strip()])
    except FileNotFoundError:
        print(f"错误: 文件未找到: {filepath}")
    except Exception as e:
        print(f"错误: 读取文件 {filepath} 时发生错误: {e}")
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
                # Handle empty file case
                content = f.read()
                if content:
                    processed_files = set(json.loads(content))
                else:
                    print(f"信息: {done_filepath} 文件为空，将创建新的记录。")
        except json.JSONDecodeError:
             print(f"警告: {done_filepath} 文件格式错误或为空，将忽略旧记录并创建新的记录。")
             # Optionally backup the corrupted file here
             # os.rename(done_filepath, done_filepath + ".corrupted")
             processed_files = set() # Start fresh if file is corrupt
        except Exception as e:
            print(f"错误: 加载已处理文件列表 {done_filepath} 失败: {e}")
            # Decide if you want to continue with an empty set or exit
            # For safety, let's exit if we can't load the state reliably
            # sys.exit(f"无法加载处理状态，程序中止。")
            print("警告: 无法加载处理状态，将作为首次运行处理所有文件。")
            processed_files = set()
    return processed_files

def save_processed_files(processed_files, done_filepath="done.json"):
    """
    将已处理的文件列表保存到 done.json 文件。

    Args:
        processed_files: 已处理文件名的集合。
        done_filepath: 保存状态的文件路径。
    """
    try:
        # Convert set to list for JSON serialization
        with open(done_filepath, 'w', encoding='utf-8') as f:
            json.dump(list(processed_files), f, ensure_ascii=False, indent=4)
        # Print message only when saving the final state, not during intermediate saves
    except Exception as e:
        print(f"错误: 保存已处理文件列表 {done_filepath} 失败: {e}")

def filter_concepts(texts, excluded_keywords):
    """
    根据关键词列表过滤文本。

    Args:
        texts: 待过滤的文本列表。
        excluded_keywords: 不应包含的关键词列表。

    Returns:
        过滤后的文本列表。
    """
    filtered = []
    excluded_count = 0
    keywords_hit_count = {keyword: 0 for keyword in excluded_keywords} # Track hits per keyword

    for text in texts:
        is_excluded = False
        # 跳过空字符串或仅包含空格的字符串
        if not text or text.isspace():
            continue
        # 检查是否包含任何排除的关键词
        for keyword in excluded_keywords:
            # 使用 case-insensitive 匹配可能更鲁棒，但对于中文和特定术语可能不适用
            # if keyword.lower() in text.lower():
            if keyword in text:
                is_excluded = True
                keywords_hit_count[keyword] += 1 # Increment count for this keyword
                break # 找到一个排除词，无需继续检查此文本
        if not is_excluded:
            filtered.append(text)
        else:
            excluded_count += 1

    # Optional: Print keyword hit counts for analysis
    # print("\n--- Keyword Hit Counts ---")
    # for keyword, count in sorted(keywords_hit_count.items(), key=lambda item: item[1], reverse=True):
    #     if count > 0:
    #         print(f"'{keyword}': {count}")
    # print("------------------------\n")

    print(f"因包含排除关键词而被过滤掉 {excluded_count} 个文本条目。")
    return filtered

# --- End of Function Definitions ---


def main():
    """
    主函数，遍历当前目录下的 TXT 文件，提取 [[ ]] 文字，
    使用关键词列表进行启发式过滤（尝试排除非科学概念，包括武器类），
    去重并输出为 JSON 数组到文件，同时记录已处理文件。
    """
    current_dir = '.'
    done_filepath = os.path.join(current_dir, "processed_files_log.json") # 更明确的文件名
    output_filepath = os.path.join(current_dir, "extracted_concepts.json") # 更明确的文件名

    # --- 配置 ---
    # 定义需要排除的关键词列表 (用于启发式过滤非科学/元数据/通用概念)
    # !! 注意：这是一个简单的关键词过滤，无法真正理解语义。它可能会错误地排除相关概念 !!
    # !! 或未能排除所有不相关的概念。需要根据实际文本内容仔细调整。 !!
    EXCLUDED_KEYWORDS = [
        # 维基百科/MediaWiki 常用前缀/元数据/管理页面
        "Category:", "分类:", "File:", "文件:", "Image:", "图像:", "Media:",
        "Template:", "模板:", "Help:", "帮助:", "Portal:", "主题:",
        "Wikipedia:", "维基百科:", "User:", "用户:", "Draft:", "草稿:",
        "Special:", "特殊:", "Talk:", "讨论:",

        # 常见分隔符或格式错误指示
        "|", ":",

        # 通用时间/地点/组织/度量衡 (可能过于宽泛，根据需要调整)
        "年", "月", "日", # 年份/日期通常不是核心科学概念
        #"世纪", # Century
        "列表", "List of", # 通常是元页面
        "国家", "省", "市", "县", "镇", "村", # 地理位置
        "公司", "组织", "大学", "政府", # 组织机构
        "人物", # 人物传记
        "历史", "History", # 'History of Science' 可能被误伤
        "文化", "艺术", "音乐", "电影", "文学", "宗教", "哲学", "政治", "经济", "法律", # 非科学领域
        "运动", # 体育运动或社会运动

        # 武器与军事相关 (新增)
        "武器", "枪械", "火炮", "炮", "导弹", "炸弹", "弹药", "爆炸物", # General Weapon/Ammo
        "枪", "步枪", "手枪", "机枪", "冲锋枪", "霰弹枪", # Firearms
        "榴弹炮", "迫击炮", # Artillery Types
        "手榴弹", "地雷", "鱼雷", # Specific Explosives/Ordnance
        "剑", "矛", # Melee Weapons
        "坦克", "战舰", "军舰", "航空母舰", "潜艇", "战斗机", "轰炸机", # Weapon Platforms/Military Vehicles
        "军事", "战争", "战役", "军队", "海军", "空军", "陆军", # Military Concepts/Organizations

        # 可能的语言标记或不完整片段
        "en:", "de:", "fr:", "ja:", "zh:", # 语言前缀
        ".", # 单独的点，可能来自文件名或不完整条目
        "·", # 中文名称分隔符

        # 数值单位 (单独出现时可能意义不大)
        #"千米", "米", "厘米", "公斤", "克", "秒", "分钟", "小时",

        # 根据实际观察到的噪音添加更多词汇...
        #"页面", "条目", "编辑", "链接",
        "帝国", # Often historical/political context
    ]
    print("--- 配置 ---")
    print(f"已处理文件记录: {done_filepath}")
    print(f"输出文件: {output_filepath}")
    print(f"将使用 {len(EXCLUDED_KEYWORDS)} 个关键词进行过滤 (包含武器军事类)...")
    # print(f"部分排除关键词示例: {EXCLUDED_KEYWORDS[:5] + EXCLUDED_KEYWORDS[-5:]}...") # Show some from start and end
    print("-------------\n")


    # --- 处理流程 ---
    all_extracted_texts = []
    try:
        txt_files = [f for f in os.listdir(current_dir) if f.lower().endswith('.txt')]
    except FileNotFoundError:
        print(f"错误: 无法访问目录: {current_dir}")
        sys.exit(1)
    except Exception as e:
        print(f"错误: 扫描目录时发生未知错误: {e}")
        sys.exit(1)


    if not txt_files:
        print("在当前目录下未找到 .txt 文件。")
        return # 或者 sys.exit(0)

    processed_files = load_processed_files(done_filepath)

    print(f"找到 {len(txt_files)} 个 .txt 文件。")
    print(f"已加载 {len(processed_files)} 个已处理文件记录。")

    newly_processed_count = 0
    total_extracted_count = 0

    for txt_file in sorted(txt_files): # Sort for consistent processing order
        if txt_file in processed_files:
            continue # Skip already processed files silently

        print(f"处理中: {txt_file} ...")
        filepath = os.path.join(current_dir, txt_file)
        extracted_texts_from_file = extract_bracketed_text(filepath)

        if extracted_texts_from_file: # Only add if extraction yielded results
             count = len(extracted_texts_from_file)
             all_extracted_texts.extend(extracted_texts_from_file)
             total_extracted_count += count
             # print(f"  提取了 {count} 个条目。") # Uncomment for more verbose output

        # Mark as processed *after* successful attempt (even if no text was found)
        processed_files.add(txt_file)
        newly_processed_count += 1

        # Save progress incrementally (more robust to interruption)
        if newly_processed_count % 10 == 0: # Save every 10 files to balance robustness and I/O overhead
             print(f"  ...已处理 {newly_processed_count} 个新文件，保存进度...")
             save_processed_files(processed_files, done_filepath)

    # Final save of processed files list after the loop finishes
    if newly_processed_count > 0: # Only save if new files were processed
        save_processed_files(processed_files, done_filepath)
    print(f"\n处理完成。共处理了 {newly_processed_count} 个新文件。")
    print(f"总共提取了 {total_extracted_count} 个 [[...]] 文本（过滤前，未去重）。")

    # --- 过滤和去重 ---
    print("\n开始过滤和去重...")
    # 使用定义的函数进行过滤
    filtered_texts = filter_concepts(all_extracted_texts, EXCLUDED_KEYWORDS)
    # The filter_concepts function now prints how many items were excluded by keywords
    print(f"关键词过滤后剩余 {len(filtered_texts)} 个文本（未去重）。")

    # 去重 (对过滤后的文本进行去重) 并排序
    unique_texts = sorted(list(set(filtered_texts)))
    print(f"去重后剩余 {len(unique_texts)} 个唯一文本。")

    # --- 输出结果 ---
    print(f"\n正在将结果写入 JSON 文件: {output_filepath} ...")
    try:
        with open(output_filepath, 'w', encoding='utf-8') as outfile:
            # Use json.dump for direct writing to file, efficient for large lists
            json.dump(unique_texts, outfile, ensure_ascii=False, indent=4)
        print(f"成功！最终结果已保存到: {output_filepath}")
    except Exception as e:
        print(f"错误: 写入输出文件 {output_filepath} 时发生错误: {e}")

if __name__ == "__main__":
    main()