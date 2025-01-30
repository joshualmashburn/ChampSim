#include "../../inc/trace_instruction.h"
#include "dr_api.h"
#include "dr_defines.h"
#include "drmgr.h"
#include "droption.h"
//#include "drreg.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <string>

using trace_instr_format_t = input_instr;

/* ================================================================== */
// Global variables
/* ================================================================== */

static uint64 instrCount = 0;

std::ofstream outfile;

trace_instr_format_t curr_instr;

/* ===================================================================== */
// Command line switches
/* ===================================================================== */
using ::dynamorio::droption::DROPTION_SCOPE_CLIENT;
using ::dynamorio::droption::droption_t;

static droption_t<std::string> outputFile(DROPTION_SCOPE_CLIENT, "o", "champsim.trace", "output file name", "specify file name for Champsim tracer output");

static droption_t<uint64_t> skipInstructions(DROPTION_SCOPE_CLIENT, "s", 0, "skipped instructions", "How many instructions to skip before tracing begins");

static droption_t<uint64_t> traceInstructions(DROPTION_SCOPE_CLIENT, "t", 1000000, "traced instructions", "How many instructions to trace");

/* ===================================================================== */
// Utilities
/* ===================================================================== */

/*!
 *  Print out help message.
 */
int32_t Usage()
{
    dr_fprintf(stderr, "This tool creates a register and memory access trace\n\
                        Specify the output trace file with -o\n\
                        Specify the number of instructions to skip before tracing with -s\n\
                        Specify the number of instructions to trace with -t\n");

  return -1;
}

/* ===================================================================== */
// Analysis routines
/* ===================================================================== */

static void Fini() {
    outfile.close();
    drmgr_unregister_bb_insertion_event(event_app_instruction);
    drmgr_exit();
}


static void
ConditionalBranch(app_pc inst_addr, app_pc targ_addr, int taken)
{
    curr_instr.is_branch = 1;
    curr_instr.branch_taken = taken;
}

static void WriteCurrentInstruction()
{
    typename decltype(outfile)::char_type buf[sizeof(trace_instr_format_t)];
    memcpy(buf, &curr_instr, sizeof(trace_instr_format_t));
    outfile.write(buf, sizeof(trace_instr_format_t));
}


static dr_emit_flags_t event_app_instruction(
    void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
    bool for_trace, bool translating, void *user_data)
{
    instrCount++;
    if ((instrCount > skipInstructions.get_value()) && (instrCount <= (traceInstructions.get_value() + skipInstructions.get_value()))) {
        curr_instr = {};
        curr_instr.ip = (unsigned long long int)instr_get_app_pc(instr);

        if(instr_is_call(instr) || instr_is_return(instr) || instr_is_ubr(instr) || instr_is_mbr(instr)) {
            curr_instr.is_branch = 1;
            curr_instr.branch_taken = 1;
        }
        else if (instr_is_cbr(instr)) {
            dr_insert_cbr_instrumentation(drcontext, bb, instr, ConditionalBranch);
        }

        uint32_t usedRegCount = 0;
        for (int i = 0; i < instr_num_srcs(instr); ++i) {
            opnd_t sourceOperand = instr_get_src(instr, i);
            if(opnd_is_reg(sourceOperand)){
                curr_instr.source_registers[usedRegCount++] = (unsigned char)opnd_get_reg(sourceOperand);
            }
        }
        usedRegCount = 0;
        for (int i = 0; i < instr_num_dsts(instr); ++i) {
            opnd_t destOperand = instr_get_dst(instr, i);
            if(opnd_is_reg(destOperand)){
                curr_instr.destination_registers[usedRegCount++] = (unsigned char)opnd_get_reg(destOperand);
            }
        }

        uint32_t usedMemCount = 0;
        if (instr_reads_memory(instr)) {
            for (int i = 0; i < instr_num_srcs(instr); ++i) {
                opnd_t sourceOperand = instr_get_src(instr, i);
                if (opnd_is_memory_reference(sourceOperand)) {
                    curr_instr.source_memory[usedMemCount++] = (unsigned long long int)opnd_get_addr(sourceOperand);
                }
            }
        }
        usedMemCount = 0;
        if (instr_writes_memory(instr)) {
            for (int i = 0; i < instr_num_dsts(instr); ++i) {
                opnd_t destOperand = instr_get_dst(instr, i);
                if (opnd_is_memory_reference(destOperand)) {
                    curr_instr.destination_memory[usedMemCount++] = (unsigned long long int)opnd_get_addr(destOperand);
                }
            }
        }

        //write current instruction
        dr_insert_clean_call(drcontext, bb, instr, WriteCurrentInstruction, false /*save FP state*/, 0);
        // Note: save_fpstate bool is ignored on non-x86 ISAs (including x86-64).
        //       The state to which it refers is the x87 and MMX state,
        //       neither of which are used in x86_64.
        //       The cost to save this state is 512 bytes on the DR stack.
    }
    else if (instrCount > (traceInstructions.get_value() + skipInstructions.get_value())) {
        dr_exit_process(0);
    }
    
    
    return DR_EMIT_DEFAULT;
}



DR_EXPORT void dr_client_main(client_id_t id, int argc, const char * argv[]) {
    using ::dynamorio::droption::droption_parser_t;
    std::string parse_err;
    int arg_error_index; //problematic argument index (for error reporting)
    if (!droption_parser_t::parse_argv(DROPTION_SCOPE_CLIENT, argc, argv, &parse_err, &arg_error_index)) {
        Usage();
        dr_abort_with_code(1);
    }

    if (!drmgr_init()){
        dr_fprintf(stderr, "Failed to initialize drmgr. Exiting.\n");
        dr_abort_with_code(1);
    }

    outfile.open(outputFile.get_value().c_str(), std::ios_base::binary | std::ios_base::trunc);
    if (!outfile) {
        std::cout << "Couldn't open output trace file. Exiting." << std::endl;
        dr_abort_with_code(1);
    }
    
    // Register function to be called when the application exits
    dr_register_exit_event(Fini);

    //register instrumentation and analysis functions
    //if (!drmgr_register_bb_instrumentation_event(event_bb_analysis, event_app_instruction, NULL))
    //    DR_ASSERT(false);
    if (!drmgr_register_bb_instrumentation_event(NULL, event_app_instruction, NULL)){
        dr_fprintf(stderr, "Failed to register instrumentation function. Exiting.\n");
        dr_abort_with_code(1);
    }
}
