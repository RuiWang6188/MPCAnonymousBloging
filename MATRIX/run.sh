#!/usr/bin/env bash

# parameter:
# 1. server (partyID)
# 2. numServers
# 3. numClients
# 4. l
# 5. partiesFile
# 6. interation number
# 7. num threads
# 8. T (number of malicious parties)
# 9. run the client
# 10. toMount 
# 11. toUnmount
# 12. to simulate


if [ "${9}" = "true" ]; then
sudo rm -rf ~/files*
./MPCAnonymousBloggingClient/MPCAnonymousBloggingClient -server "${1}" -numServers "${2}" -numClients "${3}" \
   -fieldType ZpMersenne31 -l "${4}" -T "${8}" -toMount "${10}"
fi

./MPCAnonymousBloging -partyID "${1}" -numServers "${2}" -numClients "${3}" \
   -fieldType ZpMersenne31 -l "${4}" -partiesFile "${5}" -internalIterationsNumber "${6}" -numThreads "${7}" \
    -T "${8}" -toUmount "${11}" -toSimulate "${12}"
