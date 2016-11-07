--Init  
bootTime                   = tmr.now()
base                       = "/home/sklenik/esp10/"
deviceID                   = "ESP8266 Sklenik "..node.chipid()
maxStartTime               = 10000 --10sec
sleepDelayWhenNoConnection = 3600000000 --1 hour
microsToSec                = 1000000
tempAndHumidityKoef        = 100
mVoltsToVolts              = 1000

--- INTERVAL ---
-- In seconds. Remember that the sensor reading, 
-- reboot and wifi reconnect takes a few seconds
time_between_sensor_readings = 3600  --1 hour


tmr.alarm(1, maxStartTime, tmr.ALARM_SINGLE, function()
  print("No connection. Going to sleep for 600 sec.")
  node.dsleep(sleepDelayWhenNoConnection)
end)

-- cfg=
-- {
  -- ip = "192.168.1.103",
  -- netmask = "255.255.255.0",
  -- gateway = "192.168.1.1"
-- }

-- wifi.sta.setip(cfg)
-- wifi.setmode(wifi.STATION)
-- wifi.sta.config("Datlovo","Nu6kMABmseYwbCoJ7LyG")
-- wifi.setphymode(wifi.PHYMODE_N)

if adc.force_init_mode(adc.INIT_VDD33)
then
  node.restart()
  return -- don't bother continuing, the restart is scheduled
end

Broker="192.168.1.56"  

versionSW         = 0.34
versionSWString   = "Sklenik v" 
print(versionSWString .. versionSW)

si7021 = require("si7021")
SDA_PIN = 6 -- sda pin, GPIO12
SCL_PIN = 5 -- scl pin, GPIO14
si7021.init(SDA_PIN, SCL_PIN)

function sendData()
  si7021.read(OSS)
  h = si7021.getHumidity()/tempAndHumidityKoef
  if (h>100) then h=100 end
  t = si7021.getTemperature()/tempAndHumidityKoef
  napeti = adc.readvdd33()/mVoltsToVolts
  if napeti==nil then napeti=0 end

  print("Humidity: "..h.." %")
  print("Temperature: "..t.." C")
  print("Voltage:"..napeti.." V")
  m:publish(base.."temperature",                    string.format("%.1f",t),0,0, function(conn)
    m:publish(base.."humidity",                     string.format("%.1f",h),0,0, function(conn)
      m:publish(base.."voltage",                    string.format("%.2f",napeti),0,0, function(conn)   
        m:publish(base.."bootTime",                 string.format("%.2f",(tmr.now()-bootTime)/microsToSec),0,0, function(conn)   
          m:publish(base.."VersionSW",              string.format("%.2f",versionSW),0,0, function(conn)   
            print("Boot time:"..((tmr.now()-bootTime)/microsToSec))
            time_between_sensor_readings = time_between_sensor_readings - ((tmr.now()-bootTime) / microsToSec)
            print("Going to sleep for "..time_between_sensor_readings.." sec.")
            node.dsleep(time_between_sensor_readings*microsToSec)
          end)
        end)
      end)
    end)
  end)
end

m = mqtt.Client(deviceID, 180, "datel", "hanka12")  

m:connect(Broker, 1883, 0, 1, function(conn) 
  --mqtt_sub() --run the subscription function 
  --print(wifi.sta.getip())
  print("Mqtt Connected to:" .. Broker.." - "..base) 
  sendData() 
end)


-- uart.write(0,"Connecting to Wifi")
-- tmr.alarm(0, 500, tmr.ALARM_AUTO, function()
--function loop() 
  -- uart.write(0,".")
  -- if wifi.sta.status() == 5 then
    --Stop the loop
    -- print ("Wifi connected")
    -- print(wifi.sta.getip())
    -- tmr.stop(0)
    -- m:connect(Broker, 1883, 0, function(conn) 
      -- print("Connected to MQTT")
      -- sendData()
    -- end )
  -- end
-- end)