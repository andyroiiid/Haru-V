function button1()
    signal("door1", "open")
end

function button2()
    signal("door1", "close")
end

function button3()
    signal("door1", "toggle")
end

load_audio_bank("audio/Master.bank")
load_audio_bank("audio/Master.strings.bank")
play_audio_one_shot("event:/ambience/wind_calm")
