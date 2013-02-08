// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>

#include <gtest/gtest.h>

struct null_flag
{
    struct flags
    {
        enum bits_needed { bit_size = 0 };
    };
};


template<uint32_t size, class SuperCoder>
struct flag_list
{

    typedef typename SuperCoder::flags flags;

    enum bits_needed { bit_size = size + flags::bit_size };
    enum bits_offset { bit_offset = flags::bit_size };


    static uint32_t offset()
        {
            return SuperCoder::k::flag_size - (bit_offset + size);
        }

    // static set(uint8_t *dest, uint32_t value)
    //     {
    //         static const uint32_t mask = (~0U) >> (32 - size);
    //         uint32_t array_index = bit_offset / std::numeric_limits<value_type>::digits;
    //         uint32_t offset = bit_offset % std::numeric_limits<value_type>::digits;
    //         static

};


struct flags_symbol_id
{
    uint32_t flags_offset()
        {
            return 0;
        }

    uint32_t symbol_id_size()
        {
            return 1;
        }
};


template<class SuperCoder>
struct sys : public SuperCoder
{
    typedef flag_list<4, SuperCoder> flags;



    void print(std::ostream &o)
        {
            o << "sys " << flags::offset()  << std::endl;
            SuperCoder::print(o);
        }
};

template<class SuperCoder>
struct systematic : public SuperCoder
{
    typedef flag_list<1, SuperCoder> flags;

    void print(std::ostream &o)
        {

            o << "systematic " << flags::offset() << std::endl;

            SuperCoder::print(o);

            std::cout << "Flag size in ssy" << SuperCoder::k::flag_size << std::endl;

        }
};

template<class SuperCoder>
struct sync_seed : public SuperCoder
{
    typedef flag_list<2, SuperCoder> flags;

    void print(std::ostream &o)
        {
            o << "sysc " << SuperCoder::k::flag_size - (flags::bit_offset + 2) << std::endl;


            // o << "Super final flag " << SuperCoder::final_flag_size << std::endl;

            std::cout << "Super final flag" << SuperCoder::k::flag_size << std::endl;
        }

};

template<class FINAL>
struct flag_coder
{
    typedef flag_list<0, null_flag> flags;

    struct k
    {
    enum t { flag_size = FINAL::flags::bit_size };
    };



    uint32_t flag_size()
        {
            return m_flag_size;
        }

    uint32_t flag_pos(uint32_t offset, uint32_t size)
        {
            return m_flag_size - (offset + size);
        }

    uint32_t m_flag_size;

};


struct test : public sys<systematic<sys<sync_seed<flag_coder<test> > > > >{};



TEST(TestXYZSymbolId, test)
{

    typedef flag_list<2, null_flag> flags;
    flags f;

    test t;

    t.print(std::cout);

    //std::cout << "Flag size " << t.flag_size() << std::endl;

}







/// Symbol ID iterable range represent the position of the non-zero coding
/// coefficients in a Symbol ID. The ranges behaves much like the familiar
/// iterator concepts extensively used in C++ STL and provides the following
/// API:
///
/// Moves to the next non-zero index
/// void advance()
///
/// Returns the index of the current non-zero coefficient
/// uint32_t current()
///
/// Returns true if no more non-zero coefficients exits in the symbol ID
/// bool at_end()





/// CoefficientWriter conept API:
///
/// You can use the coefficient API to set the individual coefficients
/// individually.
/// void set_coefficient(pos, value)
/// uint32_t length()
///
/// Or you can access the raw cofficients and update the entire buffer
/// in one go.
/// uint8_t* data()
/// uint32_t size()

#include <fifi/fifi_utils.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>


namespace kodo
{
    /// The Symbol ID Generator Concept API:

    /// Non positioned generator:
    /// generate(symbol_index, symbol_id)
    /// Positioned generator:
    /// generate(symbol_index, symbol_id)
    /// A symbol ID generator must _always_ return the same symbol id
    /// for the same symbol_index

    /// The linear symbol id layer contains the method needed to
    /// access and manipulate the symbol id coefficients. It expects
    ///
    template<class SuperCoder>
    class linear_symbol_id : public SuperCoder
    {
    public:

        /// The field type used
        typedef typename SuperCoder::field_type field_type;

        /// The value type used
        typedef typename field_type::value_type value_type;

    public:

        /// @copydoc final_coder_factory::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);

                m_id_size = fifi::bytes_needed<field_type>(symbols);
                assert(m_id_size > 0);
            }


        value_type coefficient(uint32_t index, const uint8_t *symbol_id)
            {
                return fifi::get_value<field_type>(symbol_id, index);
            }

        void set_coefficient(uint32_t index, uint8_t *symbol_id)
            {

            }




    protected:

        /// The number of bytes needed to store the symbol id
        /// coding coefficients
        uint32_t m_id_size;

    };


    /// @brief
    /// @ingroup symbol_id_generator
    /// @ingroup encoder_api
    template<class SuperCoder>
    class seed_encoder : public SuperCoder
    {
    public:

        /// @copydoc final_coder_factory::initialize()
        void initialize(uint32_t symbols, uint32_t symbol_size)
            {
                SuperCoder::initialize(symbols, symbol_size);
                m_symbol_index = 0;
            }

        ///
        void encode(uint8_t *symbol_data, uint8_t *symbol_id)
            {
                SuperCoder::seed(m_symbol_index);
                SuperCoder::encode(symbol_data, symbol_id);
                ++m_symbol_index;
            }


    protected:

        uint32_t m_symbol_index;
    };

    template<class SuperCoder>
    class cached_nonindexed_generator : public SuperCoder
    {
    };

    template<class SuperCoder>
    class cached_indexed_generator : public SuperCoder
    {
    };


}


// class rle_symbol_id_encoder
// {
// public:

//     void encode(uint8_t *symbol_id, uint8_t *symbol)
//         {
//             SuperCoder::generate(m_count, &m_symbol_id[0]);

//             bool rle_ok = rle_compress(symbol_id, m_symbol_id);

//             /// Put flag

//             SuperCoder::encode(&m_symbol_id[0], symbol);
//             }

//         }


// protected:

//     bool rle_compress(uint8_t *symbol_id, uint8_t* full_symbol_id)
//         {


//         }


// private:

//     /// The symbol id buffer
//     std::vector<uint8_t> m_symbol_id;

// };

template<class Field>
class null_generator
{
public:

    typedef Field field_type;
    typedef typename field_type::value_type value_type;

};


TEST(TestXYZSymbolId, test2)
{
    // typedef kodo::random_uniform_symbol_id<null_generator<fifi::binary> > test;

    // test t;




}
