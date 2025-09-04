# -*- coding: utf-8 -*-
import sys
import os

def get_image_data(prompt_str):
    """
    一个用于离线测试的模拟函数。
    它不调用任何网络API，而是根据输入的提示词，
    从本地 "local_images" 文件夹中加载并返回一张预存的图片。
    """
    try:
        # 1. 根据C++传来的提示词，决定要加载哪张图片
        #    这里的逻辑与 C++ 中的 garbageList 严格对应
        if "塑料矿泉水瓶" in prompt_str:
            image_filename = "recyclable.png"
        elif "油漆桶" in prompt_str:
            image_filename = "hazardous.png"
        elif "苹果核" in prompt_str:
            image_filename = "kitchen.png"
        elif "卫生纸" in prompt_str:
            image_filename = "other.png"
        else:
            # 如果没有匹配的关键词，默认返回一个图片或报错
            print(f"错误：未知的提示词 '{prompt_str}'，无法匹配本地图片。", file=sys.stderr)
            return None

        # 2. 构建图片的完整路径
        #    脚本假定 "local_images" 文件夹与它自己位于同一目录下
        base_path = os.path.dirname(os.path.abspath(__file__))
        image_path = os.path.join(base_path, "local_images", image_filename)

        # 3. 检查文件是否存在
        if not os.path.exists(image_path):
            print(f"错误：无法在以下路径找到图片文件: {image_path}", file=sys.stderr)
            print("请确保您已在项目根目录创建了 'local_images' 文件夹，并放入了正确的图片。", file=sys.stderr)
            return None

        # 4. 以二进制模式读取文件内容并返回
        with open(image_path, "rb") as f:
            image_bytes = f.read()
            # 模拟一个短暂的延迟，让测试更接近真实网络情况
            import time
            time.sleep(0.5) 
            return image_bytes

    except Exception as e:
        # 捕获任何可能的异常，并将错误信息打印到控制台
        print(f"错误：在执行Python离线测试脚本时发生未知异常: {e}", file=sys.stderr)
        return None
