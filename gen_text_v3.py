import os
import re
import json
import sys

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
            matches = re.findall(r'\[\[(.*?)\]\]', content)
            # 清理提取的文本，去除首尾空格
            extracted_texts.extend([match.strip()
                                   for match in matches if match.strip()])
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
            processed_files = set()  # Start fresh if file is corrupt
        except Exception as e:
            print(f"错误: 加载已处理文件列表 {done_filepath} 失败: {e}")
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
    # Optional: Track keyword hits for analysis
    # keywords_hit_count = {keyword: 0 for keyword in excluded_keywords}

    for text in texts:
        is_excluded = False
        if not text or text.isspace():
            continue
        for keyword in excluded_keywords:
            # Consider case-insensitivity for Latin suffixes like Inc., Ltd.
            if keyword in ["Inc.", "Ltd.", "Corp.", "LLC", "GmbH", "SA", "PLC", "Co."]:
                # Check if the keyword exists as a whole word or at the end, possibly preceded by a space
                # This is a simple check, regex might be more robust: r'\b' + re.escape(keyword) + r'\b'
                # For simplicity here, we check if it's contained and maybe add space check
                if keyword in text:  # A basic check for now
                    is_excluded = True
                    # keywords_hit_count[keyword] += 1
                    break
            # Standard check for other (Chinese) keywords
            elif keyword in text:
                is_excluded = True
                # keywords_hit_count[keyword] += 1
                break
        if not is_excluded:
            filtered.append(text)
        else:
            excluded_count += 1

    # Optional: Print keyword hit counts
    # ... (code from previous version) ...

    print(f"因包含排除关键词而被过滤掉 {excluded_count} 个文本条目。")
    return filtered

# --- End of Function Definitions ---


def main():
    """
    主函数，遍历当前目录下的 TXT 文件，提取 [[ ]] 文字，
    使用关键词列表进行启发式过滤（尝试排除非科学概念，包括武器类和现代公司），
    去重并输出为 JSON 数组到文件，同时记录已处理文件。
    """
    current_dir = '.'
    done_filepath = os.path.join(current_dir, "processed_files_log.json")
    output_filepath = os.path.join(current_dir, "extracted_concepts.json")

    # --- 配置 ---
    # 定义需要排除的关键词列表
    # !! 注意：这是一个简单的关键词过滤，需要根据实际文本内容仔细调整。 !!
    EXCLUDED_KEYWORDS = [
        # --- 维基百科/MediaWiki 元数据 ---
        "Category:", "分类:", "File:", "文件:", "Image:", "图像:", "Media:",
        "Template:", "模板:", "Help:", "帮助:", "Portal:", "主题:",
        "Wikipedia:", "维基百科:", "User:", "用户:", "Draft:", "草稿:",
        "Special:", "特殊:", "Talk:", "讨论:", "列表", "List of",

        # --- 格式/分隔符 ---
        "|", ":", ".", "·",

        # --- 通用时间/地点/组织 (非公司) ---
        "年", "月", "日",  # "世纪",
        "国家", "省", "市", "县", "镇", "村", "地区", "地理",
        "大学", "政府", "组织", "协会", "学会",  # General orgs, could overlap slightly
        "人物", "历史", "History", "帝国",

        # --- 非科学领域 ---
        "文化", "艺术", "音乐", "电影", "文学", "宗教", "哲学", "政治", "经济", "法律",
        "运动",  # 体育运动或社会运动

        # --- 武器与军事 ---
        "武器", "枪械", "火炮", "炮", "导弹", "炸弹", "弹药", "爆炸物",
        "枪", "步枪", "手枪", "机枪", "冲锋枪", "霰弹枪",
        "榴弹炮", "迫击炮", "手榴弹", "地雷", "鱼雷",
        "剑", "矛",
        "坦克", "战舰", "军舰", "航空母舰", "潜艇", "战斗机", "轰炸机", "无人机","戰車",
        "军事", "战争", "战役", "军队", "海军", "空军", "陆军","瓜分", "出版社","中国","国防",
        "军工", "军事", "国防", "军火", "中國", "工业",

        # --- 现代公司与商业 (新增) ---
        "公司", "企业", "集团", "控股",  # General company terms
        "银行", "保险", "证券", "投资", "金融",  # Finance
        "航空",  # Airlines often end in this
        "软件", "网络", "电子",  # Tech companies often use these, potentially risky overlap
        "汽车",  # Automotive industry
        "制药", "药业",  # Pharma, potentially risky overlap
        "能源", "石油",  # Energy sector
        "传媒", "娱乐", "影业", "游戏",  # Media/Entertainment
        "地产", "房产",  # Real Estate
        "零售", "百货", "超市",  # Retail
        "国际", "环球",  # Often in company names
        "有限", "股份", "株式会社",  # Parts of formal names
        # Common suffixes (Case check added in filter func)
        "Inc.", "Ltd.", "Corp.", "LLC", "GmbH", "SA", "PLC", "Co.",

        # --- 其他可能的噪音 ---
        "en:", "de:", "fr:", "ja:", "zh:",  # Language prefixes
        "LGBT",
        # "页面", "条目", "编辑", "链接",
    ]
    print("--- 配置 ---")
    print(f"已处理文件记录: {done_filepath}")
    print(f"输出文件: {output_filepath}")
    print(f"将使用 {len(EXCLUDED_KEYWORDS)} 个关键词进行过滤 (包含武器军事类和现代公司)...")
    print("-------------\n")

    # --- 处理流程 ---
    # ... (The rest of the main function remains the same as the previous version) ...
    all_extracted_texts = []
    try:
        txt_files = [f for f in os.listdir(
            current_dir) if f.lower().endswith('.txt')]
    except FileNotFoundError:
        print(f"错误: 无法访问目录: {current_dir}")
        sys.exit(1)
    except Exception as e:
        print(f"错误: 扫描目录时发生未知错误: {e}")
        sys.exit(1)

    if not txt_files:
        print("在当前目录下未找到 .txt 文件。")
        return

    processed_files = load_processed_files(done_filepath)

    print(f"找到 {len(txt_files)} 个 .txt 文件。")
    print(f"已加载 {len(processed_files)} 个已处理文件记录。")

    newly_processed_count = 0
    total_extracted_count = 0

    for txt_file in sorted(txt_files):
        if txt_file in processed_files:
            continue

        print(f"处理中: {txt_file} ...")
        filepath = os.path.join(current_dir, txt_file)
        extracted_texts_from_file = extract_bracketed_text(filepath)

        if extracted_texts_from_file:
            count = len(extracted_texts_from_file)
            all_extracted_texts.extend(extracted_texts_from_file)
            total_extracted_count += count

        processed_files.add(txt_file)
        newly_processed_count += 1

        if newly_processed_count % 10 == 0:
            print(f"  ...已处理 {newly_processed_count} 个新文件，保存进度...")
            save_processed_files(processed_files, done_filepath)

    if newly_processed_count > 0:
        save_processed_files(processed_files, done_filepath)
    print(f"\n处理完成。共处理了 {newly_processed_count} 个新文件。")
    print(f"总共提取了 {total_extracted_count} 个 [[...]] 文本（过滤前，未去重）。")

    # --- 过滤和去重 ---
    print("\n开始过滤和去重...")
    filtered_texts = filter_concepts(all_extracted_texts, EXCLUDED_KEYWORDS)
    print(f"关键词过滤后剩余 {len(filtered_texts)} 个文本（未去重）。")

    unique_texts = sorted(list(set(filtered_texts)))
    print(f"去重后剩余 {len(unique_texts)} 个唯一文本。")

    # --- 输出结果 ---
    print(f"\n正在将结果写入 JSON 文件: {output_filepath} ...")
    try:
        with open(output_filepath, 'w', encoding='utf-8') as outfile:
            json.dump(unique_texts, outfile, ensure_ascii=False, indent=4)
        print(f"成功！最终结果已保存到: {output_filepath}")
    except Exception as e:
        print(f"错误: 写入输出文件 {output_filepath} 时发生错误: {e}")


if __name__ == "__main__":
    main()
