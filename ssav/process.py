import json
import aiohttp
import asyncio
import csv
import logging
from tqdm import tqdm
import time
from datetime import datetime

# 配置日志模块
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s",
    handlers=[
        logging.FileHandler("./spider.log"),  # 将日志写入文件
        logging.StreamHandler()  # 同时输出到终端
    ]
)


# 读取txt文件中的URL列表
def read_urls_from_file(file_path):
    try:
        with open(file_path, 'r') as file:
            urls = file.readlines()
        logging.info(f"成功读取 {len(urls)} 个URL")
        return [url.strip() + '/api/tags' for url in urls]
    except Exception as e:
        logging.error(f"读取文件失败: {e}")
        raise


# 处理单个URL（异步请求）
async def process_single_url(session, url, semaphore, max_retries=1, timeout=2):
    retries = 0
    while retries <= max_retries:
        try:
            async with semaphore:  # 控制并发数
                start_time = time.time()
                async with session.get(url, timeout=timeout) as response:
                    elapsed_time = time.time() - start_time
                    logging.info(f"请求 {url} 成功，耗时 {elapsed_time:.2f} 秒")
                    response.raise_for_status()  # 检查请求是否成功
                    data = await response.json()

                    # 整理返回值
                    models_info = []
                    for model in data.get("models", []):
                        # 确保 families 是可迭代对象
                        families = model["details"].get("families", [])
                        if not isinstance(families, (list, tuple)):
                            families = [str(families)]  # 如果不是可迭代对象，转换为单元素列表

                        model_info = [
                            model["name"],
                            model["model"],
                            model["modified_at"],
                            model["size"],
                            model["digest"],
                            model["details"]["family"],
                            ",".join(families),  # 确保 families 是可迭代对象
                            model["details"]["parameter_size"],
                            model["details"]["quantization_level"]
                        ]
                        models_info.append(model_info)

                    # 将每个模型信息与 URL 组合
                    return [[url] + model for model in models_info]
        except aiohttp.ClientError as e:
            logging.warning(f"请求 {url} 失败（ClientError），重试次数 {retries}/{max_retries}: {e}")
        except asyncio.TimeoutError:
            logging.warning(f"请求 {url} 超时，重试次数 {retries}/{max_retries}")
        except Exception as e:
            logging.error(f"处理 {url} 时发生未知错误: {e}")

        retries += 1
        if retries > max_retries:
            logging.error(f"请求 {url} 失败，超过最大重试次数")
            return []
        await asyncio.sleep(1)  # 等待1秒后重试


# 异步获取并整理返回值（带进度条）
async def fetch_and_process_data(urls, concurrency=100):
    results = []
    errors = {"timeout": 0, "connection_error": 0, "json_error": 0, "other": 0}
    semaphore = asyncio.Semaphore(concurrency)  # 控制并发数
    async with aiohttp.ClientSession() as session:
        tasks = [process_single_url(session, url, semaphore) for url in urls]
        # 使用 tqdm 显示进度条
        for future in tqdm(asyncio.as_completed(tasks), total=len(urls), desc="Processing URLs", unit="url"):
            try:
                result = await future
                results.extend(result)
            except aiohttp.ClientError:
                errors["connection_error"] += 1
            except asyncio.TimeoutError:
                errors["timeout"] += 1
            except json.JSONDecodeError:
                errors["json_error"] += 1
            except Exception:
                errors["other"] += 1
    return results, errors


# 将结果分页存储为多个CSV文件
def write_to_csv_paginated(data, output_prefix, page_size=1000):
    headers = [
        "URL",
        "Model Name",
        "Model ID",
        "Modified At",
        "Size",
        "Digest",
        "Family",
        "Families",
        "Parameter Size",
        "Quantization Level"
    ]
    num_pages = (len(data) // page_size) + (1 if len(data) % page_size != 0 else 0)
    for i in range(num_pages):
        page_data = data[i * page_size:(i + 1) * page_size]
        output_file = f"{output_prefix}_page_{i + 1}.csv"
        with open(output_file, 'w', newline='', encoding='utf-8') as file:
            writer = csv.writer(file)
            writer.writerow(headers)  # 写入表头
            writer.writerows(page_data)  # 写入数据
        logging.info(f"已保存分页文件: {output_file}")


# 主函数
async def main():
    input_file = './url.txt'  # 输入文件路径
    output_prefix = 'processed_data1'  # 输出文件前缀

    # 性能监控：记录每个阶段的耗时
    performance_metrics = {}

    # 读取URL
    start_time = time.time()
    urls = read_urls_from_file(input_file)
    performance_metrics["read_urls"] = time.time() - start_time

    print(f"共读取到 {len(urls)} 个URL，开始处理...")

    # 获取并整理数据（异步处理）
    start_time = time.time()
    processed_results, error_stats = await fetch_and_process_data(urls)
    performance_metrics["fetch_and_process"] = time.time() - start_time

    # 将结果保存为CSV文件
    start_time = time.time()
    write_to_csv_paginated(processed_results, output_prefix)
    performance_metrics["write_to_csv"] = time.time() - start_time

    # 打印性能监控报告
    logging.info("性能监控报告:")
    for stage, duration in performance_metrics.items():
        logging.info(f"{stage.capitalize().replace('_', ' ')}: {duration:.2f} 秒")

    # 打印错误统计报告
    logging.info("错误统计报告:")
    for error_type, count in error_stats.items():
        logging.info(f"{error_type.capitalize().replace('_', ' ')}: {count} 次")

    print(f"数据已保存到以 {output_prefix} 为前缀的分页文件中")


if __name__ == "__main__":
    asyncio.run(main())