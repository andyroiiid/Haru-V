local signals = require("scripts/signals.lua")

trigger_end = signals.triggerEnter(function()
    delay(3, function ()
        loadMap("maps/puzzle/level1.haru")
    end)
end)

playAudio("event:/ambience/wind_calm")
