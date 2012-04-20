Kodo Organization
=================

This page describes how the Kodo library is organized and how the different
featur


Layers
======

..
   +--+-------------------------------+-----------------------------------+
   | 6| Encoder interface             | Decoder interface                 |
   +--+-------------------+-----------+-----------+-----------------------+
   | 5| Linear encoder    | Generator             | Decoder               |
   +--+-------------------+-----------+-----------+-----------------------+


+--+-------------------------------------------------------------------+
| 6| Coder interface                                                   |
+--+-------------------------------------------------------------------+
| 5| Code specialization and overlays                                  |
+--+-------------------------------------------------------------------+
| 4| Codes                                                             |
+--+-------------------------------------------------------------------+
| 3| Finite fields                                                     |
+--+-------------------------------------------------------------------+
| 2| Symbol and symbol id storage                                      |
+--+-------------------------------------------------------------------+
| 1| Utilities                                                         |
+--+-------------------------------------------------------------------+

Coder interface
 Provides a convinient interface to application developers. It allows for encoding over an object with a specified code and build and hold all needed encoders. Similar decoding and recoding over an object.

Code specialization
 Provides multiple specializations for the *basic* code. **Systematic** specialization where all data is first send uncoded. Cached specialiatation where coding vectors are reused between generations. **Random Annex** specilatation where a random annex partitioning scheme is overlayed on the code.


Code implementation
 Implement multiple codes, such as **Random Linear Netork Coding** (RLNC), Sparse RLNC (S-RLNC), and **Reed-Solomon** (RS). Each code uses a generator to generate symbol id's (called coding vectors, for NC codes), an encoder to encode data, a decoder to decode data, and if a NC code a recode that can recode data based on data in a decoder.


Finite fields
 Implement multiple Finite Fields, each field provide the basic operations (+ - % \*). Currently there is support for **binary extensition fields** (of the commonly used sizes 2, 2^8, 2^16, 2^32) and a couple of large **prime fields**. To ensure high performance on all platforms for all fields, most of the fields are implemented using multiple techniques.


Memory management
 Provides convinient and reusable **storage** for symbols and symbol id's.


Utilities
 Remove ?









