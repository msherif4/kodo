@defgroup symbol_id_layer_api Symbol ID API
@ingroup api_layer
@brief The symbol id describes how an encoded symbol has been produced. The
decoder uses this information during decoding. The symbol id API allows
different representations of the symbol id to be implemented.

## The symbol id layer API

@code uint32_t layer::write_id(uint8_t *symbol_id, uint8_t **symbol_id_coefficients) @endcode

The `write_id` function is responsible for writing a symbol id to the
symbol_id buffer and initializing the symbol_id_coefficients pointer
with coding coefficients correspoinding to the symbol id.

@param symbol_id The buffer where the symbol id is written. This id should
       uniquely describe the coding coefficients used to generate an encoded
       symbol. It is therefore sufficient to transmit only the symbol id and
       not necessarily all coding coefficients to decoders.
@param symbol_id_coefficient Pointer to a pointer of symbol id coefficients.
       After the call to write_id the symbol_id_coefficient pointer will point
       to memory storing all coding coefficients correspoinding to the symbol id
       written to the symbol_id buffer.
@return The number of bytes used from the symbol_id buffer.


@code void layer::read_id(uint8_t *symbol_id, uint8_t **symbol_id_coefficients) @endcode

@param symbol_id Contains the symbol id for an encoded symbol.
@param symbol_id_coefficient Pointer to a pointer of symbol id coefficients.
       After the call to read_id the symbol_id_coefficient pointer will point
       to memory storing all coding coefficients correspoinding to the symbol id
       read from the symbol_id buffer.

@code uint32_t layer::symbol_id_size() const @endcode
\return the size in bytes required for the symbol id.

@code uint32_t layer::factory::max_symbol_id_size() const @endcode

The `max_payload_size()` is a factory function which is implemented in the
factory layers (\ref factory_layer_api).

\return The maximum size in bytes required for a symbol id.

\see factory_layer_api


