#!/bin/zsh

echo "Is it morning? Please answer yes or no."

# 获取输入
read LINE

case "$LINE" in
yes | y | Yes | YES )
  echo "Good Morning!" ;;
[nN]*)
  echo "Good Afternoon!" ;;
*)
  echo "Sorry, $YES_OR_NO not recognized. Enter yes or no."
  exit 1
  ;;
esac
exit 0