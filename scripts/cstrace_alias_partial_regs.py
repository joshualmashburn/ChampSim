#
#    Copyright 2024 The ChampSim Contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This script is used to correct partial register references in existing traces
# created by the ChampSim tracer Pintool. The script checks each register
# reference and replaces it with the full register (e.g., AL -> RAX, AX -> RAX)

# Usage: python3 cstrace_alias_partial_regs.py <input_trace.xz> <output_trace.xz>

import lzma
import sys

# Intel Pin REG identifiers for x86_64
# (as of Pin 3.22, the version the ChampSim tracer uses as of 2024)
# NOTE: the REG_* values are defined in the Pin API, and are subject to change
# NOTE: the REG_* values differ between x86 and x86_64 builds of Pin
EG_INVALID_= 0
REG_NONE = 1
REG_FIRST = 2
REG_RBASE = 3
REG_MACHINE_BASE = 3
REG_APPLICATION_BASE = 3
REG_PHYSICAL_INTEGER_BASE = 3
REG_TO_SPILL_BASE = 3
REG_GR_BASE = 3
REG_RDI = 3
REG_GDI = 3
REG_RSI = 4
REG_GSI = 4
REG_RBP = 5
REG_GBP = 5
REG_RSP = 6
REG_STACK_PTR = 6
REG_RBX = 7
REG_GBX = 7
REG_RDX = 8
REG_GDX = 8
REG_RCX = 9
REG_GCX = 9
REG_RAX = 10
REG_GAX = 10
REG_R8 = 11
REG_R9 = 12
REG_R10 = 13
REG_R11 = 14
REG_R12 = 15
REG_R13 = 16
REG_R14 = 17
REG_R15 = 18
REG_GR_LAST = 18
REG_SEG_BASE = 19
REG_SEG_CS = 19
REG_SEG_SS = 20
REG_SEG_DS = 21
REG_SEG_ES = 22
REG_SEG_FS = 23
REG_SEG_GS = 24
REG_SEG_LAST = 24
REG_RFLAGS = 25
REG_GFLAGS = 25
REG_RIP = 26
REG_INST_PTR = 26
REG_PHYSICAL_INTEGER_END = 26
REG_AL = 27
REG_AH = 28
REG_AX = 29
REG_CL = 30
REG_CH = 31
REG_CX = 32
REG_DL = 33
REG_DH = 34
REG_DX = 35
REG_BL = 36
REG_BH = 37
REG_BX = 38
REG_BP = 39
REG_SI = 40
REG_DI = 41
REG_SP = 42
REG_FLAGS = 43
REG_IP = 44
REG_EDI = 45
REG_DIL = 46
REG_ESI = 47
REG_SIL = 48
REG_EBP = 49
REG_BPL = 50
REG_ESP = 51
REG_SPL = 52
REG_EBX = 53
REG_EDX = 54
REG_ECX = 55
REG_EAX = 56
REG_EFLAGS = 57
REG_EIP = 58
REG_R8B = 59
REG_R8W = 60
REG_R8D = 61
REG_R9B = 62
REG_R9W = 63
REG_R9D = 64
REG_R10B = 65
REG_R10W = 66
REG_R10D = 67
REG_R11B = 68
REG_R11W = 69
REG_R11D = 70
REG_R12B = 71
REG_R12W = 72
REG_R12D = 73
REG_R13B = 74
REG_R13W = 75
REG_R13D = 76
REG_R14B = 77
REG_R14W = 78
REG_R14D = 79
REG_R15B = 80
REG_R15W = 81
REG_R15D = 82
REG_MM_BASE = 83
REG_MM0 = 83
REG_MM1 = 84
REG_MM2 = 85
REG_MM3 = 86
REG_MM4 = 87
REG_MM5 = 88
REG_MM6 = 89
REG_MM7 = 90
REG_MM_LAST = 90
REG_XMM_BASE = 91
REG_FIRST_FP_REG = 91
REG_XMM0 = 91
REG_XMM1 = 92
REG_XMM2 = 93
REG_XMM3 = 94
REG_XMM4 = 95
REG_XMM5 = 96
REG_XMM6 = 97
REG_XMM7 = 98
REG_XMM8 = 99
REG_XMM9 = 100
REG_XMM10 = 101
REG_XMM11 = 102
REG_XMM12 = 103
REG_XMM13 = 104
REG_XMM14 = 105
REG_XMM15 = 106
REG_XMM_SSE_LAST = 106
REG_XMM_AVX_LAST = 106
REG_XMM_AVX512_HI16_FIRST = 107
REG_XMM16 = 107
REG_XMM17 = 108
REG_XMM18 = 109
REG_XMM19 = 110
REG_XMM20 = 111
REG_XMM21 = 112
REG_XMM22 = 113
REG_XMM23 = 114
REG_XMM24 = 115
REG_XMM25 = 116
REG_XMM26 = 117
REG_XMM27 = 118
REG_XMM28 = 119
REG_XMM29 = 120
REG_XMM30 = 121
REG_XMM31 = 122
REG_XMM_AVX512_HI16_LAST = 122
REG_XMM_AVX512_LAST = 122
REG_XMM_LAST = 122
REG_YMM_BASE = 123
REG_YMM0 = 123
REG_YMM1 = 124
REG_YMM2 = 125
REG_YMM3 = 126
REG_YMM4 = 127
REG_YMM5 = 128
REG_YMM6 = 129
REG_YMM7 = 130
REG_YMM8 = 131
REG_YMM9 = 132
REG_YMM10 = 133
REG_YMM11 = 134
REG_YMM12 = 135
REG_YMM13 = 136
REG_YMM14 = 137
REG_YMM15 = 138
REG_YMM_AVX_LAST = 138
REG_YMM_AVX512_HI16_FIRST = 139
REG_YMM16 = 139
REG_YMM17 = 140
REG_YMM18 = 141
REG_YMM19 = 142
REG_YMM20 = 143
REG_YMM21 = 144
REG_YMM22 = 145
REG_YMM23 = 146
REG_YMM24 = 147
REG_YMM25 = 148
REG_YMM26 = 149
REG_YMM27 = 150
REG_YMM28 = 151
REG_YMM29 = 152
REG_YMM30 = 153
REG_YMM31 = 154
REG_YMM_AVX512_HI16_LAST = 154
REG_YMM_AVX512_LAST = 154
REG_YMM_LAST = 154
REG_ZMM_BASE = 155
REG_ZMM0 = 155
REG_ZMM1 = 156
REG_ZMM2 = 157
REG_ZMM3 = 158
REG_ZMM4 = 159
REG_ZMM5 = 160
REG_ZMM6 = 161
REG_ZMM7 = 162
REG_ZMM8 = 163
REG_ZMM9 = 164
REG_ZMM10 = 165
REG_ZMM11 = 166
REG_ZMM12 = 167
REG_ZMM13 = 168
REG_ZMM14 = 169
REG_ZMM15 = 170
REG_ZMM_AVX512_SPLIT_LAST = 170
REG_ZMM_AVX512_HI16_FIRST = 171
REG_ZMM16 = 171
REG_ZMM17 = 172
REG_ZMM18 = 173
REG_ZMM19 = 174
REG_ZMM20 = 175
REG_ZMM21 = 176
REG_ZMM22 = 177
REG_ZMM23 = 178
REG_ZMM24 = 179
REG_ZMM25 = 180
REG_ZMM26 = 181
REG_ZMM27 = 182
REG_ZMM28 = 183
REG_ZMM29 = 184
REG_ZMM30 = 185
REG_ZMM31 = 186
REG_ZMM_AVX512_HI16_LAST = 186
REG_ZMM_AVX512_LAST = 186
REG_ZMM_LAST = 186
REG_K_BASE = 187
REG_K0 = 187
REG_IMPLICIT_FULL_MASK = 187
REG_K1 = 188
REG_K2 = 189
REG_K3 = 190
REG_K4 = 191
REG_K5 = 192
REG_K6 = 193
REG_K7 = 194
REG_K_LAST = 194
REG_TMM0 = 195
REG_TMM1 = 196
REG_TMM2 = 197
REG_TMM3 = 198
REG_TMM4 = 199
REG_TMM5 = 200
REG_TMM6 = 201
REG_TMM7 = 202
REG_TMM_FIRST = 195
REG_TMM_LAST = 202
REG_TILECONFIG = 203
REG_MXCSR = 204
REG_MXCSRMASK = 205
REG_ORIG_RAX = 206
REG_ORIG_GAX = 206
REG_FPST_BASE = 207
REG_FPSTATUS_BASE = 207
REG_FPCW = 207
REG_FPSW = 208
REG_FPTAG = 209
REG_FPIP_OFF = 210
REG_FPIP_SEL = 211
REG_FPOPCODE = 212
REG_FPDP_OFF = 213
REG_FPDP_SEL = 214
REG_FPSTATUS_LAST = 214
REG_ST_BASE = 215
REG_ST0 = 215
REG_ST1 = 216
REG_ST2 = 217
REG_ST3 = 218
REG_ST4 = 219
REG_ST5 = 220
REG_ST6 = 221
REG_ST7 = 222
REG_ST_LAST = 222
REG_FPST_LAST = 222
REG_DR_BASE = 223
REG_DR0 = 223
REG_DR1 = 224
REG_DR2 = 225
REG_DR3 = 226
REG_DR4 = 227
REG_DR5 = 228
REG_DR6 = 229
REG_DR7 = 230
REG_DR_LAST = 230
REG_CR_BASE = 231
REG_CR0 = 231
REG_CR1 = 232
REG_CR2 = 233
REG_CR3 = 234
REG_CR4 = 235
REG_CR_LAST = 235
REG_TSSR = 236
REG_LDTR = 237
REG_TR_BASE = 238
REG_TR = 238
REG_TR3 = 239
REG_TR4 = 240
REG_TR5 = 241
REG_TR6 = 242
REG_TR7 = 243
REG_TR_LAST = 243
REG_MACHINE_LAST = 243
REG_STATUS_FLAGS = 244
REG_DF_FLAG = 245
REG_APPLICATION_LAST = 245
# end of REGs

def alias_partial_regs(regID):
    if REG_AL <= regID <= REG_BX or REG_EBX <= regID <= REG_EAX:
        if REG_AL <= regID <= REG_AX or regID == REG_EAX:
            return REG_RAX
        elif REG_CL <= regID <= REG_CX or regID == REG_ECX:
            return REG_RCX
        elif REG_DL <= regID <= REG_DX or regID == REG_EDX:
            return REG_RDX
        elif REG_BL <= regID <= REG_BX or regID == REG_EBX:
            return REG_RBX
        else:
            print("ERROR: Unhandled register ID in bottom 4 GPRs:", regID)
            exit(1)
    elif REG_BP <= regID <= REG_EIP:
        if regID == REG_BP or regID == REG_EBP or regID == REG_BPL:
            return REG_RBP
        elif regID == REG_SI or regID == REG_ESI or regID == REG_SIL:
            return REG_RSI
        elif regID == REG_DI or regID == REG_EDI or regID == REG_DIL:
            return REG_RDI
        elif regID == REG_SP or regID == REG_ESP or regID == REG_SPL:
            return REG_RSP
        elif regID == REG_IP or regID == REG_EIP:
            return REG_RIP
        elif regID == REG_FLAGS or regID == REG_EFLAGS:
            return REG_RFLAGS
        else:
            print("ERROR: Unhandled register ID in index/stack/IP/BP/flags:", regID)
            exit(1)
    elif REG_R8B <= regID <= REG_R15D:
        if REG_R8B <= regID <= REG_R8D:
            return REG_R8
        elif REG_R9B <= regID <= REG_R9D:
            return REG_R9
        elif REG_R10B <= regID <= REG_R10D:
            return REG_R10
        elif REG_R11B <= regID <= REG_R11D:
            return REG_R11
        elif REG_R12B <= regID <= REG_R12D:
            return REG_R12
        elif REG_R13B <= regID <= REG_R13D:
            return REG_R13
        elif REG_R14B <= regID <= REG_R14D:
            return REG_R14
        elif REG_R15B <= regID <= REG_R15D:
            return REG_R15
        else:
            print("ERROR: Unhandled register ID in upper 8 GPRs:", regID)
            exit(1)
    elif REG_XMM_BASE <= regID <= REG_YMM_LAST:
        if REG_XMM_BASE <= regID <= REG_XMM_LAST:
            return REG_ZMM_BASE + (regID - REG_XMM_BASE)
        else: # REG_YMM_BASE <= regID <= REG_YMM_LAST
            return REG_ZMM_BASE + (regID - REG_YMM_BASE)
    elif REG_ST_BASE <= regID <= REG_ST_LAST:
        return REG_MM_BASE + (regID - REG_ST_BASE)
    else:
        return regID

def alias_register_values(inputfile_path, outputfile_path):
    with lzma.open(outputfile_path, 'wb') as outfile:
        with lzma.open(inputfile_path, 'rb') as infile:
            while True:
                entry = infile.read(64)
                if len(entry) < 64:
                    break  # End of file

                entry = bytearray(entry)

                # Extract and adjust 6 unsigned bytes for registers
                for i in range(10, 16):
                    entry[i] = alias_partial_regs(entry[i]) % 256

                # Write the modified entry to the new file
                outfile.write(entry)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 cstrace_alias_partial_regs.py <input_trace.xz> <output_trace.xz>")
        sys.exit(1)

    infile_path = sys.argv[1]
    outfile_path = sys.argv[2]
    if infile_path == outfile_path:
        print("Error: Input and output file paths must be different.")
        exit(1)
    # check to see if output file already exists
    try:
        with lzma.open(outfile_path, 'rb') as f:
            print(f"Error: Output file {outfile_path} already exists.")
            sys.exit(1)
    except FileNotFoundError:
        alias_register_values(infile_path, outfile_path)
