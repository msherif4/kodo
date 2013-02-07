
@defgroup fec FEC Functionality
These layers contain the functionality needed to compose specific FEC codes
e.g. Network Coding codes or Reed-Solomon codes.

@defgroup math Math Layers
Provides basic mathematical operations used in the coding operations.

@defgroup storage Storage Functionality
Handle storage of coding symbols and implement different memory management
strategies, depending on the needs of the applications.

@defgroup debug Debug Functionality
Implements various debugging functionalies. Typically useful when implementing
new codes or when monitoring the performance of the coding algorithms.

@defgroup utility Utility Functionality
Implementing various simple operations such as zero-initializing memory
where needed. In addition to the added flexibility the use of C++ templates
and parameterized inheritance allows the compiler to generate highly optimized
code, with no need for virtual function dispatching etc.

@defgroup api_layer API Layers
@brief Provides an overview of the APIs implemented by different components
in Kodo.

Kodo is largely built using a C++ design technique called Mixin-Layers.
Mixin-Layers are a fexible way of seperating functionality into reusable
building blocks (called layers).

At each layer a seperate API is specified and in order to customize or add
functionality to Kodo it is necessary to understand which API layer to look
at.

@note That is by no means required that a specific encoder and decoder utilize
or provide all API layers. The API layers main purpose is to facilitat re-use
between implementations of different encoders and decoders.

If you wish to get a better understanding the the layering techinique used, you
may take a look at the \ref mixin_example.


Before showing the typical layered structure of a encoder and decoder
implemented in Kodo is ...


@defgroup factory_layer_api Factory API
@ingroup api_layer
@brief The factory API is responsible for construction and initialization of
encoders and decoders.


