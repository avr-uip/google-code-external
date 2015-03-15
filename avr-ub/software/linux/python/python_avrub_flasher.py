#!/usr/bin/python

import os
import sys
import time
import select
import serial

MAX_CONNECT_COUNTER = 9
REBOOT_CMD = chr(0x02) + 'C' + 'R' + 'R' + chr(0x03)
ENTER_PWD = 'AVRUB'
TTY_DEVICE = '/dev/ttyUSB1'

def readbin():
    try:
        bin = open(sys.argv[1]).read()
        return sys.argv[1]
    except Exception, ex:
        print 'open bin file error: %s' %(ex)
        return False

def conn():
    global ser
    try:
        ser = serial.Serial(TTY_DEVICE, 57600, timeout=1)
    except Exception, ex:
        print 'canNOT open serial device'
        return False
    ser.flushInput()
    ser.flushOutput()
    
    counter = 0
    while True:
        
        ack = ser.readline()
        #print repr(ack)
        
        if (ack.find('waiting for password') == -1) and (ack.find('CCC') == -1):
            counter += 1
            if counter >= MAX_CONNECT_COUNTER:
                print 'device no respond'
                return False
            else:
                print '. \b',
                sys.stdout.flush()
        else:
            print 'CONNECTTED, sending password..'
            ser.write(ENTER_PWD)
            ack = ser.readline()
            print ack
            return True
        
        ser.write(REBOOT_CMD)


def flash(bin):
    global ser
    
    print 'close old handler'
    ser.close()

    cmd = 'sz --xmodem -C3 -v "%s" < %s > %s' %(bin, TTY_DEVICE, TTY_DEVICE)
    child_stdin, child_stdout, child_stderr = os.popen3(cmd)
    child_stdin.close()

    star = ('|', '/', '-', '\\')
    idx = 0
    while (True):
        try:
            stdin, stdout, stderr = select.select((child_stdout, child_stderr), (), (), 0.5)
        except Exception, ex:
            print 'select exception: %s' %(ex)
            return False

        #val_out = ''
        #val_err = ''
        for fd in stdin:
            if fd == child_stdout:
                val_out = child_stdout.readline()
                print '\n[DEBUG] stdout ->', repr(val_out)
            elif fd == child_stderr:
                val_err = child_stderr.readline()
                if val_err == '':
                    return False
                else:
                    print '\n[DEBUG] stderr ->', repr(val_err)
        
        if val_err.find('Transfer complete') != -1:
            print 'done'
            return True
        else:
            print '%c\b\b'%(star[idx]),
            idx += 1
            idx %= len(star)
            sys.stdout.flush()
        
        time.sleep(0.1)


##########################
if __name__ == '__main__':
    bin = readbin()
    if (bin == False):
        print 'GIVEN FILE CANNOT ACCESS'
        sys.exit(-1)
    
    if not conn():
        print 'CANNOT CONNECTED TO DEVICE'
        sys.exit(-1)
    
    if not flash(bin):
        print 'FLASHing ERROR'
        sys.exit(-1)
    else:
        print 'FLASHing SUCCESS'
        sys.exit(0)
