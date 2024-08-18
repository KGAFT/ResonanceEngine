import os
import shutil
import psutil

os.system("cd modules/assimp && mkdir build")
os.system("cd modules/assimp/build && cmake  -G \"Ninja\" ../")

memory_amount = psutil.virtual_memory().total /(1024.**3)
threads_count = os.cpu_count()

jobs_count = 1
if(memory_amount>threads_count):
	jobs_count = min(threads_count*1.4, memory_amount/1.14)
else:
	jobs_count = min(memory_amount, threads_count)
jobs_count = int(jobs_count)
print(jobs_count)

os.system("cd modules/assimp/build && cmake --build . --config Release -j "+str(jobs_count))

os.system("mkdir NativeModules")

shutil.copytree("modules/assimp/build/bin", "NativeModules/bin")
shutil.copytree("modules/assimp/build/include", "NativeModules/include_cfg") 
shutil.copytree("modules/assimp/include", "NativeModules/include")
shutil.copytree("modules/assimp/contrib", "NativeModules/include/contrib")
shutil.copytree("NativeModules/bin", "JavaModules")
shutil.copytree("modules/assimp/build/lib", "NativeModules/lib")
