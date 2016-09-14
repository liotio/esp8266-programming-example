import machine
import network
import socket

led_pin = machine.Pin(2,machine.Pin.OUT)
led_pin.high()

led_active = 0

sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect('SSID', 'PASS')

address = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
socket = socket.socket()
socket.bind(address)
socket.listen(1)

print(sta_if.ifconfig()[0])

while True:
    client, address = socket.accept()
    print('client connected from', address)
    req = client.recv(1024)
    
    if 'GET / ' in req:
        if led_active == 0:
            led_pin.low()
            led_active = 1
            client.send('LED is now ON')
        else:
            led_pin.high()
            led_active = 0
            client.send('LED is now OFF')
    
    client.close()