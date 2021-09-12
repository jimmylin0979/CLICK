from numpy import sign
import pandas as pd

df = pd.read_csv('AED2021090709191126.csv')

print(df.shape[0])

for idx in range(10):

    PLACE = df.iloc[idx, 0]
    LAT, LOG = df.iloc[idx, 10], df.iloc[idx, 11]
    TELE = df.iloc[idx, -1]

    # print(f'{place}, lat={lat}, log={log}, tele={tele}')

    command = f"INSERT INTO AED (ID,PLACE,LAT,LNG,TELE) \
        VALUES ({idx},'{PLACE}',{LAT},{LOG},'{TELE}' )"
    
    print(command)