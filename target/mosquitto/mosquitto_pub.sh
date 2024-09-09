BROKER="192.168.123.1"
PORT="1883"
while true
    do
        mosquitto_pub -h $BROKER -p $PORT -t "system/date" -m "$(date +"%Y-%m-%d %H:%M:%S")"
        mosquitto_pub -h $BROKER -p $PORT -t "system/hostname" -m "$(hostname)"
        mosquitto_pub -h $BROKER -p $PORT -t "system/ipaddress/eth0" -m "$(ifconfig eth0 | grep "inet addr:" | awk '{print $2}' | cut -d: -f2)"
        mosquitto_pub -h $BROKER -p $PORT -t "system/ipaddress/wlan0" -m "$(ifconfig wlan0 | grep "inet addr:" | awk '{print $2}' | cut -d: -f2)"
        mosquitto_pub -h $BROKER -p $PORT -t "system/uptime" -m "$(uptime)"
        mosquitto_pub -h $BROKER -p $PORT -t "system/Sadkowski" -m "Dennis Sadkowski"
        mosquitto_pub -h $BROKER -p $PORT -t "appl/frequency" -m "$(head -c 2 /dev/led_one_ad)"
        sleep 15
    done  

