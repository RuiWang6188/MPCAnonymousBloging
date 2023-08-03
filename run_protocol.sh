#! /bin/bash

# parameter:
# 1. min partyID
# 2. max partyID
# 3. numServers
# 4. numClients
# 5. partiesFile
# 6. numThreads
# 7. T (number of malicious parties)
# 8. toUmount
# 9. toSimulate

for i in $(seq "${1}" 1 "${2}");
do
	./MPCAnonymousBloging  -l 160 -partyID "${i}" -numServers "${3}" -partiesNumber "${3}" -numClients "${4}"  -fieldType ZpMersenne31 \
	 -partiesFile "${5}"  -internalIterationsNumber 1 -numThreads "${6}" -T "${7}" -toUmount "${8}"  -toSimulate "${9}" &
	echo "Running $i..."
done
