# THIAGO ZILIO GRR202342

#!/usr/bin/env bash
# run_likwid.sh — saída normal +  FP_ARITH_INST_RETIRED_SCALAR_DOUBLE,valor
# Uso: ./run_likwid.sh [INPUT_FILE] [CPU]

INPUT=${1:-}    
CPU=${2:-3}     

if [[ -n "$INPUT" ]]; then
  ./resolveEDO < "$INPUT"
else
  ./resolveEDO
fi

if [[ -n "$INPUT" ]]; then
  likwid-perfctr -C "$CPU" -g FLOPS_DP -m -- ./resolveEDO < "$INPUT"
else
  likwid-perfctr -C "$CPU" -g FLOPS_DP -m -- ./resolveEDO
fi \
  | awk -F'|' '/FP_ARITH_INST_RETIRED_SCALAR_DOUBLE/ { 
  	gsub(/ /, "", $4); 
	print "FP_ARITH_INST_RETIRED_SCALAR_DOUBLE," $4 
  }'



