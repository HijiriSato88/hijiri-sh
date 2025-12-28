FROM alpine:latest

# ビルドツールをインストール
RUN apk add --no-cache build-base

# 作業ディレクトリを設定
WORKDIR /shell

# ソースコードをコピー
COPY src/ ./src/

# ビルド
RUN gcc -Wall -Wextra -g -o hijiri-sh src/main.c

# 自作シェルを起動
CMD ["./hijiri-sh"]
