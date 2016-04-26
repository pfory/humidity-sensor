--Init  
base = "/home/sklenik/esp10/"
deviceID = "ESP8266 Sklenik "..node.chipid()

pulseTotal        = 0
heartBeat         = node.bootreason() + 10
pulseDuration     = 0

wifi.setmode(wifi.STATION)
wifi.sta.config("Datlovo","Nu6kMABmseYwbCoJ7LyG")
wifi.sta.autoconnect(1)

Broker="88.146.202.186"  

versionSW         = 0.1
versionSWString   = "Sklenik v" 
print(versionSWString .. versionSW)

si7021 = require("si7021")
SDA_PIN = 6 -- sda pin, GPIO12
SCL_PIN = 5 -- scl pin, GPIO14
si7021.init(SDA_PIN, SCL_PIN)

function sendData()
  si7021.read(OSS)
  h = si7021.getHumidity()
  t = si7021.getTemperature()

  print("Humidity: "..(h / 100).." %")
  print("Temperature: "..(t/100).." deg C")
  print("I am sending data to OpenHab")
  
  m:publish(base.."temperature",       string.format("%.1f",t/100),0,0)  
  m:publish(base.."humidity",          string.format("%.1f",h/100),0,0)  
end

function mqtt_sub()  
  m:subscribe(base,0, function(conn)   
    print("Mqtt Subscribed to OpenHAB feed for device "..deviceID)  
  end)  
end  


function reconnect()
  print ("Waiting for Wifi")
  if wifi.sta.status() == 5 and wifi.sta.getip() ~= nil then 
    print ("Wifi Up!")
    tmr.stop(1) 
    m:connect(Broker, 31883, 0, function(conn) 
      print(wifi.sta.getip())
      print("Mqtt Connected to:" .. Broker) 
      mqtt_sub() --run the subscription function 
    end)
  end
end

m = mqtt.Client(deviceID, 180, "datel", "hanka12")  
m:lwt("/lwt", deviceID, 0, 0)  
m:on("offline", function(con)   
  print ("Mqtt Reconnecting...")   
  tmr.alarm(1, 10000, 1, function()  
    reconnect()
  end)
end)  

 -- on publish message receive event  
m:on("message", function(conn, topic, data)   
  print("Received:" .. topic .. ":" .. data) 
end)  

tmr.alarm(0, 1000, 1, function() 
  print ("Connecting to Wifi... ")
  if wifi.sta.status() == 5 and wifi.sta.getip() ~= nil then 
    print ("Wifi connected")
    tmr.stop(0) 
    m:connect(Broker, 31883, 0, function(conn) 
      mqtt_sub() --run the subscription function 
      print(wifi.sta.getip())
      print("Mqtt Connected to:" .. Broker.." - "..base) 
      tmr.alarm(0, 20000, tmr.ALARM_AUTO, function()
        sendData() 
      end)
    end) 
  end
end)