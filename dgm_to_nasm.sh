nasm -f win64 program.s -o program.obj
link program.obj runtime.obj /SUBSYSTEM:CONSOLE
