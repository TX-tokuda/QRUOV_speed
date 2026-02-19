
## コンパイルと実行

```
$ make
$ ./qruov
```
`$ make`によって静的ライブラリ`libqruov.a`と実行ファイル`qruov`が生成される。  
`$ ./qruov`によってパラメータ初期化、鍵生成、署名生成、署名検証の一連の処理を実行するサンプルプログラムが実行される。  

## パラメータの指定

パラメータは外部インタフェース`QRUOV_paramgen()`の引数によって指定する。  
```
QRUOV_paramgen("qruov1q127L3v156m54", "SHAKE", &para);
```
パラメータに`qruov1q127L3v156m54`を指定。  
擬似乱数生成器に`SHAKE`を指定。  

## コンパイルオプション

### マルチスレッド

```
$ make USE_MULTI_THREAD=true
```
マルチスレッドで実行。  

`USE_MULTI_THREAD=true`としているときに以下のコマンドをターミナル上で実行すると、スレッド数を指定できる。  
```
$ export OMP_NUM_THREADS=4
```
スレッド数4で実行。  

```
$ make USE_MULTI_THREAD=false
```
シングルスレッドで実行。  

### デバッグモード

```
$ make DEBUG=true
```
デバッグモードをオン。エラーメッセージが出力される。  

```
$ make DEBUG=false
```
デバッグモードをオフ。エラーメッセージが出力されなくなる。  

以上
