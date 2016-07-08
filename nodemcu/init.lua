-- pin D4 == pin 2
led_pin = 4
led_active = 0

gpio.mode(led_pin,gpio.OUTPUT)

wifi.setmode(wifi.STATION)
wifi.sta.config("SSID", "PASS")
wifi.sta.connect()

tmr.alarm(1, 500, 1, function()
     if wifi.sta.getip() ~= nil then
         tmr.stop(1)
         print("Connected, IP is "..wifi.sta.getip())
     end
end)

collectgarbage()

srv=net.createServer(net.TCP) 
srv:listen(80,function(conn) 
    conn:on("receive",function(sck,payload)
        print(payload)
        location = string.match(payload, "GET (%S*) HTTP")
        if location == "/" then
            if led_active==0 then
                gpio.write(led_pin,gpio.LOW)
                led_active = 1
                sck:send("LED is now ON")
            else
                gpio.write(led_pin,gpio.HIGH)
                led_active = 0
                sck:send("LED is now OFF")
            end
        end
        collectgarbage()
    end)
    conn:on("sent", function(sck)
        sck:close()
        collectgarbage()
    end)
end)
