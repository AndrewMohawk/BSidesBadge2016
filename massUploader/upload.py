#!/usr/bin/python
import sys
import glob
import os


print '''  ____            _              _    _       _                 _           
 |  _ \          | |            | |  | |     | |               | |          
 | |_) | __ _  __| | __ _  ___  | |  | |_ __ | | ___   __ _  __| | ___ _ __ 
 |  _ < / _` |/ _` |/ _` |/ _ \ | |  | | '_ \| |/ _ \ / _` |/ _` |/ _ \ '__|
 | |_) | (_| | (_| | (_| |  __/ | |__| | |_) | | (_) | (_| | (_| |  __/ |   
 |____/ \__,_|\__,_|\__, |\___|  \____/| .__/|_|\___/ \__,_|\__,_|\___|_|   
                     __/ |             | |                                  
                    |___/              |_|                                  
'''

binPath = "./BSidesBadge2016.ino.bin"
espPath = "/Applications/Arduino.app/Contents/Java/hardware/esp8266com/esp8266/tools/esptool/esptool"
uploadSpeed = 115200

if(len(sys.argv) > 0):
    if(len(sys.argv) > 1):
        binPath = sys.argv[1]
    if(len(sys.argv) > 2):
        uploadSpeed = sys.argv[2]
    if(len(sys.argv) > 3):
        espPath = sys.argv[3]
    


ports = glob.glob('/dev/cu.wch*')
if len(ports) == 0:
    print "trying *nix"
    ports = glob.glob('/dev/ttyUSB*')
    
print "Found %s Devices:" % len(ports)
for port in ports:
    print "[*] %s " % port
    
raw_input("Press Enter to upload...")

for port in ports:
    command = "%s -cd ck -cb %s -cp %s -ca 0x00000 -cf %s" % (espPath,uploadSpeed,port,binPath)
    print ""
    print "------------------------------"
    print "Uploading to %s" % port
    print "------------------------------"
    #print "executing %s" % command
    os.system(command)
    raw_input("Press Enter to upload to next board...")
    
