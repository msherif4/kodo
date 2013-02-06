@defgroup payload_layer_api Payload Layer API
@brief The payload layer prodides users of encoder or decoder with a
convenient API.

Creating a layer implementing the payload API makes it possible extend
encoders and decoders with functionality not neccesarily requiring direct
access to the symbol id and coded symbol.

Some examples of this can be seen in the ...

The payload layer also provides a user of an encoder/decoder with a more
concenient API than the @see encoder_api, which requires the user to
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
used byte the coding layers.

This functionality has been implemented in the payload_encoder and
payload_decoder classes.

# encode(uint8_t *payload)

\code
uint32_t encode(uint8_t *payload)
\endcode

\param payload the buffer where the encoded symbol should be stored.
\return number of bytes used from the payload buffer

\see encoder_api

# payload_size

The payload_size() function returns the maxmimum size in bytes that an encoded payload
can require.

\code
uint32_t payload_size() const
\endcode

\return the size required for the encoding payload.



