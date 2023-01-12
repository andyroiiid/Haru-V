local signals = require("scripts/signals.lua")

button_pressed = false

button1 = signals.buttonRelease(function()
    if not button_pressed then
        button_pressed = true
        showObjective("Well done.")
    end
    signals.open("door1")
end)

trigger_end = signals.triggerEnter(function()
    fadeToBlack(2.1)
    delay(2, function ()
        loadMap("maps/puzzle/level2.haru")
    end)
end)

playAudio("event:/ambience/wind_calm")
fadeFromBlack(2)
delay(2, function ()
    if not button_pressed then
        showObjective("Press the button.")
    end
end)
