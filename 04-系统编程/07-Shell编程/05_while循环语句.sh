#! /bin/zsh
COUNT=3

while [ ! $COUNT -eq 0 ]; do
	echo "Hello World $COUNT"
  COUNT=$((COUNT-1))
done