# rpi-cec
Control TV from Raspberry PI, assuming you are using a TV with 2 HDMI input sources and *not* using an antenna connection. For example

    HDMI 1 = Chromecast
    HDMI 2 = Raspberry PI

The idea with rpi-cec is to run as a UDP server and receive messages for controlling the TV via HDMI. This is achieved by issuing commands to the TV via the *cec-client* application that must be installed on the PI:

    sudo apt install cec-utils

## rpi-cec messages


| TV Function     | rpi-cec UDP message   | cec-client command     |
| :---            | :--------------       | :--------------------- |
| on              | cec-on {addr}         | on {addr}              |
| standby         | cec-standby {addr}    | standby {addr}         |
| volume up       | cec-volup             | volup                  |
| volume down     | cec-voldown           | voldown                |
| mute            | cec-mute              | mute                   |
| Source HDMI 1   | cec-hdmi 1            | txn 4f:82:10:00        |
| Source HDMI 2   | cec-hdmi 2            | txn 4f:82:20:00        |
| Source HDMI {n} | cec-hdmi {n}          | txn 4f:82:{n}0:00      |
| n/a             | exit                  | n/a                    !


The cec-client commands are issued as in this example (turn TV on):

    echo "on 0" | cec-client -s -d 1
	  
## Disable Raspberry PI screen saver

If you are switching to the HDMI input source corresponding to the Raspberry PI, it might be that the PI screen saver is activated and then you see only a blank screen. To avoid this, it is possible to permanently disable Raspberry PI screen saver. Edit this file to do it:

    sudo nano /etc/lightdm/lightdm.conf
	 
add the following line under the [Seat:*] section: 

    xserver-command=X -s 0 dpms

After a reboot, the screen saver will be permanently disabled (to re-enable screen saver remove the line and reboot).