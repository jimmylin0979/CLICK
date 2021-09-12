# CLICK

## 資料夾階層

```
├───Hardware
|   ├───SerialListener
│   └───Click Patch
|
└───lineBot_Server
```

-   Hardware Click 貼片硬體控制相關程式

    -   Click_Patch
    -   SerialListener

-   lineBot_Server lineBot 聊天機器人之處理伺服器
    -   Ngrok
    -   app.py
    -   config.ini

## 啟動 LineBot Server

### 以 Heroku 部署 Server

-   優：電腦、網路無須常駐開啟，網址固定不會變動
-   劣：免費版會休眠，長時間未用時再啟用會花費約 10s 喚醒時間；

目前部署的主要方式，詳情請看 LineBot_Server 資料夾 README

部署網址：https://click-server-on-heroku.herokuapp.com/

### 以 Ngrok 部署 Server

-   優：部署簡單
-   劣：電腦、網路必須常駐開啟；每次重開 Server 時網址皆會變化

1. 執行 `server-start.bat`，以 Flask 架設伺服器
2. 執行 `server-deploy.bat`，以 Ngrok 將內網與外網串接起來
   ![Ngrok-webhookURL.png](./img/Ngrok-webhookURL.png)
3. 進入 [line Developers](https://developers.line.biz/en/) 網站，將 webhook URL 更新為當前串接之外網網址 (上一部獲得之網址)
   ![lineDevelopers-webhookURL.png](./img/lineDevelopers-webhookURL.png)
4. lineBot Server 建設完成
