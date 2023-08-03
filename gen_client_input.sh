#!/usr/bin/env bash

# parameter:
# 1. numServers
# 2. numClients
# 3. l
# 4. T (number of malicious parties)
# 5. toMount 


for i in $(seq 0 1 "$((${1} - 1))")
do
    ./MPCAnonymousBloggingClient/MPCAnonymousBloggingClient -server "${i}" -numServers "${1}" -numClients "${2}" \
    -fieldType ZpMersenne31 -l "${3}" -T "${4}" -toMount "${5}"
done

