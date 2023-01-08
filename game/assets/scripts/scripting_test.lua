function button1(event)
    if event == "release" then
        signal("door1", "open")
    end
end

function button2(event)
    if event == "release" then
        signal("door1", "close")
    end
end

function button3(event)
    if event == "press" then
        signal("door1", "open")
    elseif event == "release" then
        signal("door1", "close")
    else
        print("unknown event " .. event)
    end
end

function trigger1(event)
    if event == "enter" then
        signal("door1", "open")
    elseif event == "exit" then
        signal("door1", "close")
    else
        print("unknown event " .. event)
    end
end

load_audio_bank("audio/Master.bank")
load_audio_bank("audio/Master.strings.bank")
play_audio_one_shot("event:/ambience/wind_calm")
