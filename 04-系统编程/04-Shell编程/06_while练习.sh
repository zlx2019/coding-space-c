#! /bin/sh

echo "Enter password:"
read PASSWD

TRY_COUNT=1

while [ "$PASSWD" != "root" ]; do
  if [ "$TRY_COUNT" = 5 ]; then
    echo "Sorry, no retries available"
    exit 1
  fi
	echo "Sorry, try again"
	TRY_COUNT=$((TRY_COUNT+1))
	read PASSWD
done