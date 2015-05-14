import socket
import os
import time

PORT_NUM = 8765

#	Read in file_name using open
#	Store commands into a list
#	Open up a socket to the server
#	Iterate through the list and pass the commands through

file_name = raw_input("Please provide a local test file: ")
test_file = open(file_name, "r")

commands = []

for line in test_file:
	commands.append(line)

for line in commands:
	print line

test_file.close()
