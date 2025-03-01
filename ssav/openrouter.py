"""
title: openrouter Manifold Pipe
authors: stevessr
version: 1.0.0
required_open_webui_version: 0.3.32
license: MIT
"""

import os
import requests
import json
import time
from typing import List, Union, Generator, Iterator
from pydantic import BaseModel, Field
from open_webui.utils.misc import pop_system_message


class Pipe:
    class Valves(BaseModel):
        API_KEY: str = Field(default="")

    def __init__(self):
        self.type = "manifold"
        self.id = "openrouter"
        self.name = "openrouter/"
        self.valves = self.Valves(**{"API_KEY": os.getenv("API_KEY", "")})
        pass

    # see: https://docs.siliconflow.cn/api-reference/chat-completions/chat-completions#body-model
    def get_deepseek_models(self):
        return [
            {"id": "cognitivecomputations/dolphin3.0-r1-mistral-24b:free","name": "Dolphin3.0 R1 Mistral 24b (free)"},
            {"id": "cognitivecomputations/dolphin3.0-mistral-24b:free","name": "Dolphin3.0 Mistral 24b (free)"},
            {"id": "google/gemini-2.0-flash-lite-preview-02-05:free","name": "Google: Gemini Flash Lite 2.0 Preview (free)"},
            {"id": "google/gemini-2.0-pro-exp-02-05:free", "name": "Google: Gemini Pro 2.0 Experimental (free)"},
            {"id": "qwen/qwen-vl-plus:free","name": "Qwen: Qwen VL Plus (free)",},
            {"id": "qwen/qwen2.5-vl-72b-instruct:free","name": "Qwen: Qwen2.5 VL 72B Instruct (free)",},
            {"id": "mistralai/mistral-small-24b-instruct-2501:free","name": "Mistral: Mistral Small 3 (free)",},    
            {"id": "deepseek/deepseek-r1-distill-llama-70b:free","name": "DeepSeek: R1 Distill Llama 70B (free)",},
            {"id": "google/gemini-2.0-flash-thinking-exp:free","name": "Google: Gemini 2.0 Flash Thinking Experimental 01-21 (free)",},
            {"id": "deepseek/deepseek-r1:free","name": "DeepSeek: R1 (free)",},
            {"id": "deepseek/deepseek-chat:free","name": "DeepSeek: DeepSeek V3 (free)",},
            {"id": "google/gemini-2.0-flash-thinking-exp-1219:free","name": "Google: Gemini 2.0 Flash Thinking Experimental (free)",},
            {"id": "google/gemini-2.0-flash-exp:free","name": "Google: Gemini Flash 2.0 Experimental (free)",},
            {"id": "meta-llama/llama-3.3-70b-instruct:free","name": "Meta: Llama 3.3 70B Instruct (free)",},
        ]

    def pipes(self) -> List[dict]:
        return self.get_deepseek_models()

    def pipe(self, body: dict) -> Union[str, Generator, Iterator]:
        system_message, messages = pop_system_message(body["messages"])

        processed_messages = []

        for message in messages:
            processed_content = ""
            if isinstance(message.get("content"), list):
                for item in message["content"]:
                    if item["type"] == "text":
                        processed_content = item["text"]
            else:
                processed_content = message.get("content", "")

            processed_messages.append(
                {"role": message["role"], "content": processed_content}
            )

        # Ensure the system_message is coerced to a string
        payload = {
            "model": body["model"][body["model"].find(".") + 1 :],
            "messages": processed_messages,
            "stream": body.get("stream", False),
        }

        headers = {}
        headers["Authorization"] = f"Bearer {self.valves.API_KEY}"
        headers["Content-Type"] = "application/json"

        url = "https://openrouter.ai/api/v1/chat/completions"

        try:
            if body.get("stream", False):
                return self.stream_response(url, headers, payload)
            else:
                return self.non_stream_response(url, headers, payload)
        except requests.exceptions.RequestException as e:
            print(f"Request failed: {e}")
            return f"Error: Request failed: {e}"
        except Exception as e:
            print(f"Error in pipe method: {e}")
            return f"Error1: {e}"

    def stream_response(self, url, headers, payload):
        # Remove unnecessary fields from the payload
        unnecessary_fields = ["user", "chat_id", "title"]
        for field in unnecessary_fields:
            payload.pop(field, None)

        try:
            response = requests.post(
                url=url,
                json=payload,
                headers=headers,
                stream=True,
            )

            if response.status_code != 200:
                raise Exception(
                    f"HTTP Error {payload} {response.status_code}: {response.text}"
                )

            # Process the streamed response
            for chunk in response.iter_content(chunk_size=None):
                if chunk:
                    yield chunk.decode("utf-8")
                # Delay to avoid overwhelming the client
                time.sleep(0.01)

        except requests.exceptions.RequestException as e:
            print(f"Stream request failed: {e}")
            yield f"Error: {e}"
        except Exception as e:
            print(f"Error in stream_response method: {e}")
            yield f"Error: {e}"

    def non_stream_response(self, url, headers, payload):
        try:
            response = requests.post(
                url=url, headers=headers, json=payload, timeout=(3.05, 60)
            )
            if response.status_code != 200:
                raise Exception(f"HTTP Error: {response.status_code}: {response.text}")

            return response.json()

        except requests.exceptions.RequestException as e:
            print(f"Failed non-stream request: {e}")
            return f"Error5: {response.content}"
        except Exception as e:
            print(f"Error in non_stream_response method: {e}")
            return f"Error6: {e}"
