.. _Configuration_API:

==============================================
Legacy Configuration API (Compile-Time)
==============================================

.. warning::

   This page documents the Python configuration API used by the **compile-time** (CT) version
   of ChampSim. In the current **runtime** (RT) version, configuration is handled entirely
   via JSON files loaded at runtime (see :ref:`Creating_Config` and :ref:`Explicit_Config`).

   The Python API below (``config.parse``, ``config.filewrite``, ``config.util``) is only
   relevant if you are working with a CT build that uses the ``config.sh`` / ``config.py``
   workflow. The API reference is not generated for RT builds.

------------------------
Parsing API
------------------------

``config.parse.parse_config``
    Reads a JSON configuration file and produces a normalized system description.

------------------------
File Generation API
------------------------

The file generation API contains two interfaces: a high-level interface with ``config.filewrite.FileWriter``, and a low-level interface with ``config.filewrite.Fragment``.
Users should prefer the high-level interface where possible.

``config.filewrite.FileWriter``
    High-level interface for generating ChampSim source files from a parsed configuration.

``config.filewrite.Fragment``
    Low-level interface for generating individual code fragments.

--------------------------
Utility Functions
--------------------------

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
System operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ChampSim's configuration makes frequent use of sequences of dictionaries. The following functions operate on a system, a dictionary whose values are dictionaries.

* ``config.util.iter_system`` — Iterate over all elements of a system description.
* ``config.util.combine_named`` — Combine named elements from multiple sources.
* ``config.util.upper_levels_for`` — Find the upper-level caches for a given cache.
* ``config.util.propogate_down`` — Propagate values from upper to lower levels.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Itertools extensions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* ``config.util.collect`` — Collect elements into groups.
* ``config.util.batch`` — Batch elements into fixed-size groups.
* ``config.util.sliding`` — Sliding window over an iterable.
* ``config.util.cut`` — Split an iterable at a given index.
* ``config.util.do_for_first`` — Apply a function to only the first element.
* ``config.util.append_except_last`` — Append a separator except after the last element.
* ``config.util.multiline`` — Join lines with newline separators.
* ``config.util.yield_from_star`` — Flatten nested iterables.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Dictionary Operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* ``config.util.chain`` — Chain multiple dictionaries together.
* ``config.util.subdict`` — Extract a subset of keys from a dictionary.
* ``config.util.extend_each`` — Extend each value in a dictionary.
* ``config.util.explode`` — Expand a dictionary into individual key-value pairs.
* ``config.parse.duplicate_to_length`` — Duplicate elements to reach a target length.
* ``config.parse.extract_element`` — Extract an element from a nested structure.
