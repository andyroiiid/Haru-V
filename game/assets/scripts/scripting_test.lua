local signals = require("scripts/signals.lua")

button1 = signals.buttonRelease(function()
    signals.open("door1")
    loadMap("maps/scripting_test.haru")
end)

button2 = signals.buttonRelease(function()
    signals.close("door1")
end)

button3 = signals.button(function()
    signals.open("door1")
end, function()
    signals.close("door1")
end)

trigger1 = signals.trigger(function()
    signals.open("door1")
end, function()
    signals.close("door1")
end)

trigger1_first = signals.triggerEnter(function()
    print("First time entering the trigger")
end)

playAudio("event:/ambience/wind_calm")
