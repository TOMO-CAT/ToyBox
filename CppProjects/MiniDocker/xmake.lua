target("minidocker.mocker", function()
    set_kind("binary")
    add_files("mocker.cc")
end)

target("minidocker.mocker-fork", function()
    set_kind("binary")
    add_files("mocker-fork.cc")
end)

target("minidocker.mocker-namespace", function()
    set_kind("binary")
    add_files("mocker-namespace.cc")
end)
