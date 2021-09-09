/*
 * lora::rfm9x for Raspberry Pi Pico
 *
 * Adapted from
 * https://github.com/adafruit/Adafruit_CircuitPython_RFM9x/blob/4490961b28a129d2f6435c73d2bd65d2b042224c/adafruit_rfm9x.py
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _RFM9X_HEADER_
#define _RFM9X_HEADER_

using std::vector;
using std::string;


// Register names (FSK Mode even though we use LoRa instead, from table 85
#define RH_RF95_REG_00_FIFO                             0x00
#define RH_RF95_REG_01_OP_MODE                          0x01
#define RH_RF95_REG_06_FRF_MSB                          0x06
#define RH_RF95_REG_07_FRF_MID                          0x07
#define RH_RF95_REG_08_FRF_LSB                          0x08
#define RH_RF95_REG_09_PA_CONFIG                        0x09
#define RH_RF95_REG_0A_PA_RAMP                          0x0A
#define RH_RF95_REG_0B_OCP                              0x0B
#define RH_RF95_REG_0C_LNA                              0x0C
#define RH_RF95_REG_0D_FIFO_ADDR_PTR                    0x0D
#define RH_RF95_REG_0E_FIFO_TX_BASE_ADDR                0x0E
#define RH_RF95_REG_0F_FIFO_RX_BASE_ADDR                0x0F
#define RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR             0x10
#define RH_RF95_REG_11_IRQ_FLAGS_MASK                   0x11
#define RH_RF95_REG_12_IRQ_FLAGS                        0x12
#define RH_RF95_REG_13_RX_NB_BYTES                      0x13
#define RH_RF95_REG_14_RX_HEADER_CNT_VALUE_MSB          0x14
#define RH_RF95_REG_15_RX_HEADER_CNT_VALUE_LSB          0x15
#define RH_RF95_REG_16_RX_PACKET_CNT_VALUE_MSB          0x16
#define RH_RF95_REG_17_RX_PACKET_CNT_VALUE_LSB          0x17
#define RH_RF95_REG_18_MODEM_STAT                       0x18
#define RH_RF95_REG_19_PKT_SNR_VALUE                    0x19
#define RH_RF95_REG_1A_PKT_RSSI_VALUE                   0x1A
#define RH_RF95_REG_1B_RSSI_VALUE                       0x1B
#define RH_RF95_REG_1C_HOP_CHANNEL                      0x1C
#define RH_RF95_REG_1D_MODEM_CONFIG1                    0x1D
#define RH_RF95_REG_1E_MODEM_CONFIG2                    0x1E
#define RH_RF95_REG_1F_SYMB_TIMEOUT_LSB                 0x1F
#define RH_RF95_REG_20_PREAMBLE_MSB                     0x20
#define RH_RF95_REG_21_PREAMBLE_LSB                     0x21
#define RH_RF95_REG_22_PAYLOAD_LENGTH                   0x22
#define RH_RF95_REG_23_MAX_PAYLOAD_LENGTH               0x23
#define RH_RF95_REG_24_HOP_PERIOD                       0x24
#define RH_RF95_REG_25_FIFO_RX_BYTE_ADDR                0x25
#define RH_RF95_REG_26_MODEM_CONFIG3                    0x26

#define RH_RF95_REG_40_DIO_MAPPING1                     0x40
#define RH_RF95_REG_41_DIO_MAPPING2                     0x41
#define RH_RF95_REG_42_VERSION                          0x42

#define RH_RF95_REG_4B_TCXO                             0x4B
#define RH_RF95_REG_4D_PA_DAC                           0x4D
#define RH_RF95_REG_5B_FORMER_TEMP                      0x5B
#define RH_RF95_REG_61_AGC_REF                          0x61
#define RH_RF95_REG_62_AGC_THRESH1                      0x62
#define RH_RF95_REG_63_AGC_THRESH2                      0x63
#define RH_RF95_REG_64_AGC_THRESH3                      0x64

#define RH_RF95_DETECTION_OPTIMIZE                      0x31
#define RH_RF95_DETECTION_THRESHOLD                     0x37

#define RH_RF95_PA_DAC_DISABLE                          0x04
#define RH_RF95_PA_DAC_ENABLE                           0x07

// The crystal oscillator frequency of the module
#define RH_RF95_FXOSC                                   32000000.0

// The Frequency Synthesizer step RH_RF95_FXOSC / 2^^19
#define RH_RF95_FSTEP                                   RH_RF95_FXOSC / 524288

// RadioHead specific compatibility constants.
#define RH_BROADCAST_ADDRESS                            0xFF

// The acknowledgement bit in the FLAGS
// The top 4 bits of the flags are reserved for RadioHead.
// The lower 4 bits are reservedfor application layer use.
#define RH_FLAGS_ACK                                    0x80
#define RH_FLAGS_RETRY                                  0x40

#define SLEEP_MODE                                      0
#define STANDBY_MODE                                    1
#define FS_TX_MODE                                      2
#define TX_MODE                                         3
#define FS_RX_MODE                                      4
#define RX_MODE                                         5


class RFM9x {

    public:
        // Constructor
        RFM9x(uint8_t reset_pin, double frequency);

        // Methods
        void            reset();
        void            idle();
        void            sleep();

        void            enable_crc(bool state);
        bool            is_crc_enabled(void);

        double          get_rssi();
        double          get_snr();

        uint32_t        tx_done();
        uint32_t        rx_done();
        uint32_t        crc_error();

        bool            send(uint8_t* data, uint32_t length, bool keep_listening = true);
        void            listen();
        void            transmit();

        // Properties
        // Properties with initial values
        bool            high_power = true;
        bool            state = false;
        double          last_rssi = 0.0;
        double          last_snr = 0.0;
        double          ack_wait = 0.5;
        double          xmit_timeout = 2.0;
        uint32_t        ack_retries = 5;
        double          ack_delay = 0.0;
        uint64_t        sequence_number = 0;
        uint8_t         node = RH_BROADCAST_ADDRESS;
        uint8_t         destination = RH_BROADCAST_ADDRESS;
        // Contains seq count for reliable datagram mode
        uint64_t        identifier = 0;
        // Identifies ack/reetry packet for reliable datagram mode
        uint8_t         flags = 0;
        uint32_t        crc_error_count = 0;



    private:
        // Methods
        uint32_t        get_preamble_length(void);
        void            set_preamble_length(uint32_t length);

        float           get_frequency();
        void            set_frequency(double value);

        uint32_t        get_tx_power();
        void            set_tx_power(uint32_t value);

        uint32_t        get_coding_rate(void);
        void            set_coding_rate(uint32_t rate);

        uint32_t        get_spreading_factor(void);
        void            set_spreading_factor(uint32_t factor);

        void            set_signal_bw(uint32_t value);

        // Properties
        uint32_t        bw_bins[9] = {7800, 10400, 15600, 20800, 31250, 41700, 62500, 125000, 250000};
        uint8_t         seen_ids[256];
        uint8_t         rst_pin;

        // Proprties that are REG_BIT instances
        REG_BITS        op_mode = REG_BITS(RH_RF95_REG_01_OP_MODE, 0, 3);
        REG_BITS        low_freq_mode = REG_BITS(RH_RF95_REG_01_OP_MODE, 3, 1);
        REG_BITS        modulation_type = REG_BITS(RH_RF95_REG_01_OP_MODE, 5, 2);
        // Long range/LoRa mode can only be set in sleep mode!
        REG_BITS        long_range_mode = REG_BITS(RH_RF95_REG_01_OP_MODE, 7, 1);
        REG_BITS        output_power = REG_BITS(RH_RF95_REG_09_PA_CONFIG, 0, 4);
        REG_BITS        max_power = REG_BITS(RH_RF95_REG_09_PA_CONFIG, 4, 3);
        REG_BITS        pa_select = REG_BITS(RH_RF95_REG_09_PA_CONFIG, 7, 1);
        REG_BITS        pa_dac = REG_BITS(RH_RF95_REG_4D_PA_DAC, 0, 3);
        REG_BITS        dio0_mapping = REG_BITS(RH_RF95_REG_40_DIO_MAPPING1, 6, 2);
        REG_BITS        auto_agc = REG_BITS(RH_RF95_REG_26_MODEM_CONFIG3, 2, 1);
        REG_BITS        low_datarate_optimize = REG_BITS(RH_RF95_REG_26_MODEM_CONFIG3, 3, 1);
        REG_BITS        lna_boost_hf = REG_BITS(RH_RF95_REG_0C_LNA, 0, 2);
        REG_BITS        auto_ifon = REG_BITS(RH_RF95_DETECTION_OPTIMIZE, 7, 1);
        REG_BITS        detection_optimize = REG_BITS(RH_RF95_DETECTION_OPTIMIZE, 0, 3);
};


#endif  // _RFM9X_HEADER_