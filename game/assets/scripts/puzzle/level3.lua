local signals = require("scripts/signals.lua")

ball_in_slot = false

slot1 = signals.trigger(function()
    if not ball_in_slot then
        ball_in_slot = true
        showObjective("Well done.")
    end
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
delay(2, function ()
    if not ball_in_slot then
        showObjective("Push the ball into the slot.")
    end
end)
