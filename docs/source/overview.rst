


Layers
======

+--+-------------------------------+-----------------------------------+
| 6| Encoder interface             | Decoder interface                 |
+--+-------------------+-----------+-----------+-----------------------+
| 5| Linear encoder    | Generator             | Decoder               |
+--+-------------------+-----------+-----------+-----------------------+
| 4| Symbol and symbol id representation                               |
+--+-------------------------------------------------------------------+
| 3| Finite field math                                                 |
+--+-------------------------------------------------------------------+
| 2| Symbol and symbol id storage                                      |
+--+-------------------------------------------------------------------+
| 1| Utilities                                                         |
+--+-------------------------------------------------------------------+

6. Interface to the coders

5. Defines the code, e.g. systematic layer

4. representation of the coded symbols and their symbol id's, such as a matrix or a graph representation.

3. Finite field implementation

2. Memory management

1.




