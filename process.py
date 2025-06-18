"""
name: 批量修正 OpenWebUI 导出的模型 JSON 文件中 profile_image_url 字段的脚本
author: Hardship2495 & Cline 
version: 1.0

【功能说明】
- 根据模型 id 中的关键词，自动匹配并修正每个模型的 profile_image_url 字段。
- 只在 profile_image_url 为空、为默认值或错误时才进行替换，已是正确 URL 则跳过。
- 匹配优先级：提供商前缀 > 关键词，顺序可在 PROVIDER_MAP 中维护。
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
- PROVIDER_MAP 为 (关键词，image_url) 的有序列表，支持随时增删和调整优先级。
- 关键词区分优先级，前缀（如 openrouter/）优先于普通关键词（如 gpt）。
- 若有新模型或新提供商，只需在 PROVIDER_MAP 中添加对应项即可。

【日志说明】
- [UPDATE]   表示已替换的模型，显示 id、原始 URL、新 URL
- [SKIP]     表示无需替换的模型，显示 id 及原因
- [NOT FOUND] 未匹配到任何关键词的模型，显示 id 及当前 URL

【适用场景】
- OpenWebUI 导出的模型配置批量修正
- 需要统一或纠正 profile_image_url 字段的场景

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
    ("gemma", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/gemma-color.svg"),
    ("flux", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/flux.svg"),
    ("BAAI", "https://registry.npmmirror.com/@lobehub/icons-static-svg/latest/files/icons/baai.svg"),
]

DEFAULT_URLS = {"","/static/favicon.png"}

def find_correct_url (model_id: str) -> str:
    """根据 id 匹配对应的 image_url，优先顺序为 PROVIDER_MAP 顺序"""
    id_lower = model_id.lower ()
    for key, url in PROVIDER_MAP:
        if key in id_lower:
            return url
    return None

def get_latest_json_file ():
    files = glob.glob ("models-export-*.json")
    if not files:
        print ("未找到 models-export-*.json 文件。")
        sys.exit (1)
    files.sort (key=os.path.getmtime, reverse=True)
    return files [0]

def main ():
    parser = argparse.ArgumentParser (description="批量修正 OpenWebUI 模型 profile_image_url")
    parser.add_argument ("json_file", nargs="?", help="模型配置 JSON 文件路径（可选，默认自动查找最新）")
    parser.add_argument ("-o", "--output", help="输出文件名（可选，默认覆盖原文件）")
    parser.add_argument ("--dry-run", action="store_true", help="只预览不写入文件")
    args = parser.parse_args ()

    json_file = args.json_file or get_latest_json_file ()
    with open (json_file, "r", encoding="utf-8") as f:
        data = json.load (f)

    updated, skipped, not_found = 0, 0, 0
    update_logs = []
    skip_logs = []
    notfound_logs = []

    for model in data:
        model_id = model.get ("id", "")
        # Check if 'meta' key exists, if not, add it
        if "meta" not in model:
            model["meta"] = {}
        # Now get the meta dictionary (guaranteed to exist)
        meta = model["meta"]
        current_url = meta.get ("profile_image_url", "")
        correct_url = find_correct_url (model_id)
        if correct_url:
            if current_url == correct_url:
                skipped += 1
                skip_logs.append (f"[SKIP] {model_id} 已是正确 URL: {current_url}")
            elif current_url in DEFAULT_URLS or not current_url:
            #else:
                update_logs.append (f"[UPDATE] {model_id}\n  原 URL: {current_url}\n  新 URL: {correct_url}")
                meta ["profile_image_url"] = correct_url
                updated += 1
            else:
                skipped += 1
                skip_logs.append (f"[SKIP] {model_id} URL 已存在且与映射不符: {current_url}")
        else:
            not_found += 1
            notfound_logs.append (f"[NOT FOUND] {model_id} 未匹配到任何提供商关键词，当前 URL: {current_url}")

    print (f"处理完成：共 {len (data)} 条，更新 {updated} 条，跳过 {skipped} 条，未匹配到提供商 {not_found} 条。")
    print ("="*40)
    if update_logs:
        print ("更新记录：")
        for log in update_logs:
            print (log)
    if skip_logs:
        print ("\n 跳过记录：")
        for log in skip_logs:
            print (log)
    if notfound_logs:
        print ("\n 未匹配到提供商的模型：")
        for log in notfound_logs:
            print (log)
    print ("="*40)

    if not args.dry_run:
        output_file = args.output or json_file
        with open (output_file, "w", encoding="utf-8") as f:
            json.dump (data, f, ensure_ascii=False, indent=2)
        print (f"已写入：{output_file}")
    else:
        print ("dry-run 预览模式，未写入文件。")

if __name__ == "__main__":
    main ()