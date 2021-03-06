Implementations
===============

Gate base
---------
A custom gate implementation **must** derive from the ``gate_base`` class.

.. doxygenclass:: tweedledum::gate_base

Custom gates
------------

All gate implementations are located in `tweedledum/gates/`:

+--------------------------------+-------------+----------------+
| Interface method               | io3_gate    | mcmt           |
+================================+=============+================+
|                                | *Constants*                  |
+--------------------------------+-------------+----------------+
| ``max_num_io``                 | 3           | 32             |
+--------------------------------+-------------+----------------+
| ``network_max_num_qubits``     |             | 32             |
+--------------------------------+-------------+----------------+
|                                | *Properties*                 |
+--------------------------------+-------------+----------------+
| ``num_controls``               | ✓           | ✓              |
+--------------------------------+-------------+----------------+
| ``num_targets``                | ✓           | ✓              |
+--------------------------------+-------------+----------------+
|                                | *Iterators*                  |
+--------------------------------+-------------+----------------+
| ``foreach_control``            | ✓           | ✓              |
+--------------------------------+-------------+----------------+
| ``foreach_target``             | ✓           | ✓              |
+--------------------------------+-------------+----------------+

Networks
--------

All network implementations are located in `tweedledum/networks/`:

+--------------------------------+------------------------------+------------------------------+
| Interface method               | netlist                      | gate graph (gg)              |
+================================+==============================+==============================+
|                                | *I/O and ancillae qubits*    | *I/O and ancillae qubits*    |
+--------------------------------+------------------------------+------------------------------+
| ``add_qubit``                  | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``add_cbit``                   | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``add_ancilla``                |                              |                              |
+--------------------------------+------------------------------+------------------------------+
|                                | *Properties*                 | *Properties*                 |
+--------------------------------+------------------------------+------------------------------+
| ``name``                       | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``gate_set``                   | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
|                                | *Structural properties*      | *Structural properties*      |
+--------------------------------+------------------------------+------------------------------+
| ``size``                       | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``num_qubits``                 | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``num_cbits``                  | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``num_gates``                  | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
|                                | *Iterators*                  | *Iterators*                  |
+--------------------------------+------------------------------+------------------------------+
| ``foreach_io``                 | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``foreach_qubit``              | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``foreach_cbit``               | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``foreach_input``              | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``foreach_output``             | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``foreach_gate``               | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
| ``foreach_node``               | ✓                            | ✓                            |
+--------------------------------+------------------------------+------------------------------+
|                                | *Node iterators*             | *Node iterators*             |
+--------------------------------+------------------------------+------------------------------+
| ``foreach_child``              |                              | ✓                            |
+--------------------------------+------------------------------+------------------------------+
