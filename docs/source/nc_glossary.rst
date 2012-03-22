Network Coding Glossary
=======================

Here follows a list of terms that is often used in connection with Network Coding. These terms are used in Kodo and it is assumed in both the documentation and the sources that the reader is familiar with these terms.

Coding
------

**Encoding**
 Is performed at source(s) where the data to be send is combined in a way that is beneficial for transmission over a packet network.

**Decoding**
 Is performed at sink(s) where the coded data is decode in order to recreate the original data from the source(s).

**Recoding**
 Can be performed in the network at relay node(s) between the source and the sink. A sink that has not fully decoded the data can also recode.

**Generation**
 The data of size *B* that is to be transferred from a source to one or more sinks is divided into generations, of size *g\*m*. Each generation constitutes *g* symbols, each of size *m*. As the data within generations (generally) are independent, they are encoded, decode and recoded seperately.

 ====  ====  =====  ====
 Generation
 -----------------------
  S1    S2    ...    Sg 
 ====  ====  =====  ====

 A generation is sometimes also referred to as a *source block* or a *batch*.

**Generation Size**
 Is the number of symbols in each generation, and denoted *g*.

**Symbol** /Data
 Each symbol, with size *m*, is one element or a concatenation of elements within some specified finite field.

 A symbol is sometimes also simply referred to as *data*.

**Symbol Size**
 The size of a symbol is defined as the number of elements it contains.

**Element**
 Is defined by a finite field. E.g. the operations (+,-,*,%) that can be performed on the symbol, and the size of the element is defined by the field.


**Vector**
 A vector or coding vector is a vector of *g* field elements, where each element describes the coding operations performed on the corresponding element in the symbol. The number of elements in the vector is equal to the generation size *g*.

**Density**
 The density of a vector is the ratio of non-zero elements or the number of total elements 
 :math:`d(h) = \frac{\sum_{k=1}^g h_k \neq 0}{g}`.

 where:

  :math:`h_i` is the vector

  :math:`g` is the generation size


**Linear (In)dependent**


Networking
----------

**Packet**
 Contains a vector-data pair.

 +------------------------------+
 |            Packet            |
 +---------------+--------------+
 | Vector        | Coded Symbol |
 +---------------+--------------+
   
**Packet Size**
 Is equal to the size of the vector and symbol.

**Bulk Data**
 When bulk data is coded the file is split into one or more parts of a resonable size, each of these parts is denoted a generation.

**Streaming Data**
 When streaming data is coded some amount of data is accumulated and this data is denoted a generation.

Underlying Math
---------------

**Finite Field**
 A field is the mathematical construct over which coding operations are performed. Kodo supports several different fields and extension fields.

**Field Size** / Order

**Field Degree**


