// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_FILE_READER_HPP
#define KODO_FILE_READER_HPP

#include <fstream>
#include <cassert>

#include "has_deep_symbol_storage.hpp"

namespace kodo
{

    /// The file reader class reads data from a local file
    /// and initializes the an encoder with data from a specific
    /// offset within the file. This class can be used in
    /// conjunction with object encoders.
    ///
    /// Note that this type of data reader can only be used together
    /// with deep_symbol_storage encoders.
    template<class EncoderType>
    class file_reader
    {
    public:

        static_assert(has_deep_symbol_storage<EncoderType>::value,
                      "File reader only works with encoders using"
                       "deep storage");

    public:

        /// Pointer to the encoders
        typedef typename EncoderType::pointer pointer;

    public:

        /// Construct a new file reader
        /// @param filename of the file to use
        /// @param data_size the number of bytes needed by the temporary
        ///        memory buffer to be used with encoder->swap_symbols()
        file_reader(const std::string &filename,
                    uint32_t data_size)
            {
                m_file = boost::make_shared<std::ifstream>();
                m_file->open(filename, std::ios::binary);

                std::cout << "In file reader open " << filename << std::endl;
                
                assert(m_file->is_open());

                // Improve error handing by throwing an exception
                // if(!m_file.is_open())
                // {
                //     ec = error::make_error_code(error::failed_open_file);
                //     return;
                // }

                m_file->seekg(0, std::ios::end);
                auto position = m_file->tellg();
                assert(position >= 0);

                m_file_size = static_cast<uint32_t>(position);
                assert(m_file_size > 0);
                assert(data_size > 0);

                m_data.resize(data_size);
            }

        /// @return the size in bytes of the file
        uint32_t size() const
            {
                return m_file_size;
            }

        /// Initializes the encoder with data from the file.
        /// @param encoder to be initialized
        /// @param offset in bytes into the storage object
        /// @param size the number of bytes to use
        void read(pointer &encoder, uint32_t offset, uint32_t size)
            {
                assert(encoder);
                assert(offset < m_file_size);
                assert(size > 0);
                assert(m_file);
                assert(m_file->is_open());

                uint32_t data_size = m_data.size();
                assert(size <= data_size);

                uint32_t remaining_bytes = m_file_size - offset;
                assert(size <= remaining_bytes);

                m_file->seekg(offset, std::ios::beg);
                assert(m_file);

                m_file->read(reinterpret_cast<char*>(&m_data[0]), size);
                assert(size == static_cast<uint32_t>(m_file->gcount()));

                encoder->swap_symbols(m_data);

                // Check that the swapped vector has the same size
                assert(m_data.size() == data_size);

                // We require that encoders includes the has_bytes_used
                // layer to support partially filled encoders
                encoder->set_bytes_used(size);
            }

    private:

        /// The actual file
        boost::shared_ptr<std::ifstream> m_file;

        /// The size of the file in bytes
        uint32_t m_file_size;

        /// Intermediate buffer used for reading from the file and
        /// swapping into the encoders - avoid any additional copies of
        /// the data.
        std::vector<uint8_t> m_data;

    };

}

#endif

