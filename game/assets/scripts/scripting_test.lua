signals = {}

function signals.dummy()
end

function signals.open(target)
    signal(target, "open")
end

function signals.close(target)
    signal(target, "close")
end

function signals.device(callbacks)
    return function(event)
        local callback = callbacks[event]
        if callback then
            callback()
        else
            print("unknown event " .. event)
        end
    end
end

function signals.button(onPress, onRelease)
    return signals.device {
        press = onPress or signals.dummy,
        release = onRelease or signals.dummy
    }
end

function signals.buttonPress(onPress)
    return signals.button(onPress, nil)
end

function signals.buttonRelease(onRelease)
    return signals.button(nil, onRelease)
end

function signals.trigger(onEnter, onExit)
    return signals.device {
        enter = onEnter or signals.dummy,
        exit = onExit or signals.dummy
    }
end

function signals.triggerEnter(onEnter)
    return signals.trigger(onEnter, nil)
end

function signals.triggerExit(onExit)
    return signals.trigger(nil, onExit)
end

button1 = signals.buttonRelease(function()
    signals.open "door1"
    loadMap "maps/scripting_test.haru"
end)

button2 = signals.buttonRelease(function()
    signals.close "door1"
end)

button3 = signals.button(function()
    signals.open "door1"
end, function()
    signals.close "door1"
end)

trigger1 = signals.trigger(function()
    signals.open "door1"
end, function()
    signals.close "door1"
end)

trigger1_first = signals.triggerEnter(function()
    print "First time entering the trigger"
end)

playAudio "event:/ambience/wind_calm"
