# !usr/bin/python3

import sys

def ipc_makeheader(command_id: int, normal_params: int, translate_params: int) -> int:
    return (command_id << 16) | ((normal_params & 0x3F) << 6) | (translate_params & 0x3F)

if len(sys.argv) != 4:
    exit('bad args')

try:
    cmd_id = int(sys.argv[1])
    nor_pr = int(sys.argv[2])
    trn_pr = int(sys.argv[3])
except:
    exit('bad args')

header = ipc_makeheader(cmd_id, nor_pr, trn_pr)

print(f"IPC_MakeHeader({cmd_id}, {nor_pr}, {trn_pr}) == 0x{int.to_bytes(header, 4, 'big').hex()}")
