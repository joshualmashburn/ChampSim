#include "dr_api.h"
#include "drmgr.h"
#include <string.h>
#include <stdio.h>

/* Thread-Local Storage index */
static int tls_idx;

/* Structure to buffer instruction details until the next instruction executes */
typedef struct {
    bool has_pending;
    app_pc pc;
    const char *mnemonic;
    bool is_branch;
    bool is_cbr;
    app_pc fallthrough_pc;

    int reg_src_count;
    char src_regs[256];
    int reg_dst_count;
    char dst_regs[256];

    int mem_load_count;
    char mem_loads[512];
    int mem_store_count;
    char mem_stores[512];
} thread_data_t;

/* Helper macro to safely append strings */
#define APPEND_STR(buf, ...) do { \
    size_t len = strlen(buf); \
    snprintf(buf + len, sizeof(buf) - len, __VA_ARGS__); \
} while(0)

/* Clean call executed right BEFORE every instruction */
static void analyze_inst(app_pc pc) {
    void *drcontext = dr_get_current_drcontext();
    thread_data_t *data = (thread_data_t *)drmgr_get_tls_field(drcontext, tls_idx);

    /* 1. If we have a buffered instruction, finish evaluating it and print it */
    if (data->has_pending) {
        char taken_str[16] = "N/A";
        
        if (data->is_branch) {
            if (data->is_cbr) {
                /* If the current PC is NOT the fall-through, the branch was taken */
                bool taken = (pc != data->fallthrough_pc);
                snprintf(taken_str, sizeof(taken_str), taken ? "Yes" : "No");
            } else {
                /* Unconditional branches (jmp, call, ret) are always taken */
                snprintf(taken_str, sizeof(taken_str), "Yes");
            }
        }

        dr_printf("PC: %p | %s | Branch: %s | Taken: %s | RegSrcs: %d [%s] | RegDsts: %d [%s] | MemLds: %d [%s] | MemSts: %d [%s]\n",
                  data->pc,
                  data->mnemonic,
                  data->is_branch ? "Yes" : "No", 
                  taken_str,
                  data->reg_src_count, data->src_regs,
                  data->reg_dst_count, data->dst_regs,
                  data->mem_load_count, data->mem_loads,
                  data->mem_store_count, data->mem_stores);
    }

    /* 2. Grab machine context to calculate dynamic memory addresses */
    dr_mcontext_t mc;
    mc.size = sizeof(mc);
    mc.flags = DR_MC_ALL;
    dr_get_mcontext(drcontext, &mc);

    /* 3. Decode the CURRENT instruction */
    instr_t *instr = instr_create(drcontext);
    app_pc next_pc = decode(drcontext, pc, instr);

    /* Populate the buffer for the next cycle */
    data->pc = pc;
    data->mnemonic = decode_opcode_name(instr_get_opcode(instr));
    data->is_branch = instr_is_cbr(instr) || instr_is_ubr(instr);
    data->is_cbr = instr_is_cbr(instr);
    data->fallthrough_pc = next_pc;

    data->reg_src_count = 0;
    data->src_regs[0] = '\0';
    data->reg_dst_count = 0;
    data->dst_regs[0] = '\0';
    data->mem_load_count = 0;
    data->mem_loads[0] = '\0';
    data->mem_store_count = 0;
    data->mem_stores[0] = '\0';

    /* Analyze Sources (Registers & Memory) */
    for (int i = 0; i < instr_num_srcs(instr); i++) {
        opnd_t op = instr_get_src(instr, i);
        if (opnd_is_reg(op)) {
            data->reg_src_count++;
            APPEND_STR(data->src_regs, "%s ", get_register_name(opnd_get_reg(op)));
        }
        if (opnd_is_memory_reference(op)) {
            data->mem_load_count++;
            app_pc ea = opnd_compute_address(op, &mc);
            APPEND_STR(data->mem_loads, "0x%zx ", (size_t)ea);
        }
    }

    /* Analyze Destinations (Registers & Memory) */
    for (int i = 0; i < instr_num_dsts(instr); i++) {
        opnd_t op = instr_get_dst(instr, i);
        if (opnd_is_reg(op)) {
            data->reg_dst_count++;
            APPEND_STR(data->dst_regs, "%s ", get_register_name(opnd_get_reg(op)));
        }
        if (opnd_is_memory_reference(op)) {
            data->mem_store_count++;
            app_pc ea = opnd_compute_address(op, &mc);
            APPEND_STR(data->mem_stores, "0x%zx ", (size_t)ea);
        }
    }

    data->has_pending = true;
    instr_destroy(drcontext, instr);
}

/* Instrumentation event: inserts our clean call before every app instruction */
static dr_emit_flags_t event_app_instruction(void *drcontext, void *tag, instrlist_t *bb,
                                             instr_t *instr, bool for_trace,
                                             bool translating, void *user_data) 
{
    /* Ignore meta-instructions added by DynamoRIO itself */
    if (!instr_is_app(instr))
        return DR_EMIT_DEFAULT;

    app_pc pc = instr_get_app_pc(instr);
    
    /* Insert a clean call passing the PC of the instruction */
    dr_insert_clean_call(drcontext, bb, instr,
                         (void *)analyze_inst, false, 1,
                         OPND_CREATE_INTPTR(pc));

    return DR_EMIT_DEFAULT;
}

static void event_thread_init(void *drcontext) {
    thread_data_t *data = (thread_data_t *)dr_thread_alloc(drcontext, sizeof(thread_data_t));
    memset(data, 0, sizeof(thread_data_t));
    drmgr_set_tls_field(drcontext, tls_idx, data);
}

static void event_thread_exit(void *drcontext) {
    thread_data_t *data = (thread_data_t *)drmgr_get_tls_field(drcontext, tls_idx);
    
    /* Flush the very last instruction of the thread */
    if (data->has_pending) {
        dr_printf("PC: %p | %s | Branch: %s | Taken: N/A (Exit) | RegSrcs: %d [%s] | RegDsts: %d [%s] | MemLds: %d [%s] | MemSts: %d [%s]\n",
                  data->pc, data->mnemonic, data->is_branch ? "Yes" : "No", 
                  data->reg_src_count, data->src_regs,
                  data->reg_dst_count, data->dst_regs,
                  data->mem_load_count, data->mem_loads,
                  data->mem_store_count, data->mem_stores);
    }
    
    dr_thread_free(drcontext, data, sizeof(thread_data_t));
}

DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[]) {
    drmgr_init();
    tls_idx = drmgr_register_tls_field();

    drmgr_register_thread_init_event(event_thread_init);
    drmgr_register_thread_exit_event(event_thread_exit);

    /* Register the basic block instrumentation event */
    drmgr_register_bb_instrumentation_event(NULL, event_app_instruction, NULL);
}

