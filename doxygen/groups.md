@defgroup api_layer API Layers
@brief Provides an overview of the APIs implemented by different components
in Kodo.

Kodo is built using a C++ design technique called Mixin-Layers.
Mixin-Layers is a flexible way of separating functionality into reusable
building blocks (called layers).

At each layer a separate API is specified and in order to customize or add
functionality to Kodo it is necessary to understand which API layer to look
at.

@note That is by no means required that a specific encoder and decoder utilize
or provide all API layers. The API layers main purpose is to facilitate re-use
between implementations of different encoders and decoders.

If you wish to get a better understanding of the layering technique used, you
may take a look at the \ref mixin_example.


Before showing the typical layered structure of an encoder and decoder
implemented in Kodo is ...

//
// FACTORY
//

@defgroup factory Factory
@ingroup api_layer
@brief The factories are responsible for construction and initialization of
encoders and decoders.

@defgroup factory_api API
@ingroup factory
@brief The API used by factory layers.


@defgroup factory_layers Layers
@ingroup factory
@brief Implementations of the factory API.


@defgroup math Finite Field Math
@ingroup api_layer
@brief The finite field math layers perform computations on symbols and symbol coefficients.


@defgroup math_api API
@ingroup math
@brief The API used by math layers.

The prototype of a layer is the following:

@prototype


@defgroup math_layers Layers
@ingroup math
@brief Implementations of the math API.


@defgroup symbol_id Symbol IDs
@ingroup api_layer
@brief The symbol id describes how an encoded symbol has been produced.

The symbol id uniquely describes the coding coefficients which are used
by the encoder to create an encoded symbol as linear combinations of
a set of original source symbols. By transmitting the symbol id to the
decoder it can re-create the coding coefficients used to generate the encoded
symbol and by solving a linear system re-produce the original source symbols.

@note In its simplest from the symbol id is the actual coding coefficients.
      This type of symbol id is produced by the
      \link kodo::random_uniform_symbol_id<SuperCoder>  class.
      However for efficiency reasons you may want to limit the amount of data
      transmitted between an encoder and decoder by transmitting a smaller id.
      One example of this is to use the \link seed_symbol_id which only transmits
      the seed for the pseudo-random number generator used to generate the coding
      coefficients.


@note In Network Coding applications the symbol id is often referred to as the
      encoding vector.


@defgroup symbol_id_api API
@ingroup symbol_id
@brief The API used by symbol id layers.


@defgroup symbol_id_layers Layers
@ingroup symbol_id
@brief Implementations of the symbol id API.



@defgroup coding_info Coding Info
@ingroup api_layer
@brief Information about encoders and decoders


@defgroup coding_info_api API
@ingroup coding_info
@brief The coding information API.


@defgroup storage Storage
@ingroup api_layer
@brief Handles storage of encoding and decoding symbols.


@defgroup storage_api API
@ingroup storage
@brief The API used by storage layers.

The prototype of a layer is the following:

@prototype


@defgroup storage_layers Layers
@ingroup storage
@brief Implementations of the math API.





