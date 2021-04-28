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

for file in files:
    tmp = file.split('.')
    if(len(tmp) == 1):
        pass
    elif(tmp[1].lower() == 'c'):
        os.system('riscv32-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -E -o ' './file_i/' +
                  tmp[0] + '.i ' + './file_c/' + file)
        #print('riscv32-unknown-elf-gcc -march=rv32i -mabi=ilp32 -E -o ' + './file_i/' + tmp[0] + '.i ' + tmp[0] + '.c')
