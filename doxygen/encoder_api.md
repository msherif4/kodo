@defgroup encoder_api Encoder API
These layers implement the following API:
\code
// @param symbol_data the memory buffer where the encoded symbol will
//        be stored
// @param symbol_id the memory buffer where the symbol id will
//        be stored
// @return the number of bytes used for the symbol id
uint32_t encode(uint8_t *symbol_data, uint8_t *symbol_id)
\endcode

If the layer manipulates the symbol id e.g. adding additional information
the layer must also implement the symbol_id_size() function to account
for the space needed.

\code
// @return the number of bytes used for the symbol id
uint32_t symbol_id_size() const
\endcode

The correct implementation of this function must take into account the
space requirements of the underlying layers. E.g. reserving 2 bytes for
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


First Header  | Second Header
------------- | -------------
Content Cell  | Content Cell
Content Cell  | Content Cell

