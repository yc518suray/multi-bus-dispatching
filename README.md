# multi-bus-dispatching

This program simulates multi-bus dispatching strategy, for the final project of Intelligent Manufacturing Systems (智慧型製造系統), NCKU 2025 fall.

Reference paper: Y. Zhang, et al., "A Multi-Bus Dispatching Strategy Based on Boarding Control", _IEEE TRANSACTIONS ON INTELLIGENT TRANSPORTATION SYSTEMS_, VOL. 23, NO. 6, JUNE 2022.

# Intro

main.cpp實現參考論文的做法，main2.cpp則是修改參考論文提出的問題，並嘗試得到修改後問題的最佳解。(2025/12/29 目前仍未完成main2.cpp)

由於現實資料難以取得，我們嘗試以離散事件模擬的方式，生成trip之間來到車站的乘客數 $f_{i, j}(k)$，請見gen_data_sim.cpp。

# Data format

模擬所需的資料儲存在`.dat` extension的檔案裡。資料檔的第一行列出一些基本的參數，如下(數值為範例，隨便填上的)：

| # of trips | $\Delta$ | $T_r$ | $DT$ | $Cap_b$ | $H_p$ | $N_s$ | $N_b$ |
| ---------- | -------- | ----- | ---- | ------- | ----- | ----- | ----- |
| 12         | 600      | 3600  | 180  | 100     | 15    | 20    | 5     |

其中 $\Delta$ 為最小派車區間(seconds)，即兩trip的間隔時間， $T_r$ 為round-trip time(seconds，不計算每站停車時間)， $DT$ 為每一站的停留時間(dwell time，in seconds)， $Cap_b$ 為每輛公車的乘客容量， $H_p$ 為預測區間(prediction horizon，in seconds)， $H_p = N\Delta$， $N$ 為預測步數(prediction step)， $N_s$ 為車站數量， $N_b$ 則是公車總數。

<br>

資料檔的第二行(含)以後則是如下的格式(假設9個車站且共10個trip)：

| No. | Trip number ($k$) | $i$ | $j$ | $f_{i, j}(k)$ |
| --- | ----------------- | --- | --- | ------------- |
| 0   | 0                 | 1   | 2   | 12            |
| 1   | 0                 | 1   | 3   | 2             |
| 2   | 0                 | 1   | 4   | 0             |
| $\vdots$ | $\vdots$ | $\vdots$ | $\vdots$ | $\vdots$ |
| 366 | 9                 | 8   | 1   | 3             |
| 367 | 9                 | 9   | 1   | 1             |

其中第一欄為行數(實際上不會出現，表格只是示意這是第幾行)，第二欄(實際上的第一欄)就是trip number，第三欄是搭乘站的index ($i$)，第四欄是目的站的index ($j$)，第五欄是在第 $k$ 和第 $k + 1$ 個trip之間來到第 $i$ 站、目的地為第 $j$ 站的乘客數 ( $f_{i, j}(k)$ )。第 1 站就是發車的terminal。

# Compile

執行：

```shell
./compile.sh
```

若要改變參數(如公車數量、trip數量等等)，需要在source code裡修改後重新編譯。

# Usage

執行：

```shell
./main.sh
```

main.sh會從gen_sim_data開始執行，生成data，接著執行main來求解。因此任何變動都應修改`src/gen_sim_data.cpp`的數值設定。
