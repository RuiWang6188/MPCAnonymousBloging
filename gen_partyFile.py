NUM_SERVERS = 1
NUM_CLIENTS = 5

IP_ADDRESS = "127.0.0.1"
PORT = 8000
index = 0



with open(f"Parties_{NUM_SERVERS}_{NUM_CLIENTS}.txt", 'w') as file:
    for i in range(NUM_SERVERS):
        file.write(f"party_{index}_ip = {IP_ADDRESS}\n")
        index += 1
    for i in range(NUM_CLIENTS):
        file.write(f"party_{index}_ip = {IP_ADDRESS}\n")
        index += 1
    
    index = 0
    for i in range(NUM_SERVERS):
        file.write(f"party_{index}_port = {PORT}\n")
        PORT += 10
        index += 1
    for i in range(NUM_CLIENTS):
        file.write(f"party_{index}_port = {PORT}\n")
        PORT += 10
        index += 1

