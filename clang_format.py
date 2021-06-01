import os
import os.path
import sys
path = ""
if (len(sys.argv) >= 2):
	path = sys.argv[1]
if (path == ""):
	path = sys.path[0]

print("Target Directory:%s" %(path))

for root, dirs, files in os.walk(path):
	for name in files:
		if (name.endswith(".h") or name.endswith(".cpp")):
			localpath = root + '/' + name
			print(localpath)
			os.system("clang-format -i %s -style=File" %(localpath))

print("Format Finish")