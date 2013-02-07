@defgroup coding_layer_api Coding Layer API
@ingroup api_layer
@brief The coding layer API implements encoding and decoding algorithms.

\tableofcontents

A large part of the funcitonality of the encoding algorithms are implemented by
implementation of this API.

### encode

\code
uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
\endcode

\param symbol_data okok
\param symbol_id test
\return the bla bla

\see fec

# symbol_id_size

If the layer manipulates the symbol id e.g. adding additional information
the layer must also implement the symbol_id_size() function to account
for the space needed.

\code
uint32_t symbol_id_size() const
\endcode

\return the size required for the Symbol ID in bytes

The correct implementation of this function must take into account the
space requirements of the underlying eslayers. E.g. reserving 2 bytes for
a specific layer could be implemented as follows:

\code
template<class SuperCoder>
class my_custom_layer : public SuperCoder
{
public:

    // ... some implementation

    // @return the number of bytes used for the symbol id
    uint32_t symbol_id_size() const
    {
        return 2 + SuperCoder::symbol_id_size();
    }

};
\endcode


