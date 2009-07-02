import os

env = Environment (CCFLAGS = '-g')

source_files = [
               'gtkdrop.c',
               'test.c'
               ]

libs_pkgconfig =    [
               'gtk+-2.0'
               ]

for lib in libs_pkgconfig:
    env.ParseConfig ('pkg-config --cflags --libs ' + lib)

test = env.Program (target = 'test', source = source_files)
