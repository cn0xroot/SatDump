#pragma once

#define TEST_BITS_LENGTH 1024

#include "modules/common/sathelper/packetfixer.h"
#include "modules/viterbi/cc_decoder_impl.h"
#include "modules/viterbi/cc_encoder_impl.h"
#include "modules/viterbi/depuncture_bb_impl.h"
#include "modules/common/repack_bits_byte.h"

/*class SymbolReorg
{
private:
    // volk::vector<uint8_t> d_buffer_reorg;

public:
    int work(uint8_t *in, int size, uint8_t *out)
    {
        //d_buffer_reorg.insert(d_buffer_reorg.end(), &in[0], &in[size]);
        int size_out = 0;
        int to_process = size / 4;

        for (int i = 0; i < to_process; i++)
        {
            out[size_out++] = in[i * 4 + 0];
            out[size_out++] = in[i * 4 + 2];
            out[size_out++] = in[i * 4 + 1];
            out[size_out++] = in[i * 4 + 3];
        }

        //d_buffer_reorg.erase(d_buffer_reorg.begin(), d_buffer_reorg.begin() + to_process * 4);

        //   consume_each(std::lround((1.0 / relative_rate()) * noutput_items));
        return size_out;
    }
};*/

namespace metop
{
    class MetopViterbi2
    {
    private:
        // Settings
        const int d_buffer_size;
        const int d_outsync_after;
        const float d_ber_thresold;

        // Variables
        int d_outsinc;
        int d_state;
        bool d_first;
        float d_bers[2][4];
        float d_ber;

        // BER Decoders
       // gr::fec::depuncture_bb_impl depunc_ber;
        gr::fec::code::cc_decoder_impl cc_decoder_in_ber;
        gr::fec::code::cc_encoder_impl cc_encoder_in_ber;

        // BER Buffers
        uint8_t d_ber_test_buffer[TEST_BITS_LENGTH];
        uint8_t d_ber_input_buffer[TEST_BITS_LENGTH];
        uint8_t d_ber_input_reorg_buffer[TEST_BITS_LENGTH];
        uint8_t d_ber_input_buffer_depunc[TEST_BITS_LENGTH * 2]; // 1.5
        uint8_t d_ber_decoded_buffer[TEST_BITS_LENGTH * 2];
        uint8_t d_ber_encoded_buffer[TEST_BITS_LENGTH * 2]; // 1.5

        // Current phase status
        sathelper::PhaseShift d_phase_shift;
        bool d_iq_inv;
        sathelper::PacketFixer phaseShifter;

        // Work buffers
        uint8_t *fixed_soft_packet;
        uint8_t *converted_buffer;
        uint8_t *reorg_buffer;
        uint8_t *depunc_buffer;
        uint8_t *output_buffer;

        // Main decoder
        gr::fec::code::cc_decoder_impl cc_decoder_in;
        gr::fec::depuncture_bb_impl depunc;

        // Repacker
        RepackBitsByte repacker;

        float getBER(uint8_t *input);

        void char_array_to_uchar(const char *in, unsigned char *out, int nsamples)
        {
            for (int i = 0; i < nsamples; i++)
            {
                long int r = (long int)rint((float)in[i] * 127.0);
                if (r < 0)
                    r = 0;
                else if (r > 255)
                    r = 255;
                out[i] = r;
            }
        }

    public:
        MetopViterbi2(float ber_threshold, int outsync_after, int buffer_size);
        ~MetopViterbi2();

        int work(uint8_t *input, size_t size, uint8_t *output);
        float ber();
        int getState();
    };
} // namespace npp