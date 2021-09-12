
# AED 位置，來源：政府資料開放平台
# https://data.gov.tw/dataset/12063

import sqlite3
import pandas as pd
from tqdm import tqdm, trange

############################################################

DB_Name = 'AED.db'

# Open DataBase
conn = sqlite3.connect(DB_Name)
print("1. Opened database successfully")

# SQL : CREATE TABLE
c = conn.cursor()
c.execute(
    '''CREATE TABLE AED (
        ID INT PRIMARY KEY     NOT NULL,
        PLACE          TEXT    NOT NULL,
        LAT            REAL     NOT NULL,
        LNG            REAL     NOT NULL,
        TELE           TEXT);
    ''')
conn.commit()
conn.close()

print("2. Table created successfully")

############################################################

# SQL : INSERT 
conn = sqlite3.connect(DB_Name)
c = conn.cursor()

# Pandas : read csv as dataFrame
df = pd.read_csv('AED2021090709191126.csv')
print("3. Reac CSV successfully")

cnt = 0

for idx in tqdm(range(df.shape[0])):
    # Data Parser
    try:
        PLACE = df.iloc[idx, 0]
        LAT, LOG = df.iloc[idx, 10], df.iloc[idx, 11]
        TELE = df.iloc[idx, -1]
    
    except Exception as ex:
        print(f"Index : {idx} has missing value, ignore this Row")
        continue

    # INSERT 
    command = f"INSERT INTO AED (ID,PLACE,LAT,LNG,TELE) \
        VALUES ({cnt},'{PLACE}',{LAT},{LOG},'{TELE}' )"
    c.execute(command)
    # c.execute("INSERT INTO AED (ID,PLACE,LAT,LNG,TELE) \
    #     VALUES (1, 'Paul', 127.0121, 25.435, '0979286035' )")

    cnt += 1
    conn.commit()

conn.close()
print("4. Records created successfully")

############################################################