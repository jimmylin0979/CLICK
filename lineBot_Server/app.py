from __future__ import unicode_literals

from flask import Flask, request, abort

from linebot import LineBotApi, WebhookHandler
from linebot.exceptions import InvalidSignatureError
# MessageType Class, such as StickerSendMessage, ImageSendMessage
from linebot.models import *

import os
import configparser
import random


app = Flask(__name__)

#########################################################################################
### Configuration Here ###

# 讀取 Config.ini 設置檔
config = configparser.ConfigParser()
config.read('config.ini')

# Flask 網站設定
# 獲取 本地端 port 接口、 DEBUG 模式
# PORT = config.get('')

# LINE 聊天機器人的基本資料
# 向 config.ini 讀取 channel_secret 及 channel_access_token
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

@app.route("/esp", methods=['POST', 'GET'])
def esp():
    if request.method == 'POST':
        print(f'POST Message : {request.get_data(as_text=True)}')
        return 'Succeed : POST'
    elif request.method == 'GET':
        print(request)
        return 'Succeed : GET'
    
    return 500

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
    if event.source.user_id == 'Udeadbeefdeadbeefdeadbeefdeadbeef':   return

    # Phoebe 愛唱歌
    pretty_note = '♫♪♬'
    pretty_text = ''

    for i in event.message.text:

        pretty_text += i
        pretty_text += random.choice(pretty_note)

    line_bot_api.reply_message(
        event.reply_token,
        StickerSendMessage(
            package_id='1',
            sticker_id='1'
        )
    )

#########################################################################################
### Main Function ###

if __name__ == "__main__":

    # run the server, and set the server port to localhost://5000
    app.run(debug=True, host='0.0.0.0', port=5000)
