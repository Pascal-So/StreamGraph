#!/bin/bash

# test some of the fifo functionalities

if [ ! -d "/tmp/fifos" ]; then
    mkdir /tmp/fifos
fi

mkfifo /tmp/fifos/a
mkfifo /tmp/fifos/b

# all the fifo interaction will be done with cat

# everything that leaves a fifo open must be sent to the background using &

echo "Some t!e%x^t" | cat >/tmp/fifos/a &

cat /tmp/fifos/a | sed 's/e//g' | cat >/tmp/fifos/b &

cat /tmp/fifos/b

rm /tmp/fifos/a
rm /tmp/fifos/b
