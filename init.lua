print("Wait 1 seconds please")
tmr.alarm(0, 500, 0, function() dofile('sensorDS.lua') end)