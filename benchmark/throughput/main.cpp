// Copyright Steinwurf APS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>


//#include <kodo/basic_rlnc_codes.h>
//#include <kodo/experiment_rlnc_codes.h>
#include <kodo/rlnc/full_vector_codes.h>

#include <speedy/time_benchmark.h>
#include <speedy/result_writer.h>



class Throughput
{
public:

    Throughput(const char* filename)
	: m_results(filename),
	  m_run_time(2)
	{
	    srand(time(0));
	}
    
    template<class Encoder, class Decoder>
    void do_run(const std::string &name)
	{
            typedef typename Encoder::pointer encoder_ptr;
            typedef typename Encoder::factory encoder_factory_type;
            
            typedef typename Decoder::pointer decoder_ptr;
            typedef typename Decoder::factory decoder_factory_type;
            
	    std::cout << "running " << name << std::endl;
	    
	    std::string encoder_name;
	    std::string decoder_name;

	    encoder_name.append(name);
	    encoder_name.append(" encoder");

	    decoder_name.append(name);
	    decoder_name.append(" decoder");

	    // The size of a block in symbols
	    uint32_t symbols = 16;

	    // Number of extra symbols to encode from each block
	    uint32_t extra_symbols = symbols * 2;

	    // The total block length
	    uint32_t total_symbols = symbols + extra_symbols;

	    // The size of symbol in bytes
	    uint32_t symbol_size  = 1600;

	    // Number of blocks to encode, we will increase this number
	    // if the test runs too fast
	    uint32_t number_of_blocks = 1024;
	    
	    // BENCHMARK ENCODER START --------------
	    std::cout << "starting encoder" << std::endl;

	    double encoder_time_elapsed = 0.0;
	    bool encoder_done = false;

	    // List of encoders 
	    std::vector<encoder_ptr> encoders;
            encoder_factory_type encoder_factory(symbols, symbol_size); 

            uint32_t max_payload_size = encoder_factory.max_payload_size();
            
            std::vector< std::vector< uint8_t > > block_storage;
            std::vector< std::vector< std::vector< uint8_t > > > payload_storage;

            std::vector<uint8_t> payload;
            payload.resize(2000);
            
	    while( !encoder_done )
	    {
                encoders.resize( number_of_blocks );
                payload_storage.resize( number_of_blocks );
                block_storage.resize( number_of_blocks );

	        // Add a number of encoders
                for(uint32_t i = 0; i < number_of_blocks; ++i)
                {
                    encoder_ptr encoder = encoder_factory.build(symbols, symbol_size);

                    encoder->systematic_off();

                    block_storage[i].resize(encoder->block_size());

                    kodo::set_symbols(kodo::storage(block_storage[i]), encoder);

                    payload_storage[i].resize(total_symbols);
                    
                    for(uint32_t j = 0; j < total_symbols; ++j)
                    {
                        payload_storage[i][j].resize(max_payload_size);
                    }
                    
                    encoders[i] = encoder;
                }
                
                PXTimer encoder_timer;
                encoder_timer.reset();
            
                // Clock running
            
                for(uint32_t i = 0; i < encoders.size(); ++i)
                {
                    encoder_ptr &encoder = encoders[i];
                    //std::vector<std::vector<uint8_t> > &s = payload_storage[i];
                    
                    for(uint32_t j = 0; j < total_symbols; ++j)
                    {
                        // Encode a packet
                        //encoder->encode( &s[j][0] );
                        encoder->encode( &payload[0] );
                    }
                }
                
                encoder_time_elapsed = encoder_timer.get();
                
                    // Clock stopped
                
                if(encoder_time_elapsed > m_run_time)
                {
                    encoder_done = true;
                }
                else
                {
                    uint32_t blocks_encoded = encoders.size();
                    double factor = (m_run_time / encoder_time_elapsed);

                    number_of_blocks = (factor * blocks_encoded) + 1;
                    
                    std::cout << "Benchmark did not run long enough with "
                              << blocks_encoded
                              << " blocks additional "
                              << (number_of_blocks - encoders.size())
                              << " blocks needed" << std::endl;

                    std::cout << "Encoders was " << encoders.size()
                              << " now will be " << number_of_blocks << std::endl;
                    std::cout << "Time was " << encoder_time_elapsed << std::endl;
                    
                }
            }
                
                
                //amount of data processed
	    double encode_megs = (encoders.size()*(total_symbols)*symbol_size)/1000000.0;
	    double encode_megs_per_second = encode_megs / encoder_time_elapsed;
	    
	    // m_results.write(encoder_name, encode_megs_per_second);

	    std::cout << "encode_megs = " << encode_megs << " encode_megs_per_second = "
                      << encode_megs_per_second << " time = "
                      << encoder_time_elapsed << std::endl;
	  

		
            // // BENCHMARK DECODER START --------------
	    // std::cout << "starting decoder" << std::endl;

	    // double decoder_time_elapsed = 0.0;
		
	    // // List of encoders 
	    // std::vector<decoder_ptr> decoders;
            // decoder_factory_type decoder_factory(block_length, symbol_size); 
	    
	    // for(uint32_t i = 0; i < encoders.size(); ++i)
	    // {
            //     decoder_ptr decoder = decoder_factory.build(
            //         block_length, symbol_size, bytes_used);

            //     decoders.push_back(decoder);

	    // }
	    
	    
	    // PXTimer decoder_timer;
	    // decoder_timer.reset();
	    
	    // uint32_t symbols_used = 0;
	    
	    // // Clock running
	    
	    // for(uint32_t i = 0; i < decoders.size(); ++i)
	    // {
		
	    //     decoder_ptr &d = decoders[i];

            //     std::vector<uint8_t> &s = payload_storage[i];
            //     std::vector<uint8_t> &v = payload_id_storage[i];

            //     uint32_t payload_size = d->payload_size();
            //     uint32_t payload_id_size = d->payload_id_size();

                
	    //     for(uint32_t j = 0; j < total_block_length; ++j)
	    //     {
	    //         // Decode a packet
	    //         d->decode( &s[0] + (payload_size * j),
            //                    &v[0] + (payload_id_size * j));

	    //         ++symbols_used;
		    
	    //         if(d->is_complete())
	    //     	break;
	    //     }
	    // }
	    
	    // decoder_time_elapsed = decoder_timer.get();
	    
	    
	    // // Clock stopped
	    
	    // //amount of data processed
	    // double decode_megs = (symbols_used*symbol_size)/1000000.0;
	    // double decode_megs_per_second = decode_megs / decoder_time_elapsed;
	    
	    // // m_results.write(decoder_name, decode_megs_per_second);

	    // std::cout << "decode_megs = " << decode_megs << " decode_megs_per_second = " << decode_megs_per_second << " time = " << decoder_time_elapsed << std::endl;

	    // for(uint32_t i = 0; i < decoders.size(); ++i)
	    // {
	    //     decoder_ptr &d = decoders[i];
	    //     if(!d->is_complete())
	    //     {
	    //         std::cout << "Decoder " << i << " failed to decode, rank " << d->rank() << std::endl;
	    //     }
	    // }

	    // // Check the encoded and decoded storage
	    // if(encoders.size() != decoders.size())
	    // {
	    //     std::cout << "FAILURE: Encoder decoder size mismatch" << std::endl;
	    //     return;
	    // }
	    
	    // for(uint32_t i = 0; i < encoders.size(); ++i)
	    // {
	    //     encoder_ptr &e = encoders[i];
	    //     decoder_ptr &d = decoders[i];
	    //     bool storage_equal = memcmp(e->data(), d->data(), e->data_size()) == 0;

	    //     if(storage_equal != true)
	    //     {
	    //         std::cout << "STORAGE COMPARE FAILURE for block " << i << std::endl;
	    //         break;
	    //     }
	    // }
	    
	}
    
private:
    
    speedy::result_writer m_results;
    
    /**
     * The minimum time to benchmark
     */
    uint32_t m_run_time;
    
};

int main()
{
    {
        Throughput test("results.xml");
        test.do_run
            <
            kodo::full_rlnc_encoder<fifi::binary>,
            kodo::full_rlnc2_decoder
            >("rlnc2");

        //test.do_run<kodo::full_rlnc8_encoder, kodo::full_rlnc8_decoder>("rlnc8");
        //      test.do_run<kodo::rlnc16>("rlnc16");
        //test.do_run<kodo::exp_rlnc2>("exp_rlnc2");
        //test.do_run<kodo::rlnc8>("exp_rlnc8");
        //test.do_run<kodo::rlnc16>("exp_rlnc16");
        //test.do_run<kodo::rlnc2_no_cache>("rlnc2_no_cache");
        //test.do_run<kodo::rlnc8_no_cache>("rlnc8_no_cache");
        //test.do_run<kodo::rlnc16_no_cache>("rlnc16_no_cache");

        //test.do_run<kodo::full_rlnc16>("full_rlnc16");        
    }
    
    return 0;
}
