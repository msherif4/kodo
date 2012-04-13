Network Coding Glossary
-----------------------

Here follows a list of terms that is often used in connection with Network Coding. These terms are used in Kodo and it is assumed in both the documentation and the sources that the reader is familiar with these terms.


Finite Field
............

A finite field is a mathmatical construct and entails to much explenations to be included here. It is not necessary to have a deep understanding of finite fields, however, you must be familiar with a couple of related terms. Finite fields can be used to implement linear codes on computers with fixed precision, which is what they are used for in Kodo. Several finite fields are implemented in Fifi, which you can find alongside Kodo on GitHub.

Finite Field Element
 Is an element in the finite field over which coding operations are performed.

Field Size / Order
 A finite field element can take a number of values which is specified by the field size. Typically these values are the range from 0 to the size minus 1, which is also the case througout Kodo.

.. Extension field

.. Field Degree


Network
.......

Node
 An unit on the network.

Source
 A node that transmits data to other node(s).
 
Sink
 A node that receives data from other node(s).

Relay
 A node that receives data from other node(s) and transmit that data to other node(s), typically the relay is not itself interested in receiving the data.


Coding
......

Original data
 Some data of size *B* that is to be transferred from a source to one or more sinks.

Symbol
 Each symbol *Si*, is one finite field element or a concatenation of finite field elements. Each symbol represent a part of the original data.

Symbol Size
 The size of a symbol, *m*, is defined by the number of elements it contains and those field elements size.

Generation
 The original data is divided into generations, of size :math:`g \cdot m`. Each generation constitutes *g* symbols, each of size *m*. The data from each generation is encoded, decode and recoded seperately.

 ====  ====  =====  ====
 Generation
 -----------------------
 *S1*  *S2*   ...   *Sg* 
 ====  ====  =====  ====

 A generation is sometimes also referred to as a *source block* or a *batch*.

Generation Size
 Is the number of symbols in each generation, and denoted *g*.

Data Matrix
 The data in a generation can be arranged as a matrix, where each column correponds to a symbol.

Coding Vector
 A coding vector is a vector of *g* field elements, where each element defines the coding operations performed on the corresponding element in the generation.

Density or Degree
 The density of a coding vector is the ratio of non-zero elements in the vector

 :math:`d(h) = \frac{\sum_{k=1}^g h_k \neq 0}{g}`.

 where:

  :math:`h_i` is the vector

  :math:`g` is the generation size


Encoding
 Is performed at source(s) where the original data to be send is encoded and packetized and transmitted via the network.

Decoding
 Is performed at sink(s) where the coded data is decode in order to recreate the original data from the source(s).

Recoding
 Can be performed in the network at relay node(s) between the source and the sink. Such a node can combine received coded packets in order to create new coded packets. A sink that has not fully decoded the data can also recode.

Linear
 All codes that Kodo support are linear and coding operations are performed over some finite field. An important feature of a linear code is that a combination of any valid codewords is also a valid codeword.

Linear (In)dependent
 A symbol that is not a linear combination of a set of received symbols at a node, is said to be linear independent.

Coded Packet
 Contains a vector-data pair.

 ===============  ===============
              Packet            
 --------------------------------
   Vector          Coded Symbol 
 ===============  ===============
   
Coded Packet Size
 Is the total size of the coding vector and coded symbol.

Bulk Data
 When bulk data is coded the file is split into one or more parts of a resonable size, each of these parts is denoted a generation.

Streaming Data
 When streaming data is coded some amount of data is accumulated and this data is denoted a generation.


