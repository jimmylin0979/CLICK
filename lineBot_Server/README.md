# lineBot_Server

部署網址：https://click-server-on-heroku.herokuapp.com/

# 目錄

-   [Server 運作程式修改](#overview)
-   [Repository 必備文件](#prerequisite)

<h1 id="overview">Server 運作程式修改</h1>

請先向管理員獲取更改權限，否則更改將無法 Push 成功!!

Heroku Git 專案：https://git.heroku.com/click-server-on-heroku.git

## 1. 提交 Git Commit 並部署到 Heroku

```bash
# 提交修改，進入歷史
$ git add .
$ git commit -m "Initialize Project"

# 更新至遠端 Heroku 的 master 分支
# PUSH 完之後應該會跑一大串，慢慢等吧~
$ git push heroku master               # 提交到远程master分支
```

## 2. 啟動 Heroku 遠端程式，開始運作

```bash
# 開始進行運作，若成功執行，則架設成功
$ heroku open
```

#

<h1 id="prerequisite">Repository 必備文件</h1>

以下為應在 lineBot_Server 根目錄下 Git Repository 內具備的文件，這些文件多用來向 Heroku 描述程式、定義程式入口、啟動等的宣告檔。

最基本的目錄排列應如下：

```
folder/
  ├ .gitignore
  ├ app.json
  ├ Procfile
  ├ README.md
  ├ requirements.txt
  └ run.py
```

-   app.json：用來描述 這項應用的 細節，包括 name, repository 等等。

    repository 務必要與 heroku 上登記的一樣，其他無所謂 XD

    ```json
    {
        "name": "Click Server on Heroku",
        "description": "Click LineBot server that deploys on Heroku",
        "image": "heroku/python",
        "repository": "https://git.heroku.com/click-server-on-heroku.git",
        "keywords": ["python", "flask"]
    }
    ```

-   requirements.txt：Python 檔案運行所需的標題庫

    因为我们不能靠 Flask 自带的 Web 服务器来运行 Flask 程序，所以 gunicorn 是个很好的选择。

    ```bash
    Flask==0.10.1
    gunicorn==19.4.5
    ```

-   Procfile：告訴 Heroku 要如何啟動這個程式來開始運作

    `-log-file -`是为了让日志打印到标准输出 stdout 上，因为 Heroku 不提供撰寫本地磁盘的功能。

    ```bash
    # 以 gunicorn 代為起動 程式
    # <pyfile> 請更改為 主啟動py檔，僅需要 檔名不 需副檔名
    # 如果程式啟動為 main.py，則應為 web: gunicorn main:app --log-file -
    web: gunicorn <pyfile>:app --log-file -
    ```
