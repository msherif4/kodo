Network Coding Glossary
=======================

Here follows a list of terms that is often used in connection with Network Coding. These terms are used in Kodo and it is assumed in both the documentation and the sources that the reader is familiar with these terms.


Finite Field
------------

**Finite Field Element**
 An element in the finite field over which coding operations are performed. Kodo supports several different fields and extension fields.

**Field Size** / Order

**Field Degree**


Basic Coding
------------

**Linear**
All codes that Kodo support are linear and coding operations are performed over some finite field.

**Generation**
 The data of size *B* that is to be transferred from a source to one or more sinks is divided into generations, of size :math:`g \cdot m`. Each generation constitutes *g* symbols, each of size *m*. The data from each generation is encoded, decode and recoded seperately.

 ====  ====  =====  ====
 Generation
 -----------------------
  S1    S2    ...    Sg 
 ====  ====  =====  ====

 A generation is sometimes also referred to as a *source block* or a *batch*.

**Generation Size**
 Is the number of symbols in each generation, and denoted *g*.

**Symbol**
 Each symbol, is one finite field element or a concatenation of finite field elements. The symbol represent a part of the original data in the generation.

**Symbol Size**
 The size of a symbol, *m*, is defined as the number of elements it contains.

**Coding Vector**
 A coding vector is a vector of *g* field elements, where each element defines the coding operations performed on the corresponding element in the generation.

**Encoding**
 Is performed at source(s) where the original data to be send is encoded and packetized and transmitted via the network.

**Decoding**
 Is performed at sink(s) where the coded data is decode in order to recreate the original data from the source(s).

**Recoding**
 Can be performed in the network at relay node(s) between the source and the sink. Such a node can combine received coded packets in order to create new coded packets. A sink that has not fully decoded the data can also recode.


Other Coding
------------

**Density**
 The density of a vector is the ratio of non-zero elements
 :math:`d(h) = \frac{\sum_{k=1}^g h_k \neq 0}{g}`.

 where:

  :math:`h_i` is the vector

  :math:`g` is the generation size


**Linear (In)dependent**


Networking
----------

**Packet**
 Contains a vector-data pair.

 ===============  ===============
              Packet            
 --------------------------------
   Vector          Coded Symbol 
 ===============  ===============
   
**Packet Size**
 Is the total size of the coding vector and coded symbol.

**Bulk Data**
 When bulk data is coded the file is split into one or more parts of a resonable size, each of these parts is denoted a generation.

**Streaming Data**
 When streaming data is coded some amount of data is accumulated and this data is denoted a generation.




