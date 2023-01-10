local signals = require("scripts/signals.lua")

button1 = signals.button(function()
    signals.open("door1")
end, function()
    signals.close("door1")
end)

trigger_end = signals.triggerEnter(function()
    delay(3, function ()
        loadMap("maps/puzzle/level3.haru")
    end)
end)

playAudio("event:/ambience/wind_calm")
