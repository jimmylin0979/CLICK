from __future__ import unicode_literals
from flask import Flask, request, abort
from linebot import LineBotApi, WebhookHandler
from linebot.exceptions import InvalidSignatureError
from linebot.models import MessageEvent, TextMessage, TextSendMessage
import os
import configparser
import random


app = Flask(__name__)

#########################################################################################
### Configuration Here ###

# LINE 聊天機器人的基本資料
# 向 config.ini 讀取 channel_secret 及 channel_access_token
config = configparser.ConfigParser()
config.read('config.ini')

line_bot_api = LineBotApi(config.get('line-bot', 'channel_access_token'))
handler = WebhookHandler(config.get('line-bot', 'channel_secret'))

# LineBot 拒絕對話與回應的 黑名單，如官方罐頭訊息等
blackList = ['Udeadbeefdeadbeefdeadbeefdeadbeef']

#########################################################################################
### Server API ###

# 接收 LINE 的資訊
@app.route("/callback", methods=['POST'])
def callback():
    signature = request.headers['X-Line-Signature']

    body = request.get_data(as_text=True)
    app.logger.info("Request body: " + body)

    try:
        print(body, signature)
        handler.handle(body, signature)

    except InvalidSignatureError:
        abort(400)

    return 'OK'

#########################################################################################
### Event Handle Trigger ###

# Event Handle 事件處發函式
# 當收到 LINE 的 MessageEvent (信息事件)，且信息是屬於 TextMessage (文字信息)的時候
@handler.add(MessageEvent, message=TextMessage)
def echo(event):

    '''
        event 格式
        event = {
            "reply_token":"就是代表reply_token的一串亂碼", 
            "type":"message",
            "timestamp":"1462629479859", 
            "source":{  "type":"user",
                        "user_id":"就是代表user的一串亂碼"}, 
            "message":{ "id":"就是代表這次message的一串代碼", 
                        "type":"text", 
                        "text":"使用者傳來的文字信息內容"}
        } 
    '''

    # 若為 Line 官方傳遞之罐頭訊息，則跳過
    if event.source.user_id in blackList:   return

    # Phoebe 愛唱歌
    pretty_note = '♫♪♬'
    pretty_text = ''

    for i in event.message.text:

        pretty_text += i
        pretty_text += random.choice(pretty_note)

    line_bot_api.reply_message(
        event.reply_token,
        TextSendMessage(text=pretty_text)
    )

#########################################################################################
### Main Function ###

if __name__ == "__main__":
    app.run()
