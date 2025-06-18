import os
import shutil
import threading

def process_file(filename, keyword, tag_folder, processed_files):
    """
    处理单个文件的函数，用于多线程执行。
    复制文件到tag文件夹，然后删除源文件。
    """
    filepath = os.path.join('.', filename)
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
            if keyword in content:
                destination_path = os.path.join(tag_folder, filename)
                shutil.copy2(filepath, destination_path)  # 复制文件，保留元数据
                print(f"  [线程 {threading.current_thread().name}] 文件 '{filename}' 包含关键词 '{keyword}'，已复制到 '{tag_folder}' 文件夹。")
                os.remove(filepath)  # 删除源文件
                print(f"  [线程 {threading.current_thread().name}] 文件 '{filename}' 的源文件已删除。")
                processed_files.append(filename) #将已处理的文件名加入列表
            else:
                print(f"  [线程 {threading.current_thread().name}] 文件 '{filename}' 不包含关键词 '{keyword}'，保持不变。")
    except Exception as e:
        print(f"  [线程 {threading.current_thread().name}] 处理文件 '{filename}' 时出错: {e}")


def move_files_with_tags(tag_config):
    """
    读取当前目录下的txt文件，根据配置的关键词和tag文件夹，将包含关键词的文件复制到对应的tag文件夹下并删除源文件。
    使用多线程加速文件处理。
    支持多个关键词和tag文件夹的配置。
    将未匹配任何关键词的文件移动到 "others" 文件夹。

    Args:
        tag_config (dict): 关键词和目标tag文件夹的配置字典。
    """

    all_processed_files = []  # 用于存储所有已处理的文件

    for keyword, tag_folder in tag_config.items():
        print(f"处理关键词: '{keyword}'，目标文件夹: '{tag_folder}'")

        # 确保tag文件夹存在，如果不存在则创建
        if not os.path.exists(tag_folder):
            os.makedirs(tag_folder)
            print(f"创建tag文件夹 '{tag_folder}'")

        threads = []  # 用于存储线程对象的列表
        processed_files = []  # 用于存储已处理的文件名 (针对当前关键词)
        for filename in os.listdir('.'):
            if filename.endswith('.txt'):  # 仅处理txt文件
                # 为每个文件创建一个线程来处理
                thread = threading.Thread(target=process_file, args=(filename, keyword, tag_folder, processed_files))
                threads.append(thread)
                thread.start()

        # 等待所有线程完成
        for thread in threads:
            thread.join()

        all_processed_files.extend(processed_files)  # 将当前关键词处理的文件添加到总列表

    # --- 处理未匹配的文件 ---
    others_folder = "others"
    if not os.path.exists(others_folder):
        os.makedirs(others_folder)
        print(f"创建 'others' 文件夹")

    for filename in os.listdir('.'):
        if filename.endswith('.txt') and filename not in all_processed_files:
            source_path = os.path.join('.', filename)
            destination_path = os.path.join(others_folder, filename)
            try:
                shutil.move(source_path, destination_path)  # 使用 shutil.move 直接移动
                print(f"文件 '{filename}' 未匹配任何关键词，已移动到 'others' 文件夹。")
            except Exception as e:
                 print(f"移动文件 '{filename}' 到 'others' 文件夹时出错: {e}")


if __name__ == "__main__":
    # 配置关键词和tag文件夹的字典
    tag_configuration = {
        "数学": "数学",
        "语言": "语言",
        "比赛": "比赛",
        "日记": "日记",
        "科技": "科技",
        "生活": "生活",
        "平面": "平面"
        # 可以根据需要添加更多的关键词和tag文件夹
    }

    print("程序开始执行，根据以下配置复制并删除文件 (多线程):")
    for keyword, tag_folder in tag_configuration.items():
        print(f"  关键词: '{keyword}', 目标文件夹: '{tag_folder}'")

    move_files_with_tags(tag_configuration)
    print("程序执行完毕。")
