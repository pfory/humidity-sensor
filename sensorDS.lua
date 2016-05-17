--Init  
bootTime = tmr.now()
base = "/home/sklenik/esp10/"
deviceID = "ESP8266 Sklenik "..node.chipid()

wifi.setmode(wifi.STATION)
wifi.sta.config("Datlovo","Nu6kMABmseYwbCoJ7LyG")
wifi.setphymode(wifi.PHYMODE_N)

--- INTERVAL ---
-- In seconds. Remember that the sensor reading, 
-- reboot and wifi reconnect takes a few seconds
time_between_sensor_readings = 900

Broker="88.146.202.186"  

versionSW         = 0.23
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
  napeti = adc.readvdd33()/1000
  if napeti==nil then napeti=0 end

  print("Humidity: "..h.." %")
  print("Temperature: "..t.." C")
  print("Voltage:"..napeti.." V")
  m:publish(base.."temperature",                    string.format("%.1f",t),0,0, function(conn)
    m:publish(base.."humidity",                     string.format("%.1f",h),0,0, function(conn)
      m:publish(base.."voltage",                    string.format("%.2f",napeti),0,0, function(conn)   
        m:publish(base.."VersionSW",                string.format("%.2f",versionSW),0,0, function(conn)   
          print("Boot time:"..tmr.now()-bootTime)
          time_between_sensor_readings = time_between_sensor_readings - ((tmr.now()-bootTime) / 1000000)
          print("Going to sleep for "..time_between_sensor_readings.." sec.")
          node.dsleep(time_between_sensor_readings*1000000)
        end)
      end)
    end)
  end)
end

m = mqtt.Client(deviceID, 180, "datel", "hanka12")  

<<<<<<< HEAD
start=tmr.now()

tmr.alarm(0, 1000, tmr.ALARM_AUTO, function() 
  if (tmr.now()-start)>20000000 then //pojistna funkce pri nedostupnosti site aby se nevybili baterie
    print("nelze se spojit tak hajiiiiiii")
    node.dsleep(600000000)
  end
  print ("Connecting to Wifi... ")
  if wifi.sta.status() == 5 and wifi.sta.getip() ~= nil then 
    print ("Wifi connected")
    tmr.stop(0) 
=======



uart.write(0,"Connecting to Wifi")
function loop() 
  uart.write(0,".")
  if wifi.sta.status() == 5 then
    -- Stop the loop
    tmr.stop(0)
>>>>>>> branch 'master' of https://github.com/pfory/humidity-sensor.git
    m:connect(Broker, 31883, 0, function(conn) 
      print("Connected to MQTT")
      sendData()
    end )
  end
end
        
tmr.alarm(0, 1000, 1, function() loop() end)