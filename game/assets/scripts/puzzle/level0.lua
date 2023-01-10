local signals = require("scripts/signals.lua")

trigger_end = signals.triggerEnter(function()
    loadMap("maps/puzzle/level1.haru")
end)

playAudio("event:/ambience/wind_calm")
