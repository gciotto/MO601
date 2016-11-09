import subprocess
import sys

#cmd = '/home/gciotto/MO601-117136/project3/src/run-pintool '
cmd = sys.argv

print str(cmd[1:])

print "Starting"

#process = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE)

subprocess.call(cmd[1:])

#output, error = process.communicate()
