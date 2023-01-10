local signals = require("scripts/signals.lua")

button1 = signals.buttonRelease(function()
    signals.open("door1")
end)

trigger_end = signals.triggerEnter(function()
    loadMap("maps/puzzle/level2.haru")
end)

playAudio("event:/ambience/wind_calm")
