import os
import shutil
import time

print('*************************************************')
print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
print('Start, please wait...')

dir_C = os.getcwd()

def C2I():
    print('**********C2I**********')
    files_pulpino = os.listdir(dir_C + '/pulpino/file_c')
    files_rtthread_cpu = os.listdir(dir_C+'/rtthread/libcpu/risc-v')
    files_rtthread_src = os.listdir(dir_C+'/rtthread/src')

    path = './file_i'

    if(os.path.exists(path)):
        pass
    else:
        os.system('mkdir ' + path)

    path_pulpino = './file_i/pulpino_i'
    path_rtthread = './file_i/rtthread_i'

    if(os.path.exists(path_pulpino)):
        pass
    else:
        os.system('mkdir ' + path_pulpino)

    if(os.path.exists(path_rtthread)):
        pass
    else:
        os.system('mkdir ' + path_rtthread)


    for file_pulpino in files_pulpino:
        tmp = file_pulpino.split('.')
        if(len(tmp) == 1):
            pass
        elif(tmp[1].lower() == 'c'):
            print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/pulpino_i/' + tmp[0] + '.i ' + './pulpino/file_c/' + file_pulpino)
            os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/pulpino_i/' + tmp[0] + '.i ' + './pulpino/file_c/' + file_pulpino)

    for file_rtthread_cpu in files_rtthread_cpu:
        tmp = file_rtthread_cpu.split('.')
        if(len(tmp) == 1):
            pass
        elif(tmp[1].lower() == 'c'):
            print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/rtthread_i/' + tmp[0] + '.i ' + './rtthread/libcpu/risc-v/' + file_rtthread_cpu)
            os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/rtthread_i/' + tmp[0] + '.i ' + './rtthread/libcpu/risc-v/' + file_rtthread_cpu)

    for file_rtthread_src in files_rtthread_src:
        tmp = file_rtthread_src.split('.')
        if(len(tmp) == 1):
            pass
        elif(tmp[1].lower() == 'c'):
            print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/rtthread_i/' + tmp[0] + '.i ' + './rtthread/src/' + file_rtthread_src)
            os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/rtthread_i/' + tmp[0] + '.i ' + './rtthread/src/' + file_rtthread_src)

    print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ./file_i/rtthread_i/board.i ./rtthread/board.c')
    os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ./file_i/rtthread_i/board.i ./rtthread/board.c')

def I2S():
    print('**********I2S**********')
    files_pulpino = os.listdir(dir_C + '/file_i/pulpino_i')
    files_rtthread = os.listdir(dir_C+'/file_i/rtthread_i')

    path = './file_s'

    if(os.path.exists(path)):
        pass
    else:
        os.system('mkdir ' + path)

    path_pulpino = './file_s/pulpino_s'
    path_rtthread = './file_s/rtthread_s'

    if(os.path.exists(path_pulpino)):
        pass
    else:
        os.system('mkdir ' + path_pulpino)

    if(os.path.exists(path_rtthread)):
        pass
    else:
        os.system('mkdir ' + path_rtthread)

    shutil.copy('./pulpino/file_s/crt0_riscv_E.S',path_pulpino)
    shutil.copy('./pulpino/file_s/pulpino.h',path_pulpino)
    #shutil.copy('./rtthread/libcpu/risc-v/context_gcc.S',path_rtthread)
    #shutil.copy('./rtthread/libcpu/risc-v/interrupt_gcc.S',path_rtthread)
    #shutil.copy('./rtthread/libcpu/risc-v/cpuport.h',path_rtthread)


    for file_pulpino in files_pulpino:
        tmp = file_pulpino.split('.')
        if(len(tmp) == 1):
            pass
        elif(tmp[1].lower() == 'i'):
            print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -S -o ' + './file_s/pulpino_s/' + tmp[0] + '.s ' + './file_i/pulpino_i/' + file_pulpino)
            os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -S -o ' + './file_s/pulpino_s/' + tmp[0] + '.s ' + './file_i/pulpino_i/' + file_pulpino)

    for file_rtthread in files_rtthread:
        tmp = file_rtthread.split('.')
        if(len(tmp) == 1):
            pass
        elif(tmp[1].lower() == 'i'):
            print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -S -o ' + './file_s/rtthread_s/' + tmp[0] + '.s ' + './file_i/rtthread_i/' + file_rtthread)
            os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -S -o ' + './file_s/rtthread_s/' + tmp[0] + '.s ' + './file_i/rtthread_i/' + file_rtthread)

def S2O():
    print('**********S2O**********')
    files_pulpino = os.listdir(dir_C + '/file_s/pulpino_s')
    files_rtthread = os.listdir(dir_C+'/file_s/rtthread_s')

    path = './file_o'

    if(os.path.exists(path)):
        pass
    else:
        os.system('mkdir ' + path)

    path_pulpino = './file_o/pulpino_o'
    path_rtthread = './file_o/rtthread_o'

    if(os.path.exists(path_pulpino)):
        pass
    else:
        os.system('mkdir ' + path_pulpino)

    if(os.path.exists(path_rtthread)):
        pass
    else:
        os.system('mkdir ' + path_rtthread)


    for file_pulpino in files_pulpino:
        tmp = file_pulpino.split('.')
        if(len(tmp) == 1):
            pass
        elif(tmp[1].lower() == 's'):
            print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c -o ' + './file_o/pulpino_o/' + tmp[0] + '.o ' + './file_s/pulpino_s/' + file_pulpino)
            os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c -o ' + './file_o/pulpino_o/' + tmp[0] + '.o ' + './file_s/pulpino_s/' + file_pulpino)

    for file_rtthread in files_rtthread:
        tmp = file_rtthread.split('.')
        if(len(tmp) == 1):
            pass
        elif(tmp[1].lower() == 's'):
            print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c -o ' + './file_o/rtthread_o/' + tmp[0] + '.o ' + './file_s/rtthread_s/' + file_rtthread)
            os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c -o ' + './file_o/rtthread_o/' + tmp[0] + '.o ' + './file_s/rtthread_s/' + file_rtthread)
    print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c -o ./file_o/rtthread_o/context_gcc.o ./rtthread/libcpu/risc-v/context_gcc.S')
    os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c -o ./file_o/rtthread_o/context_gcc.o ./rtthread/libcpu/risc-v/context_gcc.S')
    print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c -o ./file_o/rtthread_o/interrupt_gcc.o ./rtthread/libcpu/risc-v/interrupt_gcc.S')
    os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c -o ./file_o/rtthread_o/interrupt_gcc.o ./rtthread/libcpu/risc-v/interrupt_gcc.S')

def Ld():
    cmd = 'riscv32-unknown-elf-ld -T link.ld '

    path_pulpino = dir_C + '/file_o/pulpino_o'
    path_rtthread = dir_C + '/file_o/rtthread_o'

    files_pulpino = os.listdir(path_pulpino)
    files_rtthread = os.listdir(path_rtthread)

    for file_pulpino in files_pulpino:
        tmp = file_pulpino.split('.')
        if(len(tmp) == 2 and tmp[1] == 'o'):
            cmd = cmd + './file_o/pulpino_o/' + file_pulpino+ ' '
        else:
            pass
    
    for file_rtthread in files_rtthread:
        tmp = file_rtthread.split('.')
        if(len(tmp) == 2 and tmp[1] == 'o'):
            cmd = cmd + './file_o/rtthread_o/' + file_rtthread + ' '
        else:
            pass
    
    cmd = cmd + 'libgcc.a libc.a -o hello'
    print('**********link**********')
    print(cmd)
    os.system(cmd)
    print('riscv32-unknown-elf-objcopy -O ihex hello helloworld.hex')
    os.system('riscv32-unknown-elf-objcopy -O ihex hello helloworld.hex')


C2I()
I2S()
S2O()
Ld()
print('**********final process**********')
print('riscv32-unknown-elf-objdump -D hello > obj.s')
os.system('riscv32-unknown-elf-objdump -D hello > obj.s')
print('python hex2hex2.py')
os.system('python hex2hex2.py')
print('Done!')