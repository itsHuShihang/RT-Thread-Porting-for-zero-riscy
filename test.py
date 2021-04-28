import os


dir_C = os.getcwd()

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
        #os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' './file_i/pulpino_i/' + tmp[0] + '.i ' + './pulpino/file_c/' + file_pulpino)
        print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/pulpino_i/' + tmp[0] + '.i ' + './pulpino/file_c/' + file_pulpino)

for file_rtthread_cpu in files_rtthread_cpu:
    tmp = file_rtthread_cpu.split('.')
    if(len(tmp) == 1):
        pass
    elif(tmp[1].lower() == 'c'):
        #os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' './file_i/pulpino_i/' + tmp[0] + '.i ' + './pulpino/file_c/' + file_pulpino)
        print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/rtthread_i/' + tmp[0] + '.i ' + './rtthread/libcpu/risc-v/' + file_rtthread_cpu)

for file_rtthread_src in files_rtthread_src:
    tmp = file_rtthread_src.split('.')
    if(len(tmp) == 1):
        pass
    elif(tmp[1].lower() == 'c'):
        #os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' './file_i/pulpino_i/' + tmp[0] + '.i ' + './pulpino/file_c/' + file_pulpino)
        print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/rtthread_i/' + tmp[0] + '.i ' + './rtthread/src/' + file_rtthread_src)

os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' './file_i/pulpino_i/' + tmp[0] + '.i ' + './pulpino/file_c/' + file_pulpino)
print('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' + './file_i/rtthread_i/board.i ./rtthread/board.c')