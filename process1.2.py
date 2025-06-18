"""
name: 批量修正 OpenWebUI 导出的模型 JSON 文件中 profile_image_url 及 tags 字段的脚本
author: Hardship2495 & Cline
version: 1.2

【功能说明】
- 根据模型 id 中的关键词，自动匹配并修正每个模型的 profile_image_url 字段。
- 根据模型 id 中的关键词，自动匹配并添加相应的 tags (支持多个匹配)。
- profile_image_url: 只在为空、为默认值或错误时才进行替换，已是正确 URL 则跳过。
- tags: 为模型 ID 匹配到的所有适用标签，只在标签不存在时才添加，已存在的标签会被保留。
- 匹配优先级：提供商前缀 > 关键词，顺序可在 PROVIDER_MAP 和 TAG_MAP 中维护 (对 URL 和第一个tag有效，后续tags按出现顺序)。
- 支持详细日志输出，显示每条记录的处理情况。

【使用方法】
1. 自动查找当前目录下最新的 models-export-*.json 文件并覆盖原文件：
   python update_profile_image_url.py

2. 指定输入文件并输出到新文件：
   python update_profile_image_url.py models-export-xxxxxxx.json -o output.json

3. 仅预览详细日志不写入文件（推荐先 dry-run 检查）：
   python update_profile_image_url.py --dry-run

【参数说明】
- json_file      输入的模型配置 JSON 文件路径（可选，默认自动查找最新）
- -o, --output   输出文件名（可选，默认覆盖原文件）
- --dry-run      只预览详细日志，不写入文件

【映射表维护】
- PROVIDER_MAP 为 (关键词，image_url) 的有序列表。
- TAG_MAP 为 (关键词，tag_name) 的有序列表。
- 关键词区分优先级，前缀（如 openrouter/）优先于普通关键词（如 gpt）。
- 若有新模型或新提供商，只需在相应 MAP 中添加对应项即可。

【日志说明】
- [UPDATE URL]   表示已替换的 profile_image_url，显示 id、原始 URL、新 URL
- [SKIP URL]     表示无需替换 profile_image_url 的模型，显示 id 及原因
- [NOT FOUND URL] 未匹配到任何关键词的 profile_image_url，显示 id 及当前 URL
- [TAG ADD]  表示已添加的 tag，显示 id 及添加的 tag name
- [TAG SKIP] 表示无需添加 tag 的模型（因为已存在），显示 id 及 tag name
- [NO TAG MATCH] 未给模型从 TAG_MAP 匹配到任何新 tag (不代表没有 tag, 只是没有从 TAG_MAP 新增)
- [TAG CONVERT]  表示将旧的字符串格式 tag 转换为字典格式

【适用场景】
- OpenWebUI 导出的模型配置批量修正
- 需要统一或纠正 profile_image_url 字段的场景
- 需要根据模型 ID 自动添加或标准化 tags 的场景
"""

import os
import sys
import json
import glob
import argparse

# 1. 维护模型提供商关键词与 image_url 的映射（可随时增删）
PROVIDER_MAP = [
    # (关键词，image_url)，顺序即优先级
    (":free", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/openrouter.svg"),
    ("openrouter/", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/openrouter.svg"),
    ("aliyun/", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/aliyun-color.svg"),
    ("gemini_pipe_new.", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/gemini-color.svg"),
    ("gpt", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/openai.svg"),
    ("openai", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/openai.svg"),
    ("o1-", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/openai.svg"),
    ("o3-", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/openai.svg"),
    ("whisper", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/openai.svg"),
    ("text-embedding-", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/dalle-color.svg"),
    ("embedding-", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/dalle-color.svg"),
    ("tts-", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/dalle-color.svg"),
    ("dall-e", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/dalle-color.svg"),
    ("claude", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/claude-color.svg"),
    ("gemma","https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/gemma-color.svg"),
    ("gemini", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/gemini-color.svg"),
    ("imagen", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/gemini-color.svg"),
    ("learnlm", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/gemini-color.svg"),
    ("ernie", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/wenxin-color.svg"),
    ("baidu", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/wenxin-color.svg"),
    ("command", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/cohere-color.svg"),
    ("cohere", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/cohere-color.svg"),
    ("deepseek", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/deepseek-color.svg"),
    ("grok", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/grok.svg"),
    ("llama", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/meta-color.svg"),
    ("meta", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/meta-color.svg"),
    ("groq", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/groq-color.svg"),
    ("qwq", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/qwen-color.svg"),
    ("qvq", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/qwen-color.svg"),
    ("qwen", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/qwen-color.svg"),
    ("abab", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/minimax-color.svg"),
    ("minimax", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/minimax-color.svg"),
    ("mistral", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/mistral-color.svg"),
    ("kimi", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/kimi-color.svg"),
    ("moonshot", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/kimi-color.svg"),
    ("ollama", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/ollama-color.svg"),
    ("hunyuan", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/hunyuan-color.svg"),
    ("tencent", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/hunyuan-color.svg"),
    ("yi", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/yi-color.svg"),
    ("glm", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/qingyan-color.svg"),
    ("zhipu", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/qingyan-color.svg"),
    ("microsoft", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/microsoft-color.svg"),
    ("BAAI", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/baai.svg"),
    ("flux", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/flux.svg"),
    ("flex", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/flux.svg"),
    ("Kwai","https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/kling-color.svg"),
    ("Reverse","/op/sv/404.svg"),
    ("逆向","/op/sv/2.svg"),
    ("aqa","/op/sv/404.svg"),
]

# 2. 维护模型关键词与 tag 名称的映射（可随时增删）
TAG_MAP = [
    # (关键词，tag_name)，顺序会影响匹配，但所有匹配的都会尝试添加
    # Provider specific tags
    ("openrouter/", "op"),
    (":free", "op"),
    ("aliyun/", "Aliyun"),
    ("openai", "oai"),
    ("o1-", "oai"),
    ("o3-", "oai"),
    ("gpt", "oai"),
    ("claude", "Claude"),
    ("gemini", "gemini"), # gemini will be added
    ("gemma", "gemma"),   # gemma will also be added if ID contains both (or gemma then gemini if ID matches that order)
    ("imagen", "gemini"), # imagen will map to gemini tag
    ("learnlm", "gemini"),# learnlm will map to gemini tag
    ("dall-e", "dall-e"), # Changed to specific tag
    ("whisper", "Whisper"),
    ("tts-", "TTS"),
    ("embedding-", "embedding"),
    ("ernie", "ERNIE"),
    ("baidu", "Baidu"),
    ("command", "Command"),
    ("cohere", "Cohere"),
    ("deepseek", "DS"),
    ("grok", "xai"),
    ("llama", "llama"),
    ("meta", "Meta"),
    ("groq", "Groq"),
    ("qwen", "qwen"),
    ("qwq", "qwen"),
    ("qvq", "qwen"),
    ("minimax", "Minimax"),
    ("mistral", "Mistral"),
    ("kimi", "Kimi"),
    ("moonshot", "Moonshot"),
    ("ollama", "Ollama"),
    ("hunyuan", "HunYuan"),
    ("tencent", "Tencent"),
    ("yi", "Yi"),
    ("glm", "GLM"),
    ("zhipu", "Zhipu"),
    ("phi","phi"),
    ("microsoft", "ms"),
    ("BAAI", "BAAI"),
    ("flux", "Flux"),
    ("flex", "Flux"), # flex will also map to Flux tag
    ("Kwai","Kwai"),
    ("Kolors","Kolors"), # Assuming Kolors is a desired tag
    ("Reverse","Reverse"),
    ("逆向","Reverse"),
    ("aqa","") # Example of an empty tag name if desired
]


DEFAULT_URLS = {"","/static/favicon.png"}

def find_correct_url (model_id: str) -> str:
    """根据 id 匹配对应的 image_url，优先顺序为 PROVIDER_MAP 顺序"""
    id_lower = model_id.lower ()
    for key, url in PROVIDER_MAP:
        if key.lower() in id_lower:
            return url
    return None

def find_matching_tags(model_id: str) -> list[str]:
    """根据 id 匹配所有对应的 tag_name，按 TAG_MAP 顺序"""
    id_lower = model_id.lower()
    found_tags = []
    for key, tag_name in TAG_MAP:
        if key.lower() in id_lower:
            # Add tag_name even if it's empty, but ensure it's not already added from a previous rule
            if tag_name not in found_tags:
                 found_tags.append(tag_name)
    return found_tags


def get_latest_json_file ():
    files = glob.glob ("models-export-*.json")
    if not files:
        print ("未找到 models-export-*.json 文件。")
        sys.exit (1)
    files.sort (key=os.path.getmtime, reverse=True)
    return files [0]

def main ():
    parser = argparse.ArgumentParser (description="批量修正 OpenWebUI 模型 profile_image_url 和 tags")
    parser.add_argument ("json_file", nargs="?", help="模型配置 JSON 文件路径（可选，默认自动查找最新）")
    parser.add_argument ("-o", "--output", help="输出文件名（可选，默认覆盖原文件）")
    parser.add_argument ("--dry-run", action="store_true", help="只预览不写入文件")
    args = parser.parse_args ()

    json_file = args.json_file or get_latest_json_file ()
    try:
        with open (json_file, "r", encoding="utf-8") as f:
            data = json.load (f)
    except FileNotFoundError:
        print(f"错误：输入文件 '{json_file}' 未找到。")
        sys.exit(1)
    except json.JSONDecodeError:
        print(f"错误：文件 '{json_file}' 不是有效的 JSON 格式。")
        sys.exit(1)


    url_updated, url_skipped, url_not_found = 0, 0, 0
    tags_added_count, tags_skipped_count, tags_no_match_model_count = 0, 0, 0 # Renamed for clarity

    url_update_logs = []
    url_skip_logs = []
    url_notfound_logs = []
    tag_add_logs = []
    tag_skip_logs = []
    tag_no_match_logs = []


    for model_idx, model in enumerate(data):
        model_id = model.get ("id", f"未知ID_索引{model_idx}")

        # --- Profile Image URL Update ---
        if "meta" not in model:
            model["meta"] = {}
        meta = model["meta"]
        current_url = meta.get ("profile_image_url", "")
        correct_url = find_correct_url (model_id)

        if correct_url:
            if current_url == correct_url:
                url_skipped += 1
                url_skip_logs.append (f"[SKIP URL] {model_id} 已是正确 URL: {current_url}")
            elif current_url in DEFAULT_URLS or not current_url: # Empty or default
                url_update_logs.append (f"[UPDATE URL] {model_id}\n  原 URL: {current_url or '空'}\n  新 URL: {correct_url}")
                meta ["profile_image_url"] = correct_url
                url_updated += 1
            else: # URL exists, is not default, and not matching our map (likely user-set)
                url_skipped += 1
                url_skip_logs.append (f"[SKIP URL] {model_id} URL 已存在且与映射不符 (保留用户自定义): {current_url}")
        else:
            url_not_found += 1
            url_notfound_logs.append (f"[NOT FOUND URL] {model_id} 未匹配到提供商关键词，当前 URL: {current_url or '空'}")

        # --- Tags Update ---
        if "tags" not in meta or not isinstance(meta["tags"], list):
            meta["tags"] = []

        current_tag_names = set()
        valid_tags_temp = []
        for t in meta["tags"]:
            if isinstance(t, dict) and "name" in t:
                current_tag_names.add(t["name"])
                valid_tags_temp.append(t)
            elif isinstance(t, str): # Handle old string tags
                # Convert string tag to dict format if it's not a duplicate of what will be added
                # or if it's not already present as a dict
                if t not in current_tag_names: # Check if this string tag is already covered by a dict tag
                    current_tag_names.add(t) # Add to current_tag_names to avoid re-adding by map later if same name
                    valid_tags_temp.append({"name": t})
                    tag_add_logs.append(f"[TAG CONVERT] {model_id}: Converted string tag '{t}' to dict format.")
                # If t is already in current_tag_names (as a dict), this string version is a duplicate, skip.
        meta["tags"] = valid_tags_temp


        suggested_tag_names_from_map = find_matching_tags(model_id)
        
        if not suggested_tag_names_from_map:
            tags_no_match_model_count += 1
            tag_no_match_logs.append(f"[NO TAG MATCH] {model_id}: 未从 TAG_MAP 匹配到新标签。现有标签: {[t['name'] for t in meta.get('tags', []) if isinstance(t, dict) and 'name' in t]}")
        else:
            model_had_at_least_one_map_tag_action = False
            for new_tag_candidate in suggested_tag_names_from_map:
                model_had_at_least_one_map_tag_action = True # A tag from map was considered
                if new_tag_candidate not in current_tag_names:
                    meta["tags"].append({"name": new_tag_candidate})
                    tag_add_logs.append(f"[TAG ADD] {model_id}: Added tag '{new_tag_candidate}'")
                    tags_added_count += 1
                    current_tag_names.add(new_tag_candidate) # Add to set to prevent duplicates within this model's processing
                else:
                    tag_skip_logs.append(f"[TAG SKIP] {model_id}: Tag '{new_tag_candidate}' (from ID) already exists.")
                    tags_skipped_count +=1
            # This check is slightly redundant now due to the `if not suggested_tag_names_from_map` block above,
            # but kept for logical completeness if the structure changes.
            # if not model_had_at_least_one_map_tag_action:
            # This case is now covered by the `if not suggested_tag_names_from_map` block


    print (f"处理完成：共 {len (data)} 条模型记录。")
    print ("="*50)
    print ("Profile Image URL 处理结果:")
    print (f"  更新: {url_updated} 条")
    print (f"  跳过: {url_skipped} 条 (已正确或用户自定义)")
    print (f"  未匹配到提供商: {url_not_found} 条")
    print ("-"*50)
    print ("Tags 处理结果:")
    print (f"  新增标签 (实际添加的tag对象数): {tags_added_count}")
    print (f"  跳过新增 (因已存在而跳过的次数): {tags_skipped_count}")
    print (f"  未从映射表匹配到新标签 (模型数): {tags_no_match_model_count}")
    print ("="*50)

    if url_update_logs:
        print ("\nProfile Image URL 更新记录：")
        for log in url_update_logs: print (log)
    if url_skip_logs:
        print ("\nProfile Image URL 跳过记录：")
        for log in url_skip_logs: print (log)
    if url_notfound_logs:
        print ("\nProfile Image URL 未匹配到提供商的模型：")
        for log in url_notfound_logs: print (log)

    print ("="*50)

    if tag_add_logs: # Includes TAG CONVERT
        print ("\nTag 添加/转换记录：")
        for log in tag_add_logs: print (log)
    if tag_skip_logs:
        print ("\nTag 跳过记录 (已存在)：")
        for log in tag_skip_logs: print (log)
    if tag_no_match_logs:
        print ("\nTag 未从映射表匹配新标签的模型：")
        for log in tag_no_match_logs: print (log)

    print ("="*50)

    if not args.dry_run:
        output_file = args.output or json_file
        with open (output_file, "w", encoding="utf-8") as f:
            json.dump (data, f, ensure_ascii=False, indent=2)
        print (f"已写入：{output_file}")
    else:
        print ("dry-run 预览模式，未写入文件。")

if __name__ == "__main__":
    main ()