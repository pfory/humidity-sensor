--Init  
base = "/home/sklenik/esp10/"
deviceID = "ESP8266 Sklenik "..node.chipid()

wifi.setmode(wifi.STATION)
wifi.sta.config("Datlovo","Nu6kMABmseYwbCoJ7LyG")

Broker="88.146.202.186"  

versionSW         = 0.22
versionSWString   = "Sklenik v" 
print(versionSWString .. versionSW)

si7021 = require("si7021")
SDA_PIN = 6 -- sda pin, GPIO12
SCL_PIN = 5 -- scl pin, GPIO14
si7021.init(SDA_PIN, SCL_PIN)

function sendData()
  si7021.read(OSS)
  h = si7021.getHumidity()/100
  t = si7021.getTemperature()/100

  print("Humidity: "..h.." %")
  print("Temperature: "..t.." C")
  print("I am sending data to OpenHab")
  
  m:publish(base.."temperature",       string.format("%.1f",t),0,0)  
  m:publish(base.."humidity",          string.format("%.1f",h),0,0)  
  m:publish(base.."VersionSW",         VersionSW,0,0)  

  --tmr.delay(10000)

end

function mqtt_sub()  
  m:subscribe(base,0, function(conn)   
    print("Mqtt Subscribed to OpenHAB feed for device "..deviceID)  
  end)  
end  

m = mqtt.Client(deviceID, 180, "datel", "hanka12")  

tmr.alarm(0, 1000, tmr.ALARM_AUTO, function() 
  print ("Connecting to Wifi... ")
  if wifi.sta.status() == 5 and wifi.sta.getip() ~= nil then 
    print ("Wifi connected")
    tmr.stop(0) 
    m:connect(Broker, 31883, 0, function(conn) 
      mqtt_sub() --run the subscription function 
      print(wifi.sta.getip())
      print("Mqtt Connected to:" .. Broker.." - "..base) 
      tmr.alarm(1, 5000, tmr.ALARM_SINGLE, function()
        print("hajiiiiiii")
        node.dsleep(60000000)
      end)
      tmr.alarm(0, 5, tmr.ALARM_SINGLE, function()
        sendData() 
      end)
    end) 
  end
end)