function button1()
    signal("door1", "open")
end

function button2()
    signal("door1", "close")
end

function button3()
    signal("door1", "toggle")
end

print("hello,world")
