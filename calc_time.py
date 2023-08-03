import os 
import json

dir_path = "."
files_in_dir = os.listdir(dir_path)

time_files = [filename for filename in files_in_dir if filename.endswith("true.json")]

print(f"number of servers: {len(time_files)}")

total_time = 0.0

for time_file in time_files:
    file_path = os.path.join(dir_path, time_file)
    
    with open(file_path, 'r') as file:
        data = json.load(file)
    
    online_time = data["times"][7]["iteration_0"]
    offline_time = data["times"][-1]["iteration_0"]
    time = float(online_time) + float(offline_time)
    total_time += time
    print(time)

print(f"average server time: {total_time / len(time_files) / 1000.0}")