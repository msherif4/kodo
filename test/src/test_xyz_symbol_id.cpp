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



namespace kodo
{
    /// Generates full encoding vectors where every coding coefficient
    /// is chosen uniformly random.
    template<class SuperCoder>
    class random_uniform : public SuperCoder
    {
    public:

        /// Typedef for the value type
        typedef typename SuperCoder::value_type value_type;

    public:

        /// Fills the vector buffer with random bytes
        /// @param buffer pointer to the destination of the symbol
        /// @param length of the the symbol in elements
        void generate(uint32_t seed, value_type *symbol_id, uint32_t size)
            {
                m_random_generator.seed(seed);
                generate(symbol_id);
            }

        ///
        void generate(uint8_t *value_type)
            {
                value_type *id =
                    reinterpret_cast<value_type*>(symbol_id);

                assert(id != 0);

                for(uint32_t i = 0; i < m_symbol_id_length; ++i)
                {
                    id[i] = m_distribution(m_random_generator);
                }

            }

        void seed(uint32_t seed_value)
            {
                m_random_generator.seed(seed_value);
            }

    protected:

        /// The length of the symbol id
        uint32_t m_symbol_id_length;

        /// The distribution wrapping the random generator
        boost::random::uniform_int_distribution<value_type> m_distribution;

        /// The random generator
        boost::random::mt19937 m_random_generator;
    };
}


class rle_symbol_id_encoder
{
public:

    void encode(uint8_t *symbol_id, uint8_t *symbol)
        {
            SuperCoder::generate(m_count, &m_symbol_id[0]);

            bool rle_ok = rle_compress(symbol_id, m_symbol_id);

            /// Put flag

            SuperCoder::encode(&m_symbol_id[0], symbol);
            }

        }


protected:

    bool rle_compress(uint8_t *symbol_id, uint8_t* full_symbol_id)
        {


        }


private:

    /// The symbol id buffer
    std::vector<uint8_t> m_symbol_id;

};


TEST(TestXYZSymbolId, test2)
{
    typedef kodo::random_uniform<kodo::null_coder> test;

    test t;



}
