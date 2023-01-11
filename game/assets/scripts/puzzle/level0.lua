local signals = require("scripts/signals.lua")

trigger_end = signals.triggerEnter(function()
    fadeToBlack(2.1)
    delay(2, function ()
        loadMap("maps/puzzle/level1.haru")
    end)
end)

playAudio("event:/ambience/wind_calm")
fadeFromBlack(2)
