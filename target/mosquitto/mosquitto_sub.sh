BROKER="192.168.123.1"
PORT="1883"
TOPIC="appl/frequency_set"

mosquitto_sub -h $BROKER -p $PORT -t $TOPIC > /dev/led_one_ad 
