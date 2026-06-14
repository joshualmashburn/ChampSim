.. _Wrong_path_support:

=====================================
Wrong-Path Support
=====================================

ChampSim supports traces that contain instructions executed due to branch mispredictions (wrong-path instructions).
This feature enables more realistic simulation of speculative execution effects on the cache hierarchy and other microarchitectural structures.

----------------------------------
Overview
----------------------------------

When a branch is mispredicted, a modern out-of-order processor will speculatively execute instructions along the wrong path until the misprediction is detected.
These wrong-path instructions can affect cache contents, prefetcher behavior, and other microarchitectural state.
ChampSim's wrong-path support allows traces to include these instructions, which the simulator can either skip or execute through the pipeline.

----------------------------------
Trace Format
----------------------------------

Wrong-path traces use an extended trace format with additional fields in each instruction:

* ``branch_target``: The branch target address recorded in the trace.
* ``flags``: A bitfield encoding instruction properties. Bit 7 (``SQUASHED``) indicates the instruction was on the wrong path.
* ``pref``: Indicates whether the instruction is a prefetch instruction (treated as wrong-path).

The trace format for ``input_instr`` is::

    struct input_instr {
      unsigned long long ip;
      unsigned char is_branch;
      unsigned char branch_taken;
      unsigned long long branch_target;        // NEW
      unsigned char destination_registers[2];
      unsigned char source_registers[4];
      unsigned long long destination_memory[2];
      unsigned long long source_memory[4];
      unsigned char flags;                     // NEW
      unsigned char pref;                      // NEW
    };

.. note::
   Traces must be generated with a wrong-path-aware tracer (e.g., the extended Pin tracer) to include the ``flags``, ``pref``, and ``branch_target`` fields.
   Standard ChampSim traces without these fields are **not** compatible with this format.

----------------------------------
Usage
----------------------------------

Wrong-path support is controlled via command-line flags:

``--wrong-path``
  Enables wrong-path execution. When set, wrong-path instructions from the trace are fed into the pipeline,
  consuming resources (ROB entries, cache bandwidth, etc.) and generating cache accesses.
  When the misprediction is resolved, the pipeline is flushed and wrong-path instructions are squashed.

``--wpa``
  Enables Wrong-Path-Aware (WPA) mode. This is an additional mode on top of ``--wrong-path`` that
  allows the simulator to approximate wrong-path instruction behavior using a map of previously seen instructions.

  Reference: Ros, A., & Jimborean, A. (2023). Wrong-Path-Aware Entangling Instruction Prefetcher. IEEE Transactions on Computers.

**Examples:**

Run with wrong-path execution enabled::

    ./bin/champsim --wrong-path -w 1000000 -i 10000000 trace.champsimtrace.xz

Run with both wrong-path and WPA modes::

    ./bin/champsim --wrong-path --wpa -w 1000000 -i 10000000 trace.champsimtrace.xz

----------------------------------
Behavior
----------------------------------

Without ``--wrong-path`` flag:
  Wrong-path instructions in the trace are skipped during instruction fetch.
  The simulator counts them in statistics but they do not enter the pipeline.
  The branch misprediction penalty is applied based on the trace annotations.

With ``--wrong-path`` flag:
  Wrong-path instructions enter the pipeline and consume resources just like correct-path instructions.
  When the mispredicted branch resolves at execution:

  1. All wrong-path instructions in the ROB are squashed.
  2. Wrong-path entries in the load and store queues are removed.
  3. The IFETCH, DECODE, and DISPATCH buffers are cleared.
  4. The pipeline re-steers to the correct path after a misprediction penalty.

----------------------------------
Statistics
----------------------------------

Wrong-path statistics are reported in the simulation output when wrong-path instructions are present:

* **Wrong Path Instructions**: Total number of wrong-path instructions encountered.
* **Skipped**: Number of wrong-path instructions that were skipped (not executed through the pipeline).
* **Loads**: Number of wrong-path load instructions.

These statistics appear in both plain text and JSON output formats.

----------------------------------
Generating Wrong-Path Traces
----------------------------------

To generate traces with wrong-path information, use a Pin-based tracer that captures the ``SQUASHED`` flag for instructions on mispredicted paths.
The tracer should set bit 7 of the ``flags`` field for wrong-path instructions and populate the ``branch_target`` field.
