@defgroup payload_layer_api Payload Layer API
@ingroup api_layer
@brief The payload layer provides users of an encoder or decoder with a
convenient API.

Creating a layer implementing the payload API makes it possible extend
encoders and decoders with functionality not neccesarily requiring direct
access to the symbol id and coded symbol.

Some examples of this can be seen in the ...

The payload layer also provides a user of an encoder/decoder with a more
concenient API than the \ref coding_layer_api, which requires the user to
provided seperate buffers for the symbol id and symbol data. Using the
payload layer API the user only provides a single memory buffer, as shown
in the following example for an encoder:

\code
std::vector<uint8_t> payload(encoder->payload_size());
uint32_t bytes_used = encoder->encode(&payload[0]);
\endcode

## Splitting the payload buffer
In order to use the payload layer one must also provide the functionality to
split the provided payload buffer into a symbol id and symbol buffer, which are
used by the \ref coding_layer_api.

This functionality has been implemented in the two classes:
- \link kodo::payload_encoder<SuperCoder>
- \link kodo::payload_decoder<SuperCoder>

## The payload layer API

@code uint32_t layer::encode(uint8_t *payload) @endcode

\param payload the buffer where the encoded symbol should be stored.
\return number of bytes used from the payload buffer

\see coding_layer_api

@code uint32_t layer::payload_size() const @endcode

The payload_size() function returns the maxmimum size in bytes that an encoded
payload can require.

\return the size required for the encoding payload.

@code uint32_t layer::factory::max_payload_size() const @endcode

The `max_payload_size()` is a factory function which is implemented in the
factory layers (\ref factory_layer_api). The function returns the maxmimum
size in bytes that an encoded payload can require.

\return the maximum size required for the encoding payload.

\see factory_layer_api

