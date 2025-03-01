"""
title: i do not kneo
authors: niubi
author_url: https://github.com/acking-you
funding_url: https://github.com/open-webui
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
        URL: str = Field(default="http://122.116.109.94:11434/")

    def __init__(self):
        self.type = "manifold"
        self.id = "scan"
        self.name = "scan/"
        self.valves = self.Valves(**{"URL": os.getenv("URL", "")})
        pass

    def get_deepseek_models(self):
        url = self.valves.URL + 'api/tags'
        sp = requests.get(url)
        sp = json.loads(sp.text)
        return [
            {"id": model["model"], "name": model["name"]}
            for model in sp['models']
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
        headers["Content-Type"] = "application/json"

        url = self.valves.URL+"/v1/chat/completions"

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
