url = "http://127.0.0.1:6006/api/chat"

import requests
import json


def send_post_request(url, data):
    try:
        headers = {"Content-type": "application/json"}
        response = requests.post(url, data=json.dumps(data))
        if response.status_code == 200:
            return response.json()
        else:
            print(f"Request failed with status code: {response.status_code}")
            return None
    except requests.exceptions.RequestException as e:
        print(f"Request failed: {e}")
        return None


if __name__ == "__main__":
    # 请求参数
    data = {
        "model": "qwen2-rp",
        "messages": [{"role": "user", "content": "who are you?"}],
        "stream": False,
    }

    # 发送 POST 请求并获取响应数据
    response_data = send_post_request(url, data)
    if response_data:
        # print("Response data:")
        # 提取 content 字段的值
        content = response_data["message"]["content"]
        print(content)
