local signals = require("scripts/signals.lua")

slot1 = signals.trigger(function()
    signals.open("door1")
end, function()
    signals.close("door1")
end)

trigger_end = signals.triggerEnter(function()
    fadeToBlack(2.1)
    delay(2, function ()
        loadMap("maps/puzzle/level0.haru")
    end)
end)

playAudio("event:/ambience/wind_calm")
fadeFromBlack(2)