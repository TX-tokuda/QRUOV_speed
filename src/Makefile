# OpenSSLのインクルードとライブラリのパス
# OPENSSL_INC=/usr/local/ssl/include
# OPENSSL_LIB=/usr/local/ssl/lib64
OPENSSL_INC=/usr/local/include
OPENSSL_LIB=/usr/local/lib64

# DEBUG=true
# USE_MULTI_THREAD=true
#USE_MULTI_THREAD=false

CC=gcc
CFLAGS=-Wall -O3 -std=c17
TARGET=qruov
STATIC_LIB=libqruov.a
SRCS=qruov.c der.c qruov_signature.c expand.c random.c qruov_matrix.c matrix.c Fql.c
OBJS=qruov.o der.o qruov_signature.o expand.o random.o qruov_matrix.o matrix.o Fql.o
LIBS=-lcrypto
INCDIR=-I../include -I${OPENSSL_INC}
LIBDIR=-L${OPENSSL_LIB}

ifeq (${DEBUG}, true)
  CFLAGS+= -DDEBUG
endif
ifeq (${USE_MULTI_THREAD}, true)
  CFLAGS+= -DQRUOV_USE_MULTI_THREAD -fopenmp
endif
# $@: ルールのターゲット名。
# $^: ルールのすべての依存ファイル（重複なし）

# allターゲット
all: static ${TARGET}
# staticターゲット
static: ${STATIC_LIB}
# 静的ライブラリのターゲット
${STATIC_LIB}: ${OBJS} 
	ar rcs $@ ${OBJS} 

# 実行ファイルのターゲット
${TARGET}: ${STATIC_LIB} main.c
	${CC} -o $@ ${CFLAGS} ${INCDIR} main.c -L. -lqruov ${LIBDIR} ${LIBS}

# オブジェクトファイルの生成
.c.o:
	${CC} ${CFLAGS} ${INCDIR} -c $^

.PHONY: clean
clean:
	rm -f *.o ${TARGET} ${STATIC_LIB}
